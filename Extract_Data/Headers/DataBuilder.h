#ifndef DATABUILDER_H
#define DATABUILDER_H

#include "CubemosStandalone.h"

class DataBuilder {

public:
    DataBuilder();
    ~DataBuilder();

    // for all functions below, the type extensions (e.g. .mp4, .jpg, .txt) will be excluded
    void processVideosKARD(std::vector<std::string> fileList); // gets path list of files
    void processSingleVideoKARD(std::string path); // gets a path for one file

    void processImagesKinect(std::vector<std::string> filesList); // gets path list of files
    void processSingleImageKinect(std::string path); // gets a path for one file

private:
    void getDepthMapKARD(std::vector<float> &depthFrame, std::string filePath);
    void getDepthMapKinect(std::vector<float> &depthFrame, int &width, int &height, std::string filePath);

    CubemosStandalone *cm;

};

// CONSTRUCTORS

DataBuilder::DataBuilder() {
    cm = new CubemosStandalone();
}

DataBuilder::~DataBuilder() {
    delete cm;
}

// KARD RELATED FUNCTIONS

void DataBuilder::processVideosKARD(std::vector<std::string> fileList) {
    for(std::string &filePath : fileList) {
        processSingleVideoKARD(filePath);
    }
}

void DataBuilder::processSingleVideoKARD(std::string path) {
    std::string videoPath = path + ".mp4";
    std::string depthPath = path + "_depthmaps.txt";
    
    cv::VideoCapture cap(videoPath);
    if(!cap.isOpened()){
	    std::cout << "[ERROR][DATABUILDER] Error opening video stream!" << std::endl;
	    return;
	} 

    std::vector<float> depthFrame;
    getDepthMapKARD(depthFrame, depthPath);

    int frameCount = 0;
    while(true){
	    cv::Mat frame;
	    cap >> frame;
        if(frame.empty()) break;
        std::vector<std::vector<Point>> skeletons;
        cm->detectFrame(skeletons, frame, depthFrame);
        frameCount++;
        std::string cvWindowName = "Cubemos Skeleton Tracking from KARD Data C/C++";
        cv::imshow(cvWindowName, frame);
    }
}

void DataBuilder::getDepthMapKARD(std::vector<float> &depthFrame, std::string filePath) {
    std::ifstream input(filePath.c_str());
    std::string line;
    while(std::getline(input, line)) {
        std::istringstream subin(line);
        int d;
        while(subin >> d){
            depthFrame.push_back((float) d);
        }
    }
}

// KINECT RELATED FUNCTIONS

void DataBuilder::processImagesKinect(std::vector<std::string> filesList) {
    for(std::string &filePath : filesList) {
        processSingleImageKinect(filePath);
    }
}

void DataBuilder::processSingleImageKinect(std::string path) {
    int idx1 = path.find("RGB");
    std::string imgPath = path + ".jpg";
    std::string depthPath = "";
    if(idx1 < 0) {
        std::cerr << "[ERROR][DATABUILDER] Wrong path is given for Kinect image!" << std::endl;
        return;
    }
    std::string commonPath = path.substr(0, idx1);
    depthPath += commonPath + "depth/";
    int idx2 =  path.rfind("/");
    depthPath += path.substr(idx1 + 4, idx2 - idx1 - 3) + "depthImg" + path.substr(path.length() - 3, 3) + ".xml";

    std::cout << imgPath << " & " << depthPath << std::endl;

    cv::Mat frame = cv::imread(imgPath);
    std::vector<float> depthFrame;
    int width, height;
    getDepthMapKinect(depthFrame, width, height, depthPath);
    cv::resize(frame, frame, cv::Size(), 0.5, 0.5);

    std::vector<std::vector<Point>> skeletons;
    cm->detectFrame(skeletons, frame, depthFrame, true);
    std::string cvWindowName = "Cubemos Skeleton Tracking from KARD Data C/C++";
    while(cv::waitKey(1) != 27) cv::imshow(cvWindowName, frame);
}

void DataBuilder::getDepthMapKinect(std::vector<float> &depthFrame, int &width, int &height, std::string filePath) {
    std::ifstream input(filePath.c_str());
    std::string line;
    bool dataRead = false;
    while(std::getline(input, line)) {
        int idx1 = line.find("<width>");
        int idx2 = line.find("<height>");
        int idx3 = line.find("<data>");
        if(!dataRead && idx1 > 0) {
            int idx1_1 = line.find("</width>");
            std::string w = line.substr(idx1 + 7, idx1_1 - idx1 - 7);
            width = std::stoi(w);
            // std::cout << "Width: " << width << std::endl;
        }
        else if(!dataRead && idx2 > 0) {
            int idx2_1 = line.find("</height>");
            std::string h = line.substr(idx2 + 8, idx2_1 - idx2 - 8);
            height = std::stoi(h);
            // std::cout << "Height: " << height << std::endl;
        }
        else if(!dataRead && idx3 > 0) {
            dataRead = true;
            continue;
        }
        if(dataRead) {
            std::istringstream subin(line);
            int d;
            float dist;
            while(subin >> d){
                if (d == 0x0000) dist = 0.0;
                else if (d >= 0x8000) dist = 4.0;
                else dist = 0.8 + (float(d - 6576) * 0.00012115165336374002280501710376283);
                depthFrame.push_back(dist);
                // std::cout << "Data Depth: " << dist << std::endl;
            }
        }
    }
}

#endif