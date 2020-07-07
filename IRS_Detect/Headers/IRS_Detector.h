#ifndef IRSDETECTOR_H
#define IRSDETECTOR_H

#include "../../Base/Realsense.h"
#include "IRS_Cubemos.h"

class IRS_Detector : public Realsense {

public:
    IRS_Detector(std::string filePath = "-", bool verbose = false);
    ~IRS_Detector();
    void trackSkeleton();
    void detectActivity();

private:
    // skeleton tracker object
    IRS_Cubemos *cm;
};

IRS_Detector::IRS_Detector(std::string filePath, bool verbose) : Realsense(filePath) {
    cm = new IRS_Cubemos(verbose);
}

IRS_Detector::~IRS_Detector() {
    delete cm;
}

void IRS_Detector::trackSkeleton() {
    rs2::align align_to_color(RS2_STREAM_DEPTH);
    cv::Mat capturedFrame;

    int frameCount = 0;
    std::string fpsTest = "Frame rate: ";

    // real skeleton calculation begins
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

    // in loop until escape key is hit
    while (cv::waitKey(1) != 27) {
        // capture image
        rs2::frameset data = pipe.wait_for_frames();
        data = align_to_color.process(data);

        rs2::frame colorFrame = data.get_color_frame();
        rs2::depth_frame depthFrame = data.get_depth_frame();

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
        std::vector<std::vector<Point>> skeletons;
        cm->detectFrame(skeletons, capturedFrame, depthFrame);

        if (frameCount % 25 == 0) {
            auto timePassed =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime)
                .count();
            auto fps = 25000.0 / timePassed;

            fpsTest = "Frame rate: " + std::to_string(fps) + " FPS";
            startTime = std::chrono::system_clock::now();
        }
        cv::putText(capturedFrame, fpsTest, cv::Point(50, 50), cv::FONT_HERSHEY_COMPLEX, 1, cm->skeletonColor);

        std::string cvWindowName = "Cubemos Skeleton Tracking with Intel Realsense Camera C/C++";
        cv::imshow(cvWindowName, capturedFrame);
    }
}

void IRS_Detector::detectActivity() {
    
}


#endif