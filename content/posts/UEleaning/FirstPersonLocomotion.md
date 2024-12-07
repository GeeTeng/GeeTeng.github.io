---
title: "FirstPersonLocomotion"
date: 2024-12-04
tags: [UE5]
description: "第一人称角色移动 提升游戏角色的运动表现和玩家的操作体验"
showDate: true
math: true
chordsheet: true
---

### 第一人称移动

#### 1 二段跳

`DoN`是Do Once Node，主要用于确保某个操作在特定条件下只执行一次，**避免多次触发**。

**`LaunchCharacter`** 的作用是用来给角色施加一个即时的力（速度向量），通常用于实现跳跃、飞行、冲刺等效果。它通过改变角色的速度或者施加一个加速度，使得角色在特定方向上做出瞬时的移动， Z方向900的速度。

**`EventOnLanded`** 是 Unreal Engine 中的一种事件，通常用于在角色或物体着陆时触发特定的操作或行为。

设置后发现第二段跳跃会下降速度慢，所以更改CharacterMovement右侧面板中的GravityScale值为更大。

![Jump](/images/UE学习/人物移动/Jump.png)

---

#### 2.加速带

新增一个检测与加速带接触的胶囊体CapsuleCollision，调整大小为角色碰撞体大小。新增一个Boolean值isWallRunning代表是在加速状态，Vector向量代表角色方向

如果碰撞到tag为加速带并且不在地面上（isFalling状态）的时候就执行加速动作，设置重力大小为0，速度为2000。

如果离开加速带的时候，停止加速，同时要恢复重力。

**`Sequence`（序列）** 是一种控制流节点，它允许你在一个蓝图中按顺序执行多个动作。Sequence 节点会按照顺序依次执行它连接的输出，并且可以输出多个执行线路。

**`playerDirection`** 通常是一个用来表示玩家（Player）朝向或方向的变量。

判断加速带方向向量和角色方向向量叉乘是否<0，代表是加速带的反方向。>0的时候会返回1，<0会返回-1，将结果与加速带的方向向量相乘。如果与加速带方向相同则向加速带方向加速，如果与加速带方向相反，就往反方向加速。

![WallRunning01](/images/UE学习/人物移动/WallRunning01.png)

![WallRunning02](/images/UE学习/人物移动/WallRunning02.png)

设置在游戏开始时的重力为Old Gravity Scale 为了方便加速后恢复原始速度。

![BeginPlay](/images/UE学习/人物移动/BeginPlay.png)

新建加速带的左碰撞体右碰撞体，原因是在加速带上的时候，角色是垂直的，绝对会有一侧碰撞体碰撞到加速带。

这个时候插值过渡一下摄像机的方向转化动画，新建一个Float Track

**`Get Control Rotation`** 用于调整摄像机的视角。

**`Get Actor Rotation`** 用于调整角色的实际朝向。

![RotateCameraTimeLine](/images/UE学习/人物移动/RotateCameraTimeLine.png)

![CameraRotateOnWallRun](/images/UE学习/人物移动/CameraRotateOnWallRun.png)