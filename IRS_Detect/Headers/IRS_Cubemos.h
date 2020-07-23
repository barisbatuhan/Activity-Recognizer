// base class header
#include "../../Base/Cubemos.h"
// realsense related packages
#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>

class IRS_Cubemos : public Cubemos {

public:
    IRS_Cubemos(bool verbose = false);
    int detectFrame(std::vector<std::vector<Point>> & skeletons, cv::Mat &frame, rs2::depth_frame &depthFrame);

private:
    void finalizePoints(std::vector<std::vector<Point>> & skeletons, rs2::depth_frame &depthFrame, int height, int width);
};

IRS_Cubemos::IRS_Cubemos(bool verbose) : Cubemos(verbose) {}

int IRS_Cubemos::detectFrame(std::vector<std::vector<Point>> & skeletons, cv::Mat &frame, rs2::depth_frame &depthFrame) {
    CM_Image image = {
        frame.data,
        CM_UINT8,
        frame.cols,
        frame.rows,
        frame.channels(),
        (int)frame.step[0],
        CM_HWC
    };
    int skelCnt = -1;
    // Run Skeleton Tracking and display the results
    CM_ReturnCode retCode = cm_skel_estimate_keypoints(handle, &image, 192, skeletonsPresent.get());
    // track the skeletons in case of successful skeleton estimation
    if (retCode == CM_SUCCESS) {
        skelCnt = skeletonsPresent->numSkeletons;
        if (skeletonsPresent->numSkeletons > 0) {
            // Assign tracking ids to the skeletons in the present frame
            cm_skel_update_tracking_id(handle, skeletonsLast.get(), skeletonsPresent.get());
            // Render skeleton overlays with tracking ids
            render<rs2::depth_frame>(skeletons, skeletonsPresent.get(), frame);
            finalizePoints(skeletons, depthFrame, frame.rows, frame.cols);
            // Set the present frame as last one to track the next frame
            skeletonsLast.swap(skeletonsPresent);
            // Free memory of the latest frame
            cm_skel_release_buffer(skeletonsPresent.get());
        }
    }
    return skelCnt;
}

void IRS_Cubemos::finalizePoints(std::vector<std::vector<Point>> & skeletons, rs2::depth_frame &depthFrame, int height, int width) {
    int skeletonNum = 1;
    for(auto &skeleton: skeletons) {
        skeletonNum++;
        int jointNum = 0;
        auto baseJoint = skeleton[1]; // chest point is taken for normalization
        if(baseJoint.x < 0) continue;
        float baseDistance = depthFrame.get_distance(baseJoint.x, baseJoint.y);
        for(auto &joint : skeleton) {     
            if(joint.x > 0) {
                float distance = depthFrame.get_distance(joint.x, joint.y);
                joint.x = (joint.x - baseJoint.x) / (width / 10);
                joint.y = (joint.y - baseJoint.y) / (height / 10);
                joint.z = (distance - baseDistance);
                if(verbose) std::cout << "--- Joint: " << jointNum << ": " << joint.to_string() << std::endl;
            }
            jointNum++;
        }
    }
}