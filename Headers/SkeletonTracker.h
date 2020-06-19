#pragma once
#ifndef SKELETON_TRACKER_H
#define SKELETON_TRACKER_H

// default library packages
#include <vector>
#include <iostream>
#include <string>
#include <time.h>
#include <chrono>

// python method call library
#include <Python.h>

// opencv related packages
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

// realsense related packages
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

// cubemos related packages
#include <cubemos/engine.h>
#include <cubemos/skeleton_tracking.h>

// project packages
#include "CmPoint.h"
#include "err_checkers.h"

using CUBEMOS_SKEL_Buffer_Ptr = std::unique_ptr<CM_SKEL_Buffer, void (*)(CM_SKEL_Buffer*)>;

class Skeleton_Tracker {

public:
    // public functions
    bool verbose;
    std::string log_path;
    std::string model_path;
    std::string license_path;
    std::string res_path;

    // constructor
    Skeleton_Tracker();
    Skeleton_Tracker(std::string cubemos_parent_dir, std::string file_path = "-", bool verbose = true);
    
    // destructor
    ~Skeleton_Tracker();

    // skeleton methods
    void track_skeleton();
    void activity_recognizer();


private:
    // parameters needed
    CM_SKEL_Handle* handle;
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::context ctx;
    rs2::pipeline_profile profile;
    
    // for rendering
    cv::Scalar skeletonColor;
    cv::Scalar jointColor;
    std::vector<std::pair<int, int>> limbKeypointsIds;
    cv::Point2f absentKeypoint;

    // initialization helpers
    int init_realsense(std::string filename = "-");
    int init_cubemos();

    // skeleton tracking helpers
    void render(const CM_SKEL_Buffer* skeletons_buffer, rs2::depth_frame const& depth_frame, cv::Mat& image);
    CUBEMOS_SKEL_Buffer_Ptr create_skel_buffer();
};

// CONSTRUCTOR & DESTRUCTOR

Skeleton_Tracker::Skeleton_Tracker() {
    // dummy constructor
    this->verbose = true;
    log_path = "-";
    license_path = "-";
    model_path = "-";
    res_path = "-";

    // parameters for rendering
    absentKeypoint = cv::Point2f(-1.0f, -1.0f);
    skeletonColor = cv::Scalar(100, 254, 213);
    jointColor = cv::Scalar(222, 55, 22);
    limbKeypointsIds = { { 1, 2 },   { 1, 5 },   { 2, 3 }, { 3, 4 },  { 5, 6 },
                         { 6, 7 },   { 1, 8 },   { 8, 9 }, { 9, 10 }, { 1, 11 },
                         { 11, 12 }, { 12, 13 }, { 1, 0 }, { 0, 14 }, { 14, 16 },
                         { 0, 15 },  { 15, 17 } };
}

Skeleton_Tracker::Skeleton_Tracker(std::string cubemos_parent_dir, std::string file_path, bool verbose) {
    // parameter initialization
    this->verbose = verbose;
    log_path = cubemos_parent_dir + "\\Cubemos\\SkeletonTracking\\logs";
    license_path = cubemos_parent_dir + "\\Cubemos\\SkeletonTracking\\license";
    model_path = cubemos_parent_dir + "\\Cubemos\\SkeletonTracking\\models\\fp32\\skeleton-tracking.cubemos";
    res_path = cubemos_parent_dir + "\\Cubemos\\SkeletonTracking\\res";

    // parameters for rendering
    absentKeypoint = cv::Point2f(-1.0f, -1.0f);
    skeletonColor = cv::Scalar(100, 254, 213);
    jointColor = cv::Scalar(222, 55, 22);
    limbKeypointsIds = { { 1, 2 },   { 1, 5 },   { 2, 3 }, { 3, 4 },  { 5, 6 },
                         { 6, 7 },   { 1, 8 },   { 8, 9 }, { 9, 10 }, { 1, 11 },
                         { 11, 12 }, { 12, 13 }, { 1, 0 }, { 0, 14 }, { 14, 16 },
                         { 0, 15 },  { 15, 17 } };

    
    // SDK initialization
    int err_code = 0;
    if (file_path == "-") err_code = init_realsense();
    else err_code = init_realsense(file_path);
    if (err_code == -1) {
        std::cerr << "[ERROR] Real Sense SDK cannot be initialized!" << std::endl;
        return;
    }
    err_code = init_cubemos();
    if (err_code == -1) {
        std::cerr << "[ERROR] Cubemos SDK cannot be initialized!" << std::endl;
        return;
    }
}

Skeleton_Tracker::~Skeleton_Tracker() {
    cm_skel_destroy_handle(&handle);
}

// PRIVATE INITIALIZATION HELPERS

int Skeleton_Tracker::init_realsense(std::string filename) {
    if (filename == "-" && ctx.query_devices().size() == 0) {
        EXIT_PROGRAM("No realsense device connected.");
    }

    if (filename == "-") {
        cfg.enable_stream(RS2_STREAM_COLOR, -1, 1280, 720, RS2_FORMAT_BGR8, 30);
        cfg.enable_stream(RS2_STREAM_DEPTH, -1, 1280, 720, RS2_FORMAT_ANY, 30);
    }
    else {
        cfg.enable_device_from_file(filename);
    }
    
    try {
        profile = pipe.start(cfg);
    }
    catch (std::exception ex)
    {
        EXIT_PROGRAM(std::string("Exception encountered starting the RealSense pipeline: ") + ex.what());
    }

    auto sensor = profile.get_device().first<rs2::depth_sensor>();
    auto range = sensor.get_option_range(RS2_OPTION_VISUAL_PRESET);

    for (auto i = range.min; i < range.max; i += range.step)
        if (std::string(sensor.get_option_value_description(RS2_OPTION_VISUAL_PRESET, i)) == "High Density")
            sensor.set_option(RS2_OPTION_VISUAL_PRESET, i);

    return 1;
}

int Skeleton_Tracker::init_cubemos() {
    // logging
    if(verbose) cm_initialise_logging(CM_LogLevel::CM_LL_INFO, true, log_path.c_str());

    // model creation
    CM_TargetComputeDevice enInferenceMode = CM_TargetComputeDevice::CM_CPU;
    CM_ReturnCode retCode = cm_skel_create_handle(&handle, license_path.c_str());
    CHECK_HANDLE_CREATION(retCode);

    // model load
    retCode = cm_skel_load_model(handle, enInferenceMode, model_path.c_str());
    if (retCode != CM_SUCCESS) {
        EXIT_PROGRAM("Model loading failed.");
    }

    // model window set
    std::string cvWindowName = "Cubemos Skeleton Tracking with Intel Realsense Camera C/C++";
    cv::namedWindow(cvWindowName, cv::WINDOW_NORMAL);
    cv::setWindowProperty(cvWindowName, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    return 1;
}

// PUBLIC METHODS

void Skeleton_Tracker::activity_recognizer() {
     
    std::vector<std::vector<double>> limbs = { { 1.1, 2, 3 },   { 1, 5, 6 },   { 2, 3, 4 }, { 3, 4, 5 },  { 5, 6, 7 },
                                            { 6, 7, 8 },   { 1, 8, 9 },   { 8, 9, 10 }, { 9, 10, 11 }, { 1, 11, 12 },
                                            { 11, 12, 13 }, { 12, 13, 14 }, { 1, 0, -1 }, { 0, 14, 1 }, { 14, 16, 20 },
                                            { 0, 15, 100.56 },  { 15, 17, 19 } };

    PyObject* pName = NULL, * pModule = NULL, * pDict = NULL, * pFunc = NULL, * pParam = NULL, * pResult = NULL;
    Py_Initialize();

    Py_ssize_t len = limbs.size();
    PyObject *result = PyTuple_New(len);
    for (Py_ssize_t i = 0; i < len; i++) {
        Py_ssize_t len2 = limbs[i].size();
        PyObject *item = PyTuple_New(len2);
        for (Py_ssize_t j = 0; j < len2; j++)
            PyTuple_SET_ITEM(item, j, PyFloat_FromDouble(limbs[i][j]));
        PyTuple_SET_ITEM(result, i, item);
    }

    pName = PyUnicode_FromString("channel");
    pModule = PyImport_Import(pName);
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, "connect_to_python");
    pParam = Py_BuildValue("(O)", result);
    pResult = PyObject_CallObject(pFunc, pParam);
    const char* res = PyUnicode_AsUTF8(pResult);
    printf("Return value: %s\n", res);
    Py_Finalize();
}

void Skeleton_Tracker::track_skeleton() {
    rs2::align align_to_color(RS2_STREAM_DEPTH);
    cv::Mat capturedFrame;
    const int nHeight = 192; // height of the image with which the DNN model will run inference

    CUBEMOS_SKEL_Buffer_Ptr skeletonsPresent = create_skel_buffer();
    CUBEMOS_SKEL_Buffer_Ptr skeletonsLast = create_skel_buffer();

    int frameCount = 0;
    std::string fpsTest = "Frame rate: ";

    // real skeleton calculation begins
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

    // in loop until escape key is hit
    // first 30 frames are initial empty frames and then the process starts
    while (cv::waitKey(1) != 27) {
        // capture image
        rs2::frameset data = pipe.wait_for_frames();
        data = align_to_color.process(data);

        rs2::frame colorFrame = data.get_color_frame();
        rs2::frame depthFrame = data.get_depth_frame();

        capturedFrame = cv::Mat(
            cv::Size(colorFrame.as<rs2::video_frame>().get_width(), colorFrame.as<rs2::video_frame>().get_height()),
            CV_8UC3,
            (void*)colorFrame.get_data(),
            cv::Mat::AUTO_STEP
        );

        frameCount++;

        // exit the loop if the captured frame is empty
        if (capturedFrame.empty()) {
            std::cerr << "No new frame could be captured using the input source. Exiting the loop." << std::endl;
            break;
        }

        CM_Image image = {
            capturedFrame.data,
            CM_UINT8,
            capturedFrame.cols,
            capturedFrame.rows,
            capturedFrame.channels(),
            (int)capturedFrame.step[0],
            CM_HWC
        };

        // Run Skeleton Tracking and display the results
        CM_ReturnCode retCode = cm_skel_estimate_keypoints(handle, &image, nHeight, skeletonsPresent.get());

        if (verbose)
            std::cout << "\nFrame: " << frameCount << " - Number of skeletons in the frame: " << skeletonsPresent->numSkeletons << std::endl;

        // track the skeletons in case of successful skeleton estimation
        if (retCode == CM_SUCCESS) {
            if (skeletonsPresent->numSkeletons > 0) {
                // Assign tracking ids to the skeletons in the present frame
                cm_skel_update_tracking_id(handle, skeletonsLast.get(), skeletonsPresent.get());
                // Render skeleton overlays with tracking ids
                render(skeletonsPresent.get(), depthFrame, capturedFrame);
                // Set the present frame as last one to track the next frame
                skeletonsLast.swap(skeletonsPresent);
                // Free memory of the latest frame
                cm_skel_release_buffer(skeletonsPresent.get());
            }
        }

        // FPS calculation and print on image
        if (frameCount % 25 == 0) {
            auto timePassed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime)
                .count();
            auto fps = 25000.0 / timePassed;

            fpsTest = "Frame rate: " + std::to_string(fps) + " FPS";
            startTime = std::chrono::system_clock::now();
        }
        cv::putText(capturedFrame, fpsTest, cv::Point(50, 50), cv::FONT_HERSHEY_COMPLEX, 1, skeletonColor);

        std::string cvWindowName = "Cubemos Skeleton Tracking with Intel Realsense Camera C/C++";
        cv::imshow(cvWindowName, capturedFrame);
    }
}

// PRIVATE TRACKING HELPER METHODS

void Skeleton_Tracker::render(const CM_SKEL_Buffer* skeletons_buffer, rs2::depth_frame const& depth_frame, cv::Mat& image) {
    CV_Assert(image.type() == CV_8UC3);
   
    for (int i = 0; i < skeletons_buffer->numSkeletons; i++) {
        if (verbose) std::cout << "Skeleton Number: " << i << std::endl;  
        CV_Assert(skeletons_buffer->skeletons[i].numKeyPoints == 18);
        int id = skeletons_buffer->skeletons[i].id;

        // depth is included in 2D joints
        for (size_t keypointIdx = 0; keypointIdx < skeletons_buffer->skeletons[i].numKeyPoints; keypointIdx++) {
            const cv::Point2f keyPoint(skeletons_buffer->skeletons[i].keypoints_coord_x[keypointIdx],
                skeletons_buffer->skeletons[i].keypoints_coord_y[keypointIdx]);
            if (keyPoint != absentKeypoint) {
                cv::circle(image, keyPoint, 4, jointColor, -1);

                // get the 3d point and render it on the joints
                cmPoint point3d = get_skeleton_point_3d(depth_frame, static_cast<int>(keyPoint.x), static_cast<int>(keyPoint.y));
                if (verbose) std::cout << "--- Limb: " << keypointIdx << " - Point: " << point3d.to_string() << std::endl;
                cv::putText(image, point3d.to_string(), keyPoint, cv::FONT_HERSHEY_COMPLEX, 1, jointColor);
            }
        }

        // lines are drawn between joints
        for (const auto& limbKeypointsId : limbKeypointsIds) {
            const cv::Point2f keyPointFirst(skeletons_buffer->skeletons[i].keypoints_coord_x[limbKeypointsId.first],
                skeletons_buffer->skeletons[i].keypoints_coord_y[limbKeypointsId.first]);

            const cv::Point2f keyPointSecond(skeletons_buffer->skeletons[i].keypoints_coord_x[limbKeypointsId.second],
                skeletons_buffer->skeletons[i].keypoints_coord_y[limbKeypointsId.second]);

            if (keyPointFirst == absentKeypoint || keyPointSecond == absentKeypoint) {
                continue;
            }

            cv::line(image, keyPointFirst, keyPointSecond, skeletonColor, 2, cv::LINE_AA);
        }
        
        // coordinates of the joints are print
        for (size_t keypointIdx = 0; keypointIdx < skeletons_buffer->skeletons[i].numKeyPoints; keypointIdx++) {
            const cv::Point2f keyPoint(skeletons_buffer->skeletons[i].keypoints_coord_x[keypointIdx],
                skeletons_buffer->skeletons[i].keypoints_coord_y[keypointIdx]);
            if (keyPoint != absentKeypoint) {
                // found a valid keypoint and displaying the skeleton tracking id next to it
                cv::putText(image,
                    (std::to_string(id)),
                    cv::Point2f(keyPoint.x, keyPoint.y - 20),
                    cv::FONT_HERSHEY_COMPLEX,
                    1,
                    skeletonColor);
                break;
            }
        }
    }
}

CUBEMOS_SKEL_Buffer_Ptr Skeleton_Tracker::create_skel_buffer() {
    return CUBEMOS_SKEL_Buffer_Ptr(new CM_SKEL_Buffer(), [](CM_SKEL_Buffer* pb) {
        cm_skel_release_buffer(pb);
        delete pb;
        });
}

#endif
