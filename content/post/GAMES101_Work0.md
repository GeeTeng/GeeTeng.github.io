---
title: "GAMES101_Work0" 

date: 2024-11-12 

draft: false
---

**作业描述** 

给定一个点P=(2,1),将该点绕原点先逆时针旋转45◦，再平移(1,2),计算出 变换后点的坐标。（要求用齐次坐标进行计算）

**编译** 

为方便之后的作业编写，本次作业要求使用cmake进行编译。 

首先，编写好本次作业的程序main.cpp。 然后, 在 main.cpp 所在目录下，打开终端(命令行)，依次输入：

 • **mkdir build:** 创建名为 build 的文件夹。

 **• cd build:** 移动到 build 文件夹下。

 **• cmake ..:** 注意其中’..’ 表示上一级目录，若为’.’ 则表示当前目录。

 • **make:** 编译程序，错误提示会显示在终端中。 

**• ./Transformation：**若上一步无错误，则可运行程序(这里的Transformation 为可执行文件名，可参照CMakeLists.txt 中修改)。

```c++
#include<cmath>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<iostream>

int main(){
    // init point
    Eigen::Vector3f p(2.0f, 1.0f, 1.0f);

    // rotate and transformation matrix
    // x' = xcos - ysin
    // y' = xsin + ycos
    // x' = x + tx
    // y' = y + ty
    Eigen::Matrix3f R, T;
    // cos45 = sin45 = sqrt(2) / 2
    float ftheta = sqrt(2.0f) / 2;
    R << ftheta, -ftheta, 0,
         ftheta, ftheta, 0,
         0, 0,1.0f;

    T << 1.0f, 0, 1.0f,
         0, 1.0f, 2.0f,
         0, 0, 1.0f;

    p = R * p;
    std::cout << "After Rotate \n";
    std::cout << p << std::endl;

    p = T * p;
    std::cout << "After Transformation \n";
    std::cout << p << std::endl;

    return 0;
}
```

**课后作业附件** 

[pa0.pdf](/code/pa0.pdf) 

 [CMakeLists.txt](/code/CMakeLists.txt) 