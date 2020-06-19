#ifndef CMPOINT_H
#define CMPOINT_H

#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

struct cmPoint {
    float color_pixel[2];
    float point3d[3];
    std::string to_string() const
    {
        char buffer[100];
        int cx = snprintf(buffer, 100, "(%.2f, %.2f, %.2f)", point3d[0], point3d[1], point3d[2]);
        return std::string(buffer);
    }
};

cmPoint get_skeleton_point_3d(rs2::depth_frame const& depthFrame, int x, int y)
{
    // Get the distance at the given pixel
    auto distance = depthFrame.get_distance(x, y);

    cmPoint point;
    point.color_pixel[0] = static_cast<float>(x);
    point.color_pixel[1] = static_cast<float>(y);

    // Deproject from pixel to point in 3D
    rs2_intrinsics intr = depthFrame.get_profile().as<rs2::video_stream_profile>().get_intrinsics(); // Calibration data
    rs2_deproject_pixel_to_point(point.point3d, &intr, point.color_pixel, distance);

    return point;
}

#endif