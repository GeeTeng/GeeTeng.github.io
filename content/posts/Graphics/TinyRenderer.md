---
title: "TinyRenderer"
date: 2024-12-16
tags: [Computer-Graphics]
description: "TinyRenderer项目 更新中"
showDate: true
math: true
chordsheet: true
---

mainCRTStartup

```c++
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	for (float t = 0.f; t < 1; t += 0.01)
	{
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		image.set(x, y, color);
	}
}
```

![line](/images/TinyRenderer/line.png)

描绘了100个点，但是能否用x来遍历，这样可以从13 - 80 只需要描绘67个点

```c++
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	for (int x = x0; x < x1; x++)
	{
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1 - t) + y1 * t;
		image.set(x, y, color);
	}
}
```

可行，需要注意整数除法。

但是当增加两条线段时

```c++
line(13, 20, 80, 40, image, white);
line(20, 13, 40, 80, image, red);
line(80, 40, 13, 20, image, red);
```

**Question：**

1. 实际上第一条线和第三条线是同一条线，为什么第三条线没有显示出来？

2. 第二条线段是离散的点而不连续

![line01](/images/TinyRenderer/line01.png)

**Answer：**

1. 我们的函数只处理了x1 > x0的情况
2. 斜率增加，y增加的太快导致离散



```c++
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	bool steep = false;
	if ((y1 - y0) / (x1 - x0) > 1)
	{
		// 关于 x = y对称
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	for (int x = x0; x < x1; x++)
	{
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1 - t) + y1 * t;
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
	}
}
```

![final](/images/TinyRenderer/final.png)

#### 线框渲染

打开Obj文件以文本编辑器形式

```text
v -0.000581696 -0.734665 -0.623267
v 0.000283538 -1 0.286843
v -0.117277 -0.973564 0.306907
...
# 1258 vertices

vt  0.532 0.923 0.000
vt  0.535 0.917 0.000
vt  0.542 0.923 0.000
...
# 1339 texture vertices

vn  0.001 0.482 -0.876
vn  -0.001 0.661 0.751
vn  0.136 0.595 0.792
...
# 1258 vertex normals

g head
s 1
f 24/1/24 25/2/25 26/3/26
f 24/1/24 26/3/26 23/4/23
f 28/5/28 29/6/29 30/7/30
...
# 2492 faces

```

Model.h文件

```c++
class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	// 返回模型中的第i个顶点
	Vec3f vert(int i);
	//返回模型中的第idx个面 面由顶点索引构成
	std::vector<int> face(int idx);
};
```

main.cpp文件

```c++
for (int i=0; i<model->nfaces(); i++) {
    // 获取第i个面
    std::vector<int> face = model->face(i);
    // 遍历面上的点 三角形
    for (int j=0; j<3; j++) {
        // 获取两个点
        Vec3f v0 = model->vert(face[j]);
        Vec3f v1 = model->vert(face[(j+1)%3]);
        // 将坐标从[-1, 1]转换成屏幕坐标[0, width]
        int x0 = (v0.x+1.)*width/2.;
        int y0 = (v0.y+1.)*height/2.;
        int x1 = (v1.x+1.)*width/2.;
        int y1 = (v1.y+1.)*height/2.;
        line(x0, y0, x1, y1, image, white);
    }
}
```

其中视口变换将 **标准化设备坐标（NDC）** 转换为 **屏幕坐标系**。

![line02](/images/TinyRenderer/line02.png)