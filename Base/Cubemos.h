#ifndef CUBEMOS_H
#define CUBEMOS_H

#include <iostream>
#include <string>
#include <vector>
// opencv related packages
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>
// cubemos related packages
#include <cubemos/engine.h>
#include <cubemos/skeleton_tracking.h>

using CUBEMOS_SKEL_Buffer_Ptr = std::unique_ptr<CM_SKEL_Buffer, void (*)(CM_SKEL_Buffer*)>;

struct Point {
    float x;
    float y;
    float z;
    Point() {
        x = -1;
        y = -1;
        z = -1;
    }
    Point(float x_coord, float y_coord, float z_coord) {
        x = x_coord;
        y = y_coord;
        z = z_coord;
    }
    std::string to_string() const
    {
        char buffer[120];
        int cx = snprintf(buffer, 100, "(%.2f, %.2f, %.2f)", x, y, z);
        return std::string(buffer);
    }
};

class Cubemos {

public:
    Cubemos(bool verbose = false);
    ~Cubemos();
    
    // for enabling/disabling comments
    bool verbose;
    // for id tracking
    CUBEMOS_SKEL_Buffer_Ptr skeletonsPresent = createSkelBuffer();
    CUBEMOS_SKEL_Buffer_Ptr skeletonsLast = createSkelBuffer();
    // for rendering
    cv::Scalar skeletonColor;

protected:
    template <class T>
    void render(std::vector<std::vector<Point>> & skeletons, const CM_SKEL_Buffer* skeletonsBuffer,
                cv::Mat& image);
    
    CUBEMOS_SKEL_Buffer_Ptr createSkelBuffer();
    
    // for licensing
    std::string logPath;
    std::string modelPath;
    std::string licensePath;
    std::string resPath;
    // for rendering
    CM_SKEL_Handle* handle;
    cv::Scalar jointColor;
    std::vector<std::pair<int, int>> limbKeypointsIds;
    cv::Point2f absentKeypoint;

};

Cubemos::Cubemos(bool verbose) {
    this->verbose = verbose;
    std::string appDataDir = getenv("USERPROFILE");
    logPath = appDataDir + "\\AppData\\Local\\Cubemos\\SkeletonTracking\\logs";
    licensePath = appDataDir + "\\AppData\\Local\\Cubemos\\SkeletonTracking\\license";
    modelPath = appDataDir + "\\AppData\\Local\\Cubemos\\SkeletonTracking\\models\\fp32\\skeleton-tracking.cubemos";
    resPath = appDataDir + "\\AppData\\Local\\Cubemos\\SkeletonTracking\\res";

    // parameters for rendering
    absentKeypoint = cv::Point2f(-1.0f, -1.0f);
    skeletonColor = cv::Scalar(100, 254, 213);
    jointColor = cv::Scalar(222, 55, 22);
    limbKeypointsIds = { { 1, 2 },   { 1, 5 },   { 2, 3 }, { 3, 4 },  { 5, 6 },
                         { 6, 7 },   { 1, 8 },   { 8, 9 }, { 9, 10 }, { 1, 11 },
                         { 11, 12 }, { 12, 13 }, { 1, 0 }, { 0, 14 }, { 14, 16 },
                         { 0, 15 },  { 15, 17 } };

    if(verbose) cm_initialise_logging(CM_LogLevel::CM_LL_INFO, true, logPath.c_str());
    // model creation
    CM_TargetComputeDevice enInferenceMode = CM_TargetComputeDevice::CM_CPU;
    CM_ReturnCode retCode = cm_skel_create_handle(&handle, licensePath.c_str());
    if(retCode != CM_SUCCESS) {
        std::cerr << "[ERROR][CM] Creation handling failed!" << std::endl;
        return;
    }
    // model load
    retCode = cm_skel_load_model(handle, enInferenceMode, modelPath.c_str());
    if (retCode != CM_SUCCESS) {
        std::cerr << "[ERROR][CM] Cubemos model loading failed!" << std::endl;
    }
}

Cubemos::~Cubemos() {
    cm_skel_destroy_handle(&handle);
}

template <class T>
void Cubemos::render(std::vector<std::vector<Point>> & skeletons, const CM_SKEL_Buffer* skeletonsBuffer, cv::Mat& image) {
    CV_Assert(image.type() == CV_8UC3);
   
    for (int i = 0; i < skeletonsBuffer->numSkeletons; i++) { 
        CV_Assert(skeletonsBuffer->skeletons[i].numKeyPoints == 18);
        int id = skeletonsBuffer->skeletons[i].id;

        std::vector<Point> oneSkeleton;
        // depth is included in 2D joints
        for (size_t keypointIdx = 0; keypointIdx < skeletonsBuffer->skeletons[i].numKeyPoints; keypointIdx++) {
            const cv::Point2f keyPoint(skeletonsBuffer->skeletons[i].keypoints_coord_x[keypointIdx],
                skeletonsBuffer->skeletons[i].keypoints_coord_y[keypointIdx]);
            if (keyPoint != absentKeypoint) {
                cv::circle(image, keyPoint, 4, jointColor, -1);
                // get the 3d point and render it on the joints
                Point point2d(keyPoint.x, keyPoint.y, -1);
                oneSkeleton.push_back(point2d);
            } else {
                oneSkeleton.push_back(Point(-1, -1, -1));
            }
        }
        skeletons.push_back(oneSkeleton);

        // lines are drawn between joints
        for (const auto& limbKeypointsId : limbKeypointsIds) {
            const cv::Point2f keyPointFirst(skeletonsBuffer->skeletons[i].keypoints_coord_x[limbKeypointsId.first],
                skeletonsBuffer->skeletons[i].keypoints_coord_y[limbKeypointsId.first]);

            const cv::Point2f keyPointSecond(skeletonsBuffer->skeletons[i].keypoints_coord_x[limbKeypointsId.second],
                skeletonsBuffer->skeletons[i].keypoints_coord_y[limbKeypointsId.second]);

            if (keyPointFirst == absentKeypoint || keyPointSecond == absentKeypoint) {
                continue;
            }

            cv::line(image, keyPointFirst, keyPointSecond, skeletonColor, 2, cv::LINE_AA);
        }
    }
}

CUBEMOS_SKEL_Buffer_Ptr Cubemos::createSkelBuffer() {
    return CUBEMOS_SKEL_Buffer_Ptr(new CM_SKEL_Buffer(), [](CM_SKEL_Buffer* pb) {
        cm_skel_release_buffer(pb);
        delete pb;
    });
}

#endif