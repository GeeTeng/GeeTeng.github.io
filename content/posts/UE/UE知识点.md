



### **垃圾回收**

> 如何保持新建的不被自动GC垃圾回收？

- 使用AddToRoot()函数可以保护对象不被自动回收，移除保护时使用RemoveFromRoot()并把对象指针置为nullptr即可由引擎自动回收；

```cpp
UMyObject* MyObject=NewObject<UMyObject>();
MyObject->AddToRoot();                     //保护对象不被回收
MyObject->RemoveFromRoot();
MyObject=nullptr;                          //交给引擎回收对象
```

- 使用UPROPERTY宏标记一下对象指针变量也可以保护对象不被自动回收，在该类被销毁时，新建的对象也会被引擎自动回收；

- 使用FStreamableManager加载资源时，将`bManageActiveHandle`设置为true也可以防止对象被回收；

```cpp
FSoftObjectPath AssetPaths(TEXT("[资源路径]"));
FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
TSharedPtr<FStreamableHandle> Handle = AssetLoader.RequestSyncLoad(AssetPath, true);//加载资源到内存中，bManageActiveHandle=true
```

### 反射

[UE -- UBT、UHT与反射基本理解 - 知乎](https://zhuanlan.zhihu.com/p/400473355)

C#中反射的定义是：运行中的程序查看本身的元数据或其他程序的元数据的行为。

C++本身不支持反射，也没有垃圾回收，因此UE自己实现了一套反射系统，通过反射可以实现序列化、垃圾回收GC、网络复制、蓝图通信等重要功能。简而言之就是把C++代码暴露给引擎，可以使用蓝图、序列化等功能。

因此UHT必须要把UE代码转换成C++代码，而UBT就是负责调用UHT做这件事情的。——**UBT本身不参与反射机制的实现，但是它负责调用UHT**。

**完整编译流程：**UBT搜集目录中的.cs文件，然后UBT调用UHT分析.h .cpp文件（根据文件是否含有#include"FileName.generated.h"，是否有UCLASS()、UPROPERTY等宏）生成**generated.h**和**gen.cpp**文件，最后UBT调用**MSBuild**，将.h.cpp和generated.h gen.cpp结合到一起然后编译。

![01](/images/UE/UE知识点/01.jpg)



### C++与蓝图对比

C++ or 蓝图

C++是编程语言，适合实现游戏底层系统；蓝图是脚本系统，适合定义高级交互、装饰性细节等。它没有一个明确的界限必须在什么时候使用蓝图或C++，但是两者可以很轻易的互通。

从源码构建项目时，C++会被编译为机器码。能够直接在CPU上运行的指令列表；而蓝图会由脚本编译器编译成脚本字节码，由引擎的脚本虚拟机在运行时执行。编译器会对C++进行一些优化，从而性能会更好，但是蓝图不会，所以消耗更大。因此复杂的数学计算或Tick这样被频繁调用的函数最好用C++实现。

虚幻提供了蓝图本地化（Blueprint Nativization）用来让蓝图转换成C++源代码，这也意味着不再通过虚拟机执行，而是会像C++一样被编译为原生机器代码。但是生成的代码不像C++一样容易阅读。

所以我们可以对蓝图节点中开销比较大的位置用C++重构。

C++中可以通过模块化管理类的依赖关系，但是蓝图没有模块化的概念，可以自由引用C++任何类，而C++类不会知道任何蓝图类型相关的信息。