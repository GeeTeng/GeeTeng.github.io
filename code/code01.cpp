#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float rotation_angle_radian = rotation_angle * MY_PI / 180;
    Eigen::Matrix4f rotation;
    rotation << cos(rotation_angle_radian), -sin(rotation_angle_radian), 0, 0,
                sin(rotation_angle_radian), cos(rotation_angle_radian), 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
    model = rotation * model;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float eye_fov_radian = eye_fov * MY_PI / 180;
    float t = tan(eye_fov_radian / 2) * abs(zNear);
    float r = t * aspect_ratio;
    float b = -t;
    float l = -r;

    Eigen::Matrix4f translation;
    translation << 1, 0, 0, -(r + l) / 2.0f,
                   0, 1, 0, -(t + b) / 2.0f,
                   0, 0, 1, -(zNear + zFar) / 2.0f,
                   0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.0f / (r - l), 0, 0, 0,
             0, 2.0f / (t - b), 0, 0,
             0, 0, 2 / (zNear - zFar), 0,
             0, 0, 0, 1;

    projection = scale * translation;
    Eigen::Matrix4f M_persp_ortho;
    M_persp_ortho << zNear, 0, 0, 0,
                     0, zNear, 0, 0,
                     0, 0, zNear + zFar, -zFar*zNear,
                     0, 0, 1, 0;
                     
    projection = projection * M_persp_ortho;
    return projection;
}

Eigen::Matrix4f getRotation(Vector3f axis, float angle){
    float rotation_angle_radian = angle * MY_PI / 180;
    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
    Eigen::Matrix3f N;
    N << 0, -axis(2), axis(1),
         axis(2), 0, -axis(0),
         -axis(1), axis(0), 0;
    Eigen::Matrix3f r3 = cos(rotation_angle_radian) * I
    + (1 - cos(rotation_angle_radian)) * (axis * axis.transpose())
    + sin(rotation_angle_radian) * N;
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    rotation.block(0, 0, 3, 3) << r3;
    return rotation;
}


int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
