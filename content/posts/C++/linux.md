







## 静态库 动态库

静态库：**程序在编译时会把库文件的二进制编码链接到目标程序中**。

如果多个程序中用到了同一静态库中的函数，就会存在多份拷贝。

生成静态库

```c++
g++ -c -o libpublic.a public.cpp
```

**编译 `demo.cpp` 并链接一个名为 `public` 的库**

```c++
g++ -o demo demo.cpp -L/home/test1/codes/tools -lpublic
```



动态库（共享库）：**编译时不会把库文件的二进制代码链接到目标程序中，运行时候才会被载入**。

如果多个进程在用到同一动态库中的函数 / 类，那么在内存中只有一份，避免了空间浪费的问题。

程序升级比较简单 不需要重新编译 只需要更新动态库。

如果静态库和动态库同时存在，会优先使用动态库。

生成动态库

```c++
g++ -gPIC -shared -o libpublic.so public.cpp
```

编译并链接动态库

```c++
g++ -o demo demo.cpp -L/home/test1/codes/tools -lpublic
```

发现执行会报错显示找不到这样的文件 原因是需要设置好环境变量



使用make来生成库，可以避免写呢么多代码。

```makefile
# 指定编译的目标文件是 libpublic.a 和 libpublic.so
all:libpublic.a \
    libpublic.so
# 编译 libpublic.a 需要依赖 public.h 和 public.cpp
# 如果被依赖文件内容发生了变化，将重新编译 libpublic.a
libpublic.a:public.h public.cpp
        g++ -c -o libpublic.a public.cpp
libpublic.so:public.h public.cpp
        g++ -fPIC -shared -o libpublic.so public.cpp
# clean 用于清理编译目标文件，仅在 make clean 才会执行。
clean:
        rm -f libpublic.a libpublic.so
```



## main的参数

main函数有三个参数，`argc`、`argv`和`envp`，它的标准写法如下：

```c++
int main(int argc,char *argv[],char *envp[])
```

`argc`   存放了程序参数的个数，包括程序本身。

`argv`   字符串的数组，存放了每个参数的值，包括程序本身。

`envp`  字符串的数组，存放了环境变量，数组的最后一个元素是空。