#ifndef REALSENSE_H
#define REALSENSE_H

#include <iostream>
#include <string>
// opencv related packages
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
// realsense related packages
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

class Realsense {

public:
    Realsense(std::string filePath = "-");

protected:
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::context ctx;
    rs2::pipeline_profile profile;

};

Realsense::Realsense(std::string filePath) {
    if (filePath == "-" && ctx.query_devices().size() == 0) {
        std::cerr << "[ERROR][IRS] No realsense device connected." << std::endl;
        return;
    }

    if (filePath == "-") {
        cfg.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, RS2_FORMAT_BGR8, 30);
        cfg.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, RS2_FORMAT_ANY, 30);
    }
    else cfg.enable_device_from_file(filePath);

    try {
        profile = pipe.start(cfg);
    } catch (std::exception ex) {
        std::cerr << "[ERROR][IRS] Exception encountered starting the RealSense pipeline: " << ex.what() << std::endl;
        return;
    }

    auto sensor = profile.get_device().first<rs2::depth_sensor>();
    auto range = sensor.get_option_range(RS2_OPTION_VISUAL_PRESET);

    for (auto i = range.min; i < range.max; i += range.step)
        if (std::string(sensor.get_option_value_description(RS2_OPTION_VISUAL_PRESET, i)) == "High Density")
            sensor.set_option(RS2_OPTION_VISUAL_PRESET, i);
}

#endif