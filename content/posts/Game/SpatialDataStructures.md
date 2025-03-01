---
title: "空间数据结构"
date: 2024-12-17
tags: [Game]
description: "空间划分的数据结构"
showDate: true
math: true
chordsheet: true
---

---

多人在线游戏中，由于玩家规模十分庞大，如果想通过计算每一个玩家与其他 n - 1 个玩家之间的信息，时间复杂度会达到n^2，性能会变得很低。于是需要正确的处理这些数据，优化性能。

![SpatialDataStructures](/images/Game/Grid/SpatialDataStructures.png)

## 包围体层次结构 (BVH, Bounding Volume Hierarchy)

层次包围盒树，根节点是一个大的包围盒，往下是其子包围盒。

**AABB（Axis-Aligned Bounding Box）** 是一种常用的 **包围盒**（Bounding Box）类型

在 **2D 空间** 中，AABB 可以表示为：

- Min = (x_min, y_min)
- Max = (x_max, y_max)

在 **3D 空间** 中，AABB 可以表示为：

- Min = (x_min, y_min, z_min)
- Max = (x_max, y_max, z_max)

![BVH](/images/Game/Grid/BVH.png)

在游戏引擎中，大部分物体都会动态更新，而层次包围盒树 / 层次球包围盒树可以动态更新，从叶节点一级一级的往上更新上面的节点，使包围体包住子节点。



## 四叉树（Quadtree)

四叉树基本思想是将 2D 空间递归划分成不同层次的树结构，将已知范围的空间等分成 4 个相等的空间，直到树的层次到达一定深度或满足要求后停止分割。

![grid](/images/Game/Grid/grid.png)

![quadtree](/images/Game/Grid/quadtree.png)

## 八叉树 (Octree)

八叉树与四叉树结构类似，拥有 8 个节点，适用于 3D 空间。应用于：碰撞检车、物体的空间位置等。

八叉树和四叉树的区别是一个用于二维空间，一个用于三维空间。但是当四维空间....更多维度的空间时该怎么办呢？总不能2的n次方去划分，于是引出了KD-Tree，一种跟维度没有关系，可以划分空间。

![Octree](/images/Game/Grid/Octree.png)



## k-d 树

k-d 树就是一种特殊形式的BSP树（轴对齐的BSP树），其每个节点都代表一个k维坐标点，树的每层都是对应一个划分维度。

构建 k-d 树将一组n个点按维度递归地划分成子空间，通常使用**中位数分割法**，所以构建树时间复杂度为O（n logn），最近邻搜索最优O（log n）

k-d 树应用：邻近对象查询，敌人寻找目标，玩家找NPC。

如图是一个二维的 k-d 树，第一层是x、第二层是y、第三层是x(交替的进行划分)。

![k-d](/images/Game/Grid/k-d.png)

对应空间如下：

![kdgrid](/images/Game/Grid/kdgrid.png)

如果我们要寻找（3，5）最近的目标，在绿色范围内是最近的目标，排除了（8，1）和（9，6）这两点，通过该算法可以剪枝一些不可能的子树。

![kd01](/images/Game/Grid/kd01.png)

![kd02](/images/Game/Grid/kd02.png)

## BSP (Binary Space Partitioning)

BSP 树是一棵二叉树，关键是递归地将空间划分为两部分（分区），每次分割都使用一个超平面，通过超平面可以有效地将三维空间分割成多个较小的部分。

二叉树结构：每个节点代表一个超平面。要构造出一棵尽可能平衡的 BSP 树，在每次添加节点（增加超平面）的时候，让其左右子树节点相差不多（超平面前方多边形数和后方相差不多）。

BSP 树构造的最坏时间复杂度为 O(N²logN) ，平均时间复杂度为 O(N²)。没有KD-Tree效率高，因为它的分区不是横平竖直的，计算起来麻烦。

BSP 应用：自动生成室内 portal（门户），进行额外的视野剔除...

![BSP](/images/Game/Grid/BSP.png)

- ## SceneGraph（场景图）

场景图是一种用于管理场景元素（如物体、光源、摄像机等数据结构），采用树形结构来描述对象之间的层次关系。

