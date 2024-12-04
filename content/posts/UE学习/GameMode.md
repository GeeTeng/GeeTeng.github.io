---
title: "GameMode游戏模式"
date: 2024-11-23
tags: [UE5]
description: "课程笔记及作业"
showDate: true
math: true
chordsheet: true
---

---

#### 平行世界？World

UE中有三个世界

- 游戏世界 
- 编辑器世界 
- 预览世界

关卡蓝图-定义关卡规则

`UGameInstance`  存在于游戏的整个生命周期中 不会随着地图切换 销毁 适合业务全局管理操作，Uengine  管理 GameInstance

---

#### **游戏中最重要的实体AActor**

AActor 标志所有权的Owner指针，标志本地权限的Role枚举，作为网格同步的基础单位，根组件提供世界变化信息

Actor不只是3D中的表示，一些不可见对象也可以是Actor，如*AInfo*（派生类*Aworldsetting*，*AGameMode*，*AgameState*...）

**Actor在UE中就像是世界里的元素**

**Component 是 Actor 的组成部分，用于定义 Actor 的特性和行为**

1. *RootComponent*是Actor的核心组件，决定了Actor在场景中的位置、旋转和缩放
2. *SceneComponent* 是一个基础组件，主要用于定义场景中对象的位置、旋转和缩放。它本身是一个抽象组件，不会直接渲染或显示。

**Actor生命周期详解**

- 生命周期1

  actor分为

  - 静态的actor - 在关卡编辑器里直接放置的Actor  
  - 动态创建的actor（*spawnActor*） - 通过代码或蓝图在运行时动态生成的 Actor

- 生命周期2
  重要的生命周期函数

  BeginPlay - 在Actor激活时调用 初始化

  Tick - 每帧调用一次 处理逻辑

  EndPlay - 在Actor即将销毁时调用 清理资源

  

- 生命周期3
  GC完成收尾 注意有效性的判断

  ```c++
  if (IsValid(MyActor)) {    // Actor 有效 }
  
  if (MyActor && !MyActor->IsPendingKill()) {    // Actor 没有被标记为销毁 }
  ```

  *isPendingKill*是否已被标记为即将销毁，但尚未被垃圾回收系统（GC）完全移除。

---

#### **APawn（可操控的棋子)**

最重要的特性：可以被controller控制 提供了输入、移动框架的支持

UE提供了一个默认的Pawn - **DeflaultPawn** 

**默认三件套**

- ***DefaultPawnMovementComponent*** 
- **sphericalCollisionComponent**
-  ***StaticMeshComponent*** 



***SpectatorPawn*** 

观众想观战但并不需要真正的表示他们 所以提供了**USpectatorPawnMovement**（不带重力漫游） 并关闭了StaticMesh的显示，碰撞设置到了Spectator通道

---

**Character像是Pawn的加强特化版本**

**ACharacter**人形角色（人形Pawn）

- 近似仿真人形的胶囊体碰撞盒***UCapsuleComponent***

- 骨骼模型***USkeletalMeshComponent***

- 人物移动组件***UCharacterMovementComponent***

  - 配合胶囊体完成了各种仿真移动计算Walking、Falling...

  - 提供了Custom自定义移动模式供扩展

  - 网络游戏移动同步架构 主控端预表现（网断了还能跑）服务器端校验 模拟端预测

---

#### **MVC模式（显示、数据、算法）** 

**显示（View）**：

- 由骨骼模型和动画组件（`USkeletalMeshComponent`）负责呈现角色的外观。
- 包括实时动画播放、物理效果展示等。

**数据（Model）**：

- 角色的核心属性和状态（如速度、位置、旋转、是否跳跃）。
- 包括状态同步的网络消息结构和角色实例化的变量集合。

**算法（Controller）**：

- 负责移动逻辑、动画状态切换、物理计算等行为。
- 包括人物移动组件的各种模式扩展与自定义处理逻辑。

抽象这三个变化并归纳关系就是MVC模式



#### **继承和层次结构**

1. **AController 继承自 AActor**：
   - AController 是一个专门用于控制 APawn 行为的类。
   - 它充当“提线木偶操控者”，通过指令驱动角色的移动、行为和其他逻辑。
2. **UObject**：
   - Unreal Engine 中最基础的类，所有对象的根基。
   - **功能**：提供反射、序列化、垃圾回收（GC）和内存管理的机制。
   - **反射机制**：支持运行时类型信息 (RTTI) 和蓝图扩展。
   - **序列化**：用于保存和加载对象状态，广泛用于游戏保存、配置文件等。
3. **UActorComponent**：
   - **功能载体**：AActor 的组件化架构。
   - 逻辑代码或功能模块的载体（如健康系统、AI逻辑等）。
   - **嵌套能力**：USceneComponent 提供世界变换（位置、旋转、缩放）以及嵌套结构支持。
4. **AActor**：
   - 游戏对象的基础类，是组件的容器。
   - **功能**：管理生命周期（Spawn/Destroy）、事件（Tick/Overlap）以及和世界的交互（物理、渲染等）。
5. **APawn**：
   - AActor 的子类，表示玩家或 AI 可控制的实体。
   - 特点：
     - 提供基本的物理表示（如碰撞体）。
     - 支持玩家输入和基本移动能力。
     - 可被 AController 绑定以实现复杂逻辑。
   - 当前常用于角色控制和 AI 代理。

------

#### **具体功能与模块**

**APlayerController（提线木偶操控者）**：

- **定义**：玩家的控制代理，负责接收输入并将其转换为游戏行为。
- **职责：**
  - 处理输入：将玩家输入映射到动作或功能。
  - 控制 Pawn：通过控制接口驱动角色行为。
  - 管理玩家视角：负责管理摄像机、HUD 和其他 UI 组件。
- **网络连接与所有权**：
  - 在多人游戏中，APlayerController 维护客户端和服务器之间的连接。
  - 确保特定 APawn 的所有权归特定客户端（或服务器）所有。

**UInputComponent**：

- **功能**：用于绑定输入映射，将玩家输入转化为游戏事件。
- **特点**：
  - 支持蓝图和 C++ 输入绑定。
  - 使用输入映射表（Input Mapping Context）实现灵活的按键配置。

**APlayerCameraManager**：

- **职责**：
  - 管理玩家的摄像机视角和摄像机效果。
  - 支持视角切换（第一人称/第三人称）。
  - 应用于动态摄像机逻辑，如抖动、跟随目标、动态缩放等。

**`AHUD`**：

- **定义**：传统的屏幕 UI 渲染类。
- **特点**：
  - 早期用于绘制简单的屏幕元素（如血量、雷达等）。
  - 被更灵活的 UMG 取代，但仍可用作低级别绘制需求。
- **区别**：
  - HUD 是屏幕上的信息展示，通常绑定到玩家控制器。
  - UI 更复杂，可通过 UMG 实现交互式用户界面。

------

#### AGameMode 真·游戏模式

玩家如何加入游戏 游戏特定行为（获胜）
所有GameMode均为 AGameMode的子类 AGameModeBase包含大量可覆盖的基础功能

---

#### AgameState游戏状态

同mode基类子类

APlayerState玩家状态 
playerState Character、Controller的职责区别

---

#### Question？

> 如果你来设计游戏，以下数据应该存放在哪里，是否需要同步给玩家？
> 玩家血量、玩家得分、队友位置、毒圈空头信息

**1.玩家血量**：

玩家血量是游戏的核心状态 需要在服务器存储并管理（防止作弊）

需要实时同步给本地的UI更新，但是不需要同步给敌人玩家，在团队作战中可以同步给队友。

**2.玩家得分：**

玩家得分是游戏结果的重要数据，需要服务器去存储，也需要同步给玩家的UI，但是客户端仅需要展示不需要存储。自己的得分需要同步，其他玩家可以定期的同步，如排行榜。

**3.队友位置：**

存储在服务器和客户端

需要同步给同队玩家，不能同步给敌人

**4.空投信息：**

毒圈和空投是全局信息，在服务器存储，客户端需要同步给所有玩家

需要同步给所有玩家，同步半径变化，毒圈缩小等，

