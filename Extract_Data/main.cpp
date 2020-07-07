#include "./Headers/DataBuilder.h"

int main() {
    DataBuilder db;
    // db.processSingleVideoKARD("../Data/KARD/a1/a01_s01_e01");
    db.processSingleImageKinect("../Data/Kinect_Img/RGB/s01_e01/colorImg190");
    return 0;
}