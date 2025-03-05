---
title: "Linux学习"
date: 2025-03-02
tags: [C++]
description: "linux基础操作、进程控制、网络编程等"
showDate: true
math: true
chordsheet: true
---







## Linux基础操作

### 静态库 动态库

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



### main的参数

main函数有三个参数，`argc`、`argv`和`envp`，它的标准写法如下：

```c++
int main(int argc,char *argv[],char *envp[])
```

`argc`   存放了程序参数的个数，包括程序本身。

`argv`   字符串的数组，存放了每个参数的值，包括程序本身。

`envp`  字符串的数组，存放了环境变量，数组的最后一个元素是空。



### gdb调试

| **命令** | **简写** | **命令说明**                                                 |
| -------- | -------- | ------------------------------------------------------------ |
| set args |          | 设置程序运行的参数。                                         |
| break    | b        | 设置断点，b 20 表示在第20行设置断点，可以设置多个断点。      |
| run      | r        | 开始运行程序, 程序运行到断点的位置会停下来，如果没有遇到断点，程序一直运行下去。 |
| next     | n        | 执行当前行语句，如果该语句为函数调用，不会进入函数内部。 VS的F10 |
| step     | s        | 执行当前行语句，如果该语句为函数调用，则进入函数内部。VS的F11  注意了，如果函数是库函数或第三方提供的函数，用s也是进不去的，因为没有源代码，如果是自定义的函数，只要有源码就可以进去。 |
| print    | p        | 显示变量或表达式的值，如果p后面是表达式，会执行这个表达式。  |
| continue | c        | 继续运行程序，遇到下一个断点停止，如果没有遇到断点，程序将一直运行。  VS的F5 |
| set var  |          | 设置变量的值。                                               |
| quit     | q        | 退出gdb。                                                    |

**当程序运行过程中发生了内存泄漏，会被内核强行终止，提示“Segmentation fault (core dumped)“ 段错误（吐核），内存的状态将保存在core文件中**。 

![01](/images/C++/linux/01.png)

1. 用`ulimit -a`查看当前用户的资源限制参数；

2. 用`ulimit -c unlimited`把core file size改为unlimited；

3. 运行程序，产生core文件；

4. 运行gdb 程序名 core文件名；

5. 在gdb中，用bt查看函数调用栈。

通过调试core文件，可以查看到第8行有问题。还可以通过bt看到函数调用栈。

![02](/images/C++/linux/02.png)

> 如何调试正在运行中的程序？

必须要知道进程编号，`gdb 程序名 -p 进程编号`。然后程序就会停止下来，之后就可以正常调试了。

![03](/images/C++/linux/03.png)



### 时间操作

- `time_t`用于表示事件类型，是一个long类型的别名。`typedef long time_t`

- time()库函数，包含头文件time.h，有两种调用方法：

```c++
time_t now=time(0);      // 将空地址传递给time()函数，并将time()返回值赋给变量now。
time_t now; time(&now);  // 将变量now的地址作为参数传递给time()函数。
```

- tm结构体

```c++
struct tm
{
  int tm_year;	// 年份：其值等于实际年份减去1900
  int tm_mon;	// 月份：取值区间为[0,11]，其中0代表一月，11代表12月
  int tm_mday;	// 日期：一个月中的日期，取值区间为[1,31]
  int tm_hour; 	// 时：取值区间为[0,23]
  int tm_min;	// 分：取值区间为[0,59]
  int tm_sec;     	// 秒：取值区间为[0,59]
  int tm_wday;	// 星期：取值区间为[0,6]，其中0代表星期天，6代表星期六
  int tm_yday;	// 从每年的1月1日开始算起的天数：取值区间为[0,365] 
  int tm_isdst;   // 夏令时标识符，该字段意义不大
};
  // 根据tm结构体拼接成中国人习惯的字符串格式。
  string stime = to_string(tmnow.tm_year+1900)+"-"
               + to_string(tmnow.tm_mon+1)+"-"
               + to_string(tmnow.tm_mday)+" "
               + to_string(tmnow.tm_hour)+":"
               + to_string(tmnow.tm_min)+":"
               + to_string(tmnow.tm_sec);
```



### 文件操作

- 用`opendir()`函数打开目录。DIR *opendir(const char *pathname);

  成功-返回目录的地址，失败-返回空地址。

- 用`readdir()`函数**循环**的读取目录。struct dirent *readdir(DIR *dirp);

  成功-返回struct dirent结构体的地址，失败-返回空地址。

- 用`closedir()`关闭目录。

```c++
#include<iostream>
#include<dirent.h>
using namespace std;

int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    cout << "Using ./demo dir.\n";
    return -1;
  }
  DIR *dir;
  if((dir = opendir(argv[1])) == nullptr) return -1;

  struct dirent* stdinfo = nullptr;
  while(1)
  {
    if((stdinfo = readdir(dir)) == nullptr) break;
    cout << "dir name" << stdinfo->d_name << "type = " << (int)stdinfo->d_type <<endl;
  }
  closedir(dir);
}

```

`d_name`：文件名或目录名。

`d_type`：文件的类型，有多种取值，最重要的是8和4。8是常规文件，4是子目录文件（A directory）。注意，d_name的数据类型是字符，不可直接显示。

![04](/images/C++/linux/04.png)



### 系统错误

整型全局变量`errno`，存放了函数调用过程中产生的错误代码。配合 `strerror()` 和 `perror()` 两个函数

