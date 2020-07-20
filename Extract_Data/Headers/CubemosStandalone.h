#ifndef CUBEMOS_STANDALONE_H
#define CUBEMOS_STANDALONE_H

#include "../../Base/Cubemos.h"

class CubemosStandalone : public Cubemos {

public:
    CubemosStandalone(bool verbose = false);
    int detectFrame(std::vector<std::vector<Point>> & skeletons, cv::Mat &frame, std::vector<float> &depthFrame, bool ifKinect = false);

private:
    int finalizePoints(std::vector<std::vector<Point>> & skeletons, std::vector<float> &depthFrame, int height, int width, bool ifKinect);
};

CubemosStandalone::CubemosStandalone(bool verbose) : Cubemos(verbose) {}

int CubemosStandalone::detectFrame(std::vector<std::vector<Point>> & skeletons, cv::Mat &frame, std::vector<float> &depthFrame, bool ifKinect) {
    CM_Image image = {
        frame.data,
        CM_UINT8,
        frame.cols,
        frame.rows,
        frame.channels(),
        (int)frame.step[0],
        CM_HWC
    };
    // Run Skeleton Tracking and display the results
    CM_ReturnCode retCode = cm_skel_estimate_keypoints(handle, &image, 192, skeletonsPresent.get());
    // track the skeletons in case of successful skeleton estimation
    if (retCode == CM_SUCCESS) {
        if (skeletonsPresent->numSkeletons > 0) {
            // Assign tracking ids to the skeletons in the present frame
            cm_skel_update_tracking_id(handle, skeletonsLast.get(), skeletonsPresent.get());
            // Render skeleton overlays with tracking ids
            render<std::vector<int>>(skeletons, skeletonsPresent.get(), frame);
            std::cout << "Skeleton Count: " << skeletonsPresent->numSkeletons << std::endl;
            int idxWithSkeleton = finalizePoints(skeletons, depthFrame, frame.rows, frame.cols, ifKinect);
            // Set the present frame as last one to track the next frame
            skeletonsLast.swap(skeletonsPresent);
            // Free memory of the latest frame
            cm_skel_release_buffer(skeletonsPresent.get());
            return idxWithSkeleton;
        }
    }
    return -1;
}

int CubemosStandalone::finalizePoints(std::vector<std::vector<Point>> & skeletons, std::vector<float> &depthFrame, int height, int width, bool ifKinect) {
    int skeletonNum = 1;
    int idxWithSkel = -1;
    int iterNo = 0;
    int skelMaxJoint = -1;
    for(auto &skeleton: skeletons) {
        skeletonNum++;
        int jointNum = 0;
        auto baseJoint = skeleton[1];
        int skelJointCnt = 0;
        if(baseJoint.x < 0) continue;
        float baseDistance = depthFrame[width * baseJoint.y + baseJoint.x];
        for(auto &joint : skeleton) {
            if(joint.x > 0) {
                skelJointCnt++;
                float distance = depthFrame[width * joint.y + joint.x];
                // joint.x = (joint.x - baseJoint.x) / width;
                // joint.y = (joint.y - baseJoint.y) / height;
                // if(ifKinect) joint.z = (distance - baseDistance);
                // else joint.z = (distance - baseDistance) / 1000;
                joint.x = (joint.x) / width;
                joint.y = (joint.y) / height;
                if(ifKinect) joint.z = (distance / 3);
                else joint.z = (distance) / 3000;
                if(true) std::cout << "--- Joint: " << jointNum << ": " << joint.to_string() << std::endl;
            }
            jointNum++;
        }
        if(skelJointCnt > skelMaxJoint) {
            idxWithSkel = iterNo;
            skelMaxJoint = skelJointCnt;
        }
        iterNo++;
    }
    return idxWithSkel;
}

#endif