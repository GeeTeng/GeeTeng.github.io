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