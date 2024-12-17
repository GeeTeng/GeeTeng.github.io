---
title: "Shader"
date: 2024-12-16
tags: [Computer-Graphics]
description: "shader未完结"
showDate: true
math: true
chordsheet: true
---

### Z-Buffer(深度缓存）

用来处理可见性问题，即**确定每个像素最前层物体**。

**Frame Buffer**(帧缓冲区：用于存储最终图像数据)、**Depth Buffer**（深度缓冲区：判断哪些物体在前，哪些物体被遮挡）

![Z-buffer](/images/Shader/Z-buffer.png)

工作原理：近处是更黑的，远处是发白的。

Z-Buffer处理不了透明物体，透明物体需要特殊处理。

#### Z-Buffer Algorithm

![code](/images/Shader/code.jpg)



![Z-BufferAlgorithm](/images/Shader/Z-BufferAlgorithm.png)

R代表无穷大，如果有三角形，就覆盖掉这些无穷大的像素

新的三角形深度是8 肯定要被原本三角形深度5所遮挡 所以就忽略它

如果新的三角形离的更近 更新这个像素对应的深度值



**Z-Buffer复杂度 - O(n)**

> **Question：**为什么排序n个三角形是在线性的时间？
>
> **Answer：**其实它不是排序，只是在对每个像素一直求最小值。

---

### Shader

**定义：对不同的物体应用不同材质的过程**

物体在经过MVP（模型、视图、投影矩阵变换）、ViewPort之后现在我们会得到一系列旋转的立方体

需要对他做着色（Shading）



#### Blinn-Phong 反射模型

Blinn-Phong 反射模型是计算光照和表面反射的常用模型。它将光照分为三部分：

- **漫反射（Diffuse Reflection）**：表示光照与物体表面相互作用后，光线在不同方向上散射的现象，通常效果比较均匀，不会有明显的亮点。
- **镜面反射（Specular Highlights）**：表示光线被物体表面镜面反射的部分，产生亮光点，通常依赖于视角与光源方向的关系，产生较强的高光效果。
- **环境光照（Ambient Lighting）**：表示全局环境中的光照，通常是一个常量，作用于整个场景，不依赖于具体的物体或光源。



##### diffuse reflection

通常假设反射光是一个半球，并计算每个着色点（shading point）接收到的能量。漫反射的强度与光照方向和表面法线之间的夹角有关。

**Lambert‘s cosine law：描述漫反射光照强度与光照方向和法线之间夹角的关系**，公式如下：
$$
L_{d}=k_{d}\left(I / r^{2}\right) \max (0, \mathbf{n} \cdot \mathbf{l})
$$

$$
L_d:漫反射分量（反射光强度）\qquad k_d:漫反射系数（物体表面材质的属性）\qquad I:光源强度\qquad r:光源到表面点的距离 \qquad n:法线方向 \qquad l:光源方向
$$



**注意**：漫反射不考虑其他物体的影响，只关注当前着色点的表面法线与光照方向的关系。