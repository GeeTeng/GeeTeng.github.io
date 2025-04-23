---
title: "Lyra动画蓝图——角色基础移动"
date: 2025-04-22
description: 角色的基础移动
tags: [UE5]
showDate: true
math: true
chordsheet: true
---

























































首先将lyra的动画导入到项目中，一共需要创建如下动画蓝图和动画层接口。

并勾选所需插件Animation Locomotion Library和Animation Warping。

![01](/images/UE/LyraAnimation/01.png)

## CharacterBase

CharacterBase用于绑定角色蓝图的动画

Lyra中用了**`BlueprintThreadSafeUpdateAnimation`**线程安全的动画更新方法，用于在工作线程中执行高性能的动画计算，避免阻塞游戏线程。

![02](/images/UE/LyraAnimation/02.png)

![03](/images/UE/LyraAnimation/03.png)

这三个函数分别用来更新角色的朝向和转向、更新速度大小处理走跑和加速度数据起步和急停，需要勾选线程安全。

![04](/images/UE/LyraAnimation/04.png)

`GetMovementComponent`函数用来返回角色的移动组件，用于`Update Acceleration Data`函数中获取角色当前加速度。

![05](/images/UE/LyraAnimation/05.png)

动画事件中，四个状态：Idle、Start（开始走）、Cycle（行走循环）、Stop（停止走）

![06](/images/UE/LyraAnimation/06.png)

- Idle-->Start：有加速度时，提升为共享ToStart，Blend Profile混合配置为FastFeet(细节，脚先移动)
- Start-->Cycle：勾选Automatic Rule Based on Sequence Player In State（自动播放到下一个动画）
- Cycle-->Stop：没有加速度时，提升为共享ToStart
- Stop-->Idle：勾选Automatic Rule Based on Sequence Player In State
- Stop-->Start和Start-->Stop：使用共享

## LayerInterface

接下来新建动画层接口，在动画层接口中新建4个动画层，分别是`FullBody_Idle`、`FullBody_Start`、`FullBody_Cycle`、`FullBody_Stop`。

点击Idle等状态机，将动画层输出。

![08](/images/UE/LyraAnimation/08.png)

## LayersBase

创建动画序列连接到输出，并且stop和start将LoopAnimation取消

![07](/images/UE/LyraAnimation/07.png)

## PistolLayer

新建LayersBase子蓝图

可以在右侧面板对动画赋值

![09](/images/UE/LyraAnimation/09.png)

之后在角色蓝图中新建动画层图标，运行PistolLayer，将网格体动画蓝图和这个动画层关联起来，同时PistolLayer又继承了LayerInterface，CharacterBase又关联了LayerInterface。

![10](/images/UE/LyraAnimation/10.png)

此时动画已经可以播放了，但是出现问题发现角色移动跑到一个地方又退回来，发现是根动画的原因。

将动画序列通过属性矩阵进行批量编辑，RootMotion启用根运动、强制根锁定。

<video controls width="100%">
  <source src="/images/UE/LyraAnimation/11.mp4" type="video/mp4">
  您的浏览器不支持视频标签。
</video>
