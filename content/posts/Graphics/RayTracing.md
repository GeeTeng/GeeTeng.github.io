---
title: "RayTracing光线追踪"
date: 2025-01-03
tags: [Computer-Graphics]
description: "光线追踪 更新中"
showDate: true
math: true
chordsheet: true
---



### ShadowMapping（阴影贴图）

1. 从光源视角生成深度图，存储深度值。
2. 主摄像机渲染场景并计算阴影，与存储的深度值进行比较，如果当前点深度大于阴影贴图深度，说明该点被遮挡是阴影点；否则是光照射的点。

![01](/images/RayTracing/01.png)

**缺点：**

- 生成的是硬阴影（点光源）
- 效果取决于shadowmap分辨率
- 浮点精度比较问题



由于点光源有大小，会形成如图所示的（Umbra）本影区域和半影（Penumbra）区域。所以会形成阴影的过渡。

![02](D:\GeeSite\static\images\RayTracing\/images/RayTracing/02.png)

> Question：为什么需要光线追踪？
>
> Answer：光栅化难以做软阴影、难以表现光线多次弹射、光栅化（实时 游戏 快），光线追踪（离线 动画 慢）