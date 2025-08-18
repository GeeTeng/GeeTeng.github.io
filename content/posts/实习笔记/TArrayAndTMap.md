---
title: "数据结构与UE模块创建"
date: 2025-05-16
tags: [实习笔记]
description: "数据结构、UGit使用、UE模块创建"
showDate: true
math: true
chordsheet: true
---





- 数据结构Array、List、Stack、Queue、TArray、TMap
- UGit新增分支

- UE模块创建


# 附件：

 [List.cpp](/code/TArrayAndTMap/List.cpp)  [Queue.cpp](/code/TArrayAndTMap/Queue.cpp)  [Stack.cpp](/code/TArrayAndTMap/Stack.cpp)  [TArray.cpp](/code/TArrayAndTMap/TArray.cpp)  [TMap.cpp](/code/TArrayAndTMap/TMap.cpp) 

 [Array.cpp](/code/TArrayAndTMap/Array.cpp) 

# TArray

TArray类型由两大属性定义：元素类型和可选分配器（默认FDefaultAllocator）。

TArray只是内存的包装器，是对一块动态分配的连续内存进行管理。

比如你要一次性从文件或网络中读取 100 个 FMyStruct，而不想一个一个 Add，还要调用构造函数再赋值，影响效率。就可以使用AddUninitialized。

```c++
TArray<FMyStruct> MyArray;
MyArray.AddUninitialized(100); // 分配空间而不调用构造
 
// 假设你从磁盘中读了一块内存
FMemory::Memcpy(MyArray.GetData(), FileBuffer, 100 * sizeof(FMyStruct));
 
```

AddZeroed(int32 Count) 插入5个元素，但不会调用构造函数并且内存会被填充为0（memset）。适合纯数据结构体（POD）：结构体没有虚函数、继承、自定义构造和析构。比如FString就不是POD，因为管理堆内存、有构造和析构等。

SetNumUninitialized(int32 NewNum) 修改数组长度为100，扩容部分不会构造元素，只是扩展内存。

 

## Slack

Slack可调整数组的大小，为了避免每次添加元素都要重新分配内存，分配器提供的内存通常会超过必要内存，自认为和Vector一样。同样删除元素不会释放内存，此操作会是数组拥有Slack元素，也就是当前未使用的有效预分配元素储存槽。（可储存的元素数量 - 存储元素数量 = Slack）

## 堆

TArray拥有支持二叉堆数据结构的函数，调用Heapify函数可将现有数组转换为堆。



```c++
TArray<int32> HeapArr;
for (int32 Val = 10; Val != 0; --Val)
{
	HeapArr.Add(Val);
}
// HeapArr == [10,9,8,7,6,5,4,3,2,1]
HeapArr.Heapify();
// HeapArr == [1,2,4,3,6,5,8,10,7,9]
```

HeapPush 将新元素添加到堆，并重新排序维护堆。

HeapPop和HeapPopDiscard函数用于移除堆的顶部节点，前者会返回顶部元素副本，而后者仅移除节点。

HeapRemoveAt将删除数组中给定索引的元素，并重新排列元素维护堆。

可使用 HeapTop 检查堆的顶部节点。

```c++
TArray<int32> SlackArray;
	// SlackArray.GetSlack() == 0
	// SlackArray.Num()      == 0
	// SlackArray.Max()      == 0
 
	SlackArray.Add(1);
	// SlackArray.GetSlack() == 3 = 4 - 1
	// SlackArray.Num()      == 1
	// SlackArray.Max()      == 4
```

可以管理Slack对数组进行优化，比如要添加100个元素，呢么就要确保有100个Slack。

Empty就可以选择Slack参数。

```c++
MyArray.Empty(); // 清空并释放所有内存
MyArray.Empty(10); // 清空但预留 10 个元素空间
```

Shrink可以释放Slack，把容量压缩到正好存储当前元素数量。

Reserve可以预先分配好空间，防止Add后再分配。

 

## 插入

TArray中添加单个元素有：Add、Push、Emplace、AddUnique、Insert；前四种是在数组尾部追加元素，Insert可以在任意位置插入。

TArray 中的 Emplace 与 Add 的区别，基本和 C++ STL 中 std::vector 的 emplace_back 与 push_back 的区别是一致的：Emplace/emplace_back 是原地构造，Add/push_back 是先构造再拷贝或移动。

比如在TArray<FString>中，Add将用字符串文字创建临时FString，然后将该临时FString的内容移到容器内的新FString中，而Emplace可避免创建临时文件。由于FString非浅显，所以Emplace更好。

所以Add最好用于浅显类型而Emplace用于其他类型。



## 排序

TArray 的排序算法有三种，分别是 Sort快排、 HeapSort堆排序、 StableSort归并排序。



## 删除

TArray 的删除有 Remove、 RemoveSingle、 RemoveAll、 RemoveAt、 Empty（清空数组）。

 

## 查询

TArray 的查询有 [] 运算符、 Last、 Top。

判断元素是否存在： Contains是否包含、 ContainsByPredicate（自定义规则）。

查找元素所在的索引： Find返回第一个、 FindLast返回最后一个、 IndexOfByKey、 IndexOfByPredicate。

查找元素对象： FindByKey、 FindByPredicate、 FilterByPredicate。

 

# TMap

在STL的map中，使用 操作符[] 访问不存在的键时，会自动添加一个键值对元素，而在虚幻引擎TMap不同的是，操作符[] 访问了不存在的键会触发断言；

TMap 是 哈希表（Hash Map），它的底层实现基于哈希算法（类似于 C++ 标准库中的 std::unordered_map）TSet：基于哈希表的哈希集合，类似于 std::unordered_set。

## unordered_map 和 map的区别

|            | map      | unordered_map |
| ---------- | -------- | ------------- |
| 底层结构   | 红黑树   | 哈希表        |
| 是否排序   | 有序     | 无序          |
| 时间复杂度 | O(log N) | O(1)          |
| 内存使用   | 少       | 多            |

TMap 的底层实现依赖于 TMapBase<KeyType, ValueType, SetAllocator, bSparse>。

TMapBase 自己内部使用了一个 TSet<TPair<KeyType, ValueType>, ...> 来存储键值对。

也就是说：TMap 是通过包装一个内部的 TSet<TPair<Key, Value>> 来实现的，TMap 是通过将键值对作为元素插入一个哈希集合 TSet 来实现的。

```c++
TSet<TPair<KeyType, ValueType>, ...> Pairs;
```



## 查找机制：KeyFuncs

告诉 TMap 如何比较两个 Key 是否相等，用于查找、删除、插入等操作。

告诉 TMap 如何计算 Key 的哈希值，用于快速定位元素桶的位置。

 

## 哈希分布：Bucket 数组（哈希槽）

TSet 内部维护了一个 Bucket 数组（哈希槽）：

每个元素（TPair）根据 Key 的哈希值决定放在哪个槽；

哈希冲突时使用 **开放寻址法（Open Addressing）**；

 

## 删除和空位处理

当元素被删除时，为了保持探测序列连续，不是直接清空，而是标记为 “已删除”（Removed Entry）。

因此：

会有 “空槽”、“占用槽”、“已删除槽” 三种状态；

插入时跳过 “已删除槽”，找到第一个可用位置；

为了避免占满，TMap 定期会触发 Rehash（重新构建哈希表）

 

## 内存与增长策略（Rehash）

TSet 会根据元素数量动态扩容；

通常使用 2 的幂次作为桶的个数；

超过负载因子时会 Rehash：重新分配 Bucket，重新哈希所有元素。

举例说当你插入一个键值对到TMap中，实际上会用KeyFuncs获取Key的哈希值，然后TSet根据哈希值找到桶的位置，检查冲突。如果冲突则线性探测下一个位置，否则就直接插入。如果负载上线，就会自动再哈希。

TMap的Shrink可移除TMap中的全部Slack，Shrink只会在后端移除Slack，但是会在中间留下空白元素。

```c++
for (int32 i = 0; i < 10; i += 2)
{
		FruitMap.Remove(i);
}
FruitMap.Shrink(); // 
```

要移除所有slack，首先应使用Compact函数，将空白空间组合在一起，为调用 Shrink 做好准备。

 

# UGit新增本地分支

![img](/images/实习笔记/01.png)

 

# UE模块创建

[如何在虚幻引擎中创建Gameplay模块 | 虚幻引擎 5.5 文档 | Epic Developer Community](https://dev.epicgames.com/documentation/zh-cn/unreal-engine/how-to-make-a-gameplay-module-in-unreal-engine)

UE模块可以封装功能并隐藏代码

模块为单独的编译单元，所以只有已更改的模块才需要编译，对于大项目编译时间会缩短。未使用的模块会被排除在编译之外。可以控制在运行时何时加载和卸载某个模块，也可以基于为哪个平台编译的而引入或排除模块。

UE会根据项目中的Target.cs（Target.cs 决定了构建的目标Game、Editor、Server 等）和Build.cs构建项目，而不是IDE的解决方案文件sln构建。因为真正的构建系统是UBT（Unreal Build Tool）。

ModuleName.Build.cs 决定了每个模块的依赖、包含路径、PCH、链接方式等。

.uproject包含一个Modules列表，定义了哪些模块会纳入到项目中，以及模块加载的方式。

大部分Gameplay模块将直接列出名称（Name），而类型（Type） 将设置为 Runtime。

 

**UBT UHT**

UBT编译cs文件，调用UHT文件来处理带有UCLASS/UPROPERTY等的宏代码

**UBT本身不参与反射机制的实现，但是它负责调用UHT**。

完整编译流程：UBT搜集目录中的.cs文件，然后UBT调用UHT分析需要分析的.h .cpp文件（根据文件是否含有#include"FileName.generated.h"，是否有UCLASS()、UPROPERTY等宏）生成generated.h和gen.cpp文件，最后UBT调用MSBuild，将.h.cpp和generated.h gen.cpp结合到一起然后编译。

**反射**

[UE -- UBT、UHT与反射基本理解 - 知乎](https://zhuanlan.zhihu.com/p/400473355)

C#中反射的定义是：运行中的程序查看本身的元数据或其他程序的元数据的行为。

C++本身不支持反射，也没有垃圾回收，因此UE自己实现了一套反射系统，通过反射可以实现序列化、垃圾回收GC、网络复制、蓝图通信等重要功能。简而言之就是把C++代码暴露给引擎，可以使用蓝图、序列化等功能。

Demo.h

Gameplay模块要继承FDefaultGameModuleImpl

```c++
#pragma once
#include "CoreMinimal.h"
class FDemoModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
};
```

Demo.cpp

```c++
// Copyright Epic Games, Inc. All Rights Reserved.
 
#include "Demo.h"
void FDemoModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Demo module has started up."));
}
 
IMPLEMENT_GAME_MODULE(FDemoModule, Demo);
 
```

Demo.Build.cs

```c++
using UnrealBuildTool;
 
public class Demo : ModuleRules
{
    public Demo(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
 
        // 公共依赖模块（其他模块能访问）
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            
        });
 
        // 私有依赖模块（本模块内部用，其他模块不可访问）
        PrivateDependencyModuleNames.AddRange(new string[] {
            "AnimGraphRuntime",
            "EnhancedInput",
            "Slate",
            "SlateCore",
            "UMG",
            "GameplayAbilities",
            "GameplayTasks",
            "AdvancedWidgets",
            "AnimGraphRuntime",
            "NavigationSystem",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
        });
 
        // 如果是编辑器模块，可以这样判断：
        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "BJEditorTool",
            });
        }
 
        // 添加额外的公共包含路径
        PublicIncludePaths.AddRange(new string[] {
            "Demo",
        });
    }
}
```

在xxxEditor.Target.cs和xxx.Target.cs文件中添加该模块。

```c++
ExtraModuleNames.AddRange(new string[] { "PMS", "BJFramework", "BJEditorTool", "PMSGamePlay", "Demo" });
```

在Module中添加

```c++
{
	"Name": "Demo",
	"Type": "Runtime",
	"LoadingPhase": "Default",
	"AdditionalDependencies": [
		"Engine"
	]
}
```