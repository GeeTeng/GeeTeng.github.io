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



## 进程控制和同步

### linux的信号

**发送信号命令：**

kill -信号的类型 进程编号

killall -信号的类型 进程名

大部分操作默认信号15是终止进程，根据信号类型发送不同信号，比如1是挂起。

**重要的信号如下**：

| 信号名      | 信号值 | 默认处理动作 | 发出信号的原因                                       |
| :---------- | :----: | :----------: | :--------------------------------------------------- |
| **SIGINT**  | **2**  |    **A**     | **键盘中断Ctrl+c**                                   |
| **SIGKILL** | **9**  |   **AEF**    | **采用kill  -9 进程编号 强制杀死程序。**             |
| **SIGSEGV** | **11** |   **CEF**    | **无效的内存引用（数组越界、操作空指针和野指针等）** |
| **SIGALRM** | **14** |    **A**     | **由闹钟alarm()函数发出的信号**                      |
| **SIGTERM** | **15** |    **A**     | **采用“kill  进程编号”或“killall 程序名”通知程序。** |
| **SIGCHLD** | **17** |    **B**     | **子进程结束信号**                                   |

处理动作一项中的字母含义如下：

- A 缺省的动作是终止进程。
- B 缺省的动作是忽略此信号，将该信号丢弃，不做处理。
- C 缺省的动作是终止进程并进行内核映像转储（core dump）。
- D 缺省的动作是停止进程，进入停止状态的程序还能重新继续，一般是在调试的过程中。
- E 信号不能被捕获。
- F 信号不能被忽略。



**进程对信号的处理方法有三种：**

1. 对该信号的处理采用系统的默认操作，大部分的信号的默认操作是终止进程。
2. 设置信号的处理函数，收到信号后，由该函数来处理。
3. 忽略某个信号，对该信号不做任何处理，就像未发生过一样。
4. 

`signal()`函数可以设置程序对信号的处理方式。

**函数声明：**`sighandler_t signal(int signum, sighandler_t handler);`

参数`signum`表示信号的编号（信号的值）。

参数`handler`表示信号的处理方式，有三种情况：

1. SIG_DFL：恢复参数signum信号的处理方法为默认行为。 
2. 一个自定义的处理信号的函数，函数的形参是信号的编号。
3. SIG_IGN：忽略参数signum所指的信号。

服务程序运行在后台，如果杀掉它不是个好办法，因为进程被杀的时候，是突然死亡，没有安排善后工作。如果向服务程序发送一个信号，服务程序收到信号后，调用一个函数，在函数中编写善后的代码，程序就可以有计划的退出。

**如果向服务程序发送0的信号，可以检测程序是否存活。**





### 进程终止

有8种方式可以中止进程，其中5种为正常终止，它们是：

1. 在main()函数用return返回；
2. 在任意函数中调用exit()函数；
3. 在任意函数中调用\_exit()或\_Exit()函数；(不会调用任何析构函数)
4. 最后一个线程从其启动例程（线程主函数）用return返回；
5. 在最后一个线程中调用pthread_exit()返回；

**main()函数中的return还会调用全局对象的析构函数**。

**exit()**表示终止进程，不会调用局部对象的析构函数**，只调用全局对象的析构函数。**



`atexit()` 函数用于在程序终止时注册一个回调函数，也就是终止函数。当程序退出时，操作系统会自动调用这些函数，以便进行一些清理工作，比如释放资源、关闭文件、打印日志等。

程序退出前，会按照相反的顺序调用这些终止函数。也就是说，先注册的函数会后被调用，后注册的函数会先被调用。



### 创建进程

使用pstree命令来查看进程树

如下图，所有系统进程都是由systemd创建的，有网络管理、用户账户、日志等进程。

![05](/images/C++/linux/05.png)

每个进程都有一个非负整数表示的唯一的进程ID。虽然是唯一的，但是进程ID可以复用。当一个进程终止后，其进程ID就成了复用的候选者。Linux采用**延迟复用算法**，让新建进程的ID不同于最近终止的进程所使用的ID。这样防止了新进程被误认为是使用了同一ID的某个已终止的进程。

在调用 `fork()` 时，操作系统会创建一个子进程，并将 `fork()` 的返回值分别返回给父进程和子进程。

- **父进程：`pid > 0`，返回的是子进程的 PID**
- **子进程：`pid == 0`，返回 0**

如下代码就是在main中调用fork后，创建了一个新的子进程来执行下面的代码。父进程 `sleep(1)`，让子进程先运行，子进程修改并不会影响到父进程。`fork()` **复制了整个进程的地址空间**。

子进程获得了父进程数据空间、堆和栈的副本（**子进程拥有的是副本，不是和父进程共享**）。

**在C++中看到的地址是虚拟地址，不是物理地址**。即使看到子进程和父进程地址是一样的，他们实际上也不是一块地方。

```c++
#include<iostream>
#include<unistd.h>
using namespace std;
int main()
{
   int bh = 0;
   string message = "haha";
   pid_t pid = fork();
   if(pid > 0)
   {
     sleep(1);
     cout << "fatherpid = " << pid << endl;
     cout << bh << message << endl;
   }
   else
   {
     bh = 1;
     message = "hehe";
     cout << "childpid = " << pid << endl;
     cout << bh << message << endl;
   }
}
/*
childpid = 0
1hehe
fatherpid = 3394
0haha
*/
```

`ps -ef|grep demo3`查看进程。

**fork()的两种用法**

1. 父进程复制自己，然后，父进程和子进程分别执行不同的代码。这种用法在网络服务程序中很常见，父进程等待客户端的连接请求，当请求到达时，父进程调用fork()，让子进程处理些请求，而父进程则继续等待下一个连接请求。

2. 进程要执行另一个程序。这种用法在Shell中很常见，子进程从fork()返回后立即调用exec。例子如下：

```c++
#include<iostream>
#include<unistd.h>
using namespace std;
int main() {
  if(fork() > 0) { //父进程持续执行
    while(true) {
      sleep(1);
      cout << "father is running.\n";
    }
  }
  else {
    sleep(10);
    cout << "child is running.\n";
    execl("/bin/ls", "/bin/ls", "-lt", "/api", (char*) NULL); // 换进程执行
    cout << "child end, exit. \n"; //这行不会被执行
  }
}
```

**vfork**让子进程先运行，在子进程调用exec或exit()之后父进程才恢复运行。



### 僵尸进程

- 如果父进程比子进程先退出，子进程将**被1号进程托管**（这也是一种让程序在后台运行的方法）。
- 如果子进程比父进程先退出，而**父进程没有处理子进程退出的信息，子进程将成为僵尸进程**。

> 僵尸进程有什么危害？

父进程如果处理了子进程退出的信息，内核就会释放这个数据结构；父进程如果没有处理子进程退出的信息，内核就不会释放这个数据结构，子进程的进程编号将一直被占用。系统可用的进程编号是有限的，如果产生了大量的僵尸进程，将因为没有可用的进程编号而导致系统不能产生新的进程。

> 如何避免僵尸进程？

1. 父进程通过`wait()`等函数等待子进程结束，在子进程退出之前，父进程将被阻塞待。

​	`pid_t wait(int *stat_loc);`

```c++
#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
using namespace std;
int main()
{
  if(fork() > 0)
  {
    int sts;
    pid_t pid = wait(&sts);
    cout << "exit child number is:" << pid << endl;
    if(WIFEXITED(sts))
    {
      cout << "normal exit, status is: " << WEXITSTATUS(sts) << endl;
    }
    else
    {
      cout << "error exit, status is: " << WTERMSIG(sts) << endl;
    }          
}
```

2. 如果父进程很忙，可以捕获SIGCHLD信号，在信号处理函数中调用wait()/waitpid()。

```c++
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using  namespace std;
void func(int sig)   // 子进程退出的信号处理函数。
{
  int sts;
  pid_t pid=wait(&sts);

  cout << "已终止的子进程编号是：" << pid << endl;

  if (WIFEXITED(sts)) { cout << "子进程是正常退出的，退出状态是：" << WEXITSTATUS(sts) << endl; }
  else { cout << "子进程是异常退出的，终止它的信号是：" << WTERMSIG(sts) << endl; }
}
int main()
{
  signal(SIGCHLD,func);  // 捕获子进程退出的信号。

  if (fork()>0)
  { // 父进程的流程。
    while (true)
    {
      cout << "父进程忙着执行任务。\n";
      sleep(1);
    }
  }
  else
  { // 子进程的流程。
    sleep(5);
    // int *p=0; *p=10;
    exit(1);
  }
}
```





### 多进程与信号

如下代码，父进程会不断地每隔五秒钟创建一个子进程。如果父进程终止，所有的子进程都会终止；如果子进程终止，则只有子进程一个终止。

`SIG_IGN`：忽略参数signum所指的信号

```c++
#include<iostream>
#include<unistd.h>
#include<signal.h>
using namespace std;

void FathEXIT(int sig);
void ChldEXIT(int sig);

int main()
{
  // 忽略全部信号 不希望被打扰
  for(int i = 1; i <= 64; i ++) signal(i, SIG_IGN);
  signal(SIGTERM, FathEXIT); // 获取这个信号就调用函数
  signal(SIGINT, FathEXIT);
  while(true)
  {
    if(fork() > 0)
    {
      sleep(5);
      continue; // 跳到循环开始 创建子进程
    }
    else
    {
      signal(SIGTERM, ChldEXIT);
      signal(SIGINT, SIG_IGN);
    }
    while(true)
    { // 子进程不断运行
      cout << "child process" << getpid() << "is running.\n";
      sleep(3);
      continue;
    }
  }
}
void FathEXIT(int sig)
{
  // 防止信号处理函数在执行过程中再次被信号中断
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  cout << "father process exit, sig = " << sig << endl;
  kill(0, SIGTERM);
  // 释放资源的代码
  exit(0);
}
void ChldEXIT(int sig)
{
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  cout << "child process " << getpid() << "exit, sig = " << sig << endl;
  // 释放资源的代码
  exit(0);
}                                                              25,5          65%
```

### 共享内存

多线程共享进程的地址空间，如果多个线程需要访问同一块内存，用全局变量就可以了。

在多进程中，**每个进程的地址空间是独立的，不共享的**，如果多个进程需要访问同一块内存，不能用全局变量，只能用共享内存。

线程使用互斥锁和条件变量来实现线程同步，进程使用信号量来实现进程同步。

用`ipcs -m`可以查看系统的共享内存，包括：键值（key），共享内存id（shmid），拥有者（owner），权限（perms），大小（bytes）。

用`ipcrm -m 0`删除共享内存。

![06](/images/C++/linux/06.png)

1. *shmget函数*

int shmget(key_t key, size_t size, int shmflg); 

其中key为键值16进制，唯一性，size是共享内存大小，shmflg是访问权限，例如0666|IPC_CREAT，0666表示全部用户对它可读写，IPC_CREAT表示如果共享内存不存在，就创建它。

返回值：成功返回共享内存的id（一个非负的整数），失败返回-1（系统内存不足、没有权限）

2. *shmat函数*

**该函数用于把共享内存连接到当前进程的地址空间**。

void *shmat(int shmid, const void *shmaddr, int shmflg);

其中shmid是shmget返回的共享内存id，shmaddr是共享内存连接到当前进程中的地址位置，通常填0，shmflg通常填0。

调用成功时返回共享内存起始地址，失败返回(void*)-1。

3. *shmdt函数*

**该函数用于将共享内存和进程分离，shmat函数的反操作。**

int shmdt(const void *shmaddr);

shmaddr  shmat()函数返回的地址，调用成功时返回0，失败时返回-1。

4. *shmctl函数*

**该函数用于操作共享内存，最常用的操作是删除共享内存。**

int shmctl(int shmid, int command, struct shmid_ds *buf);

command 操作共享内存的指令，如果要删除共享内存，填IPC_RMID。buf 操作共享内存的数据结构的地址，如果要删除共享内存，填0。

注意：**不能使用STL容器也不能使用移动语义**，只能用内置类型，因为STL容器会在堆区分配内存，不属于共享内存。

```c++
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
using namespace std;

struct person
{
  int no;
  char name[51];
};

int main(int argc, char* argv[])
{
  if(argc != 3) // 规范输入
  {
    cout << "using: ./demo no name\n"; 
    return -1;
  }
  int shmid = shmget(0x5005, sizeof(person), 0640|IPC_CREAT);
  if(shmid == -1)
  {
    cout << "shmget(0x5005) failed.\n";
    return -1;
  }
  cout << "shmid = " << shmid << endl;

  person* ptr = (person*)shmat(shmid, 0, 0);
  if(ptr == (void*)-1)
  {
    cout << "shmat() failed\n";
    return -1;
  }

  cout << "origin value: no = " << ptr->no << ",name = " << ptr->name << endl;
  ptr->no = atoi(argv[1]);
  strcpy(ptr->name, argv[2]);
  cout << "new value: no = " << ptr->no << ",name = " << ptr->name << endl;

  shmdt(ptr);

  if(shmctl(shmid, IPC_RMID, 0) == -1)
  {
    cout << "shmctl failed\n";
    return -1;
  }
}
```



### 循环队列

public.h

```c++
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
using namespace std;

template<class T, int MaxLength>
class squeue
{
private:
  bool m_inited;
  T m_data[MaxLength];
  int m_head;
  int m_tail;
  int m_length;
  squeue(const squeue &) = delete;
  squeue &operator=(const squeue &) = delete;
public:
  squeue() {init();}
  void init()
  {
    if(m_inited != true)
    {
      m_head = 0;
      m_tail = MaxLength - 1;
      m_length = 0;
      memset(m_data, 0, sizeof(m_data));
      m_inited = true;
    }
  }
  bool push(const T &e)
  {
    if(full() == true)
    {
      cout << "queue is full, failed to add." << endl;
      return false;
	}
    m_tail = (m_tail + 1) % MaxLength;
    m_data[m_tail] = e;
    m_length ++;
    return true;
  }
  int size()
  {
    return m_length;
  }
  bool empty()
  {
    if(m_length == 0) return true;
    return false;
  }
  bool full()
  {
    if(m_length == MaxLength) return true;
    return false;
  }
  T& front()
  {
    return m_data[m_head];
  }
  bool pop()
  {
    if(empty() == true) return false;
    m_head = (m_head + 1) % MaxLength;
    m_length --;
    return true;
  }
  void printqueue()
  {
    for(int i = 0; i < size(); i ++)
    {
      cout << "m_data["<< (m_head+i)%MaxLength << "],value=" \
             << m_data[(m_head+i)%MaxLength] << endl;
    }
  }
};   
```

demo2.cpp 循环队列的共享内存

```c++
#include"public.h"

int main()
{
  using ElemType = int;
  int shmid = shmget(0x5005, sizeof(squeue<ElemType, 5>), 0640|IPC_CREAT);
  if(shmid == -1)
  {
    cout << "shmget(0x5005) failed.\n";
    return -1;
  }
  squeue<ElemType, 5> *QQ = (squeue<ElemType, 5>*) shmat(shmid, 0, 0);
  if(QQ == (void*)-1)
  {
    cout << "shmat() failed.\n";
    return -1;
  }
  QQ->init();
  ElemType ee;      // 创建一个数据元素。

  cout << "元素（1、2、3）入队。\n";
  ee=1;  QQ->push(ee);
  ee=2;  QQ->push(ee);
  ee=3;  QQ->push(ee);

  cout << "队列的长度是" << QQ->size() << endl;
  QQ->printqueue();

  ee=QQ->front(); QQ->pop(); cout << "出队的元素值为" << ee << endl;
  ee=QQ->front(); QQ->pop(); cout << "出队的元素值为" << ee << endl;

  cout << "队列的长度是" << QQ->size() << endl;
  QQ->printqueue();

  cout << "元素（11、12、13、14、15）入队。\n";
  ee=11;  QQ->push(ee);
  ee=12;  QQ->push(ee);
  ee=13;  QQ->push(ee);
  ee=14;  QQ->push(ee);
  ee=15;  QQ->push(ee);

  cout << "队列的长度是" << QQ->size() << endl;
  QQ->printqueue();

  shmdt(QQ);  // 把共享内存从当前进程中分离。
}
```



## 网络编程

### socket函数详解

`int socket(int domain, int type, int protocol);`

成功返回一个有效的socket，失败返回-1，errno被设置。

单个进程中创建的socket数量与受系统参数open files的限制。（ulimit -a ）一般是1024。



*domain协议家族*

`PF_INET`      IPv4互联网协议族。

`PF_INET6`     IPv6互联网协议族。



*type数据传输的类型*

`SOCK_STREAM`    面向连接的socket：1）数据不会丢失；2）数据的顺序不会错乱；3）双向通道。

`SOCK_DGRAM`    无连接的socket：1）数据可能会丢失；2）数据的顺序可能会错乱；3）传输的效率更高。



*protocol最终使用的协议*

在IPv4网络协议家族中，数据传输方式为`SOCK_STREAM`的协议只有`IPPROTO_TCP`，数据传输方式为`SOCK_DGRAM`的协议只有`IPPROTO_UDP`。

```c++
socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  // 创建tcp的sock
socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);  // 创建udp的sock
```



### 主机字节序与网络字节序

为了解决不同字节序的计算机之间传输数据的问题，约定采用网络字节序（大端序）。
