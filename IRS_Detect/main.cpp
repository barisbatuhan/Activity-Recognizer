#include "./Headers/IRS_Detector.h"

int main() {
    
    std::vector<std::string> files = {"../Data/IRS/claphands.bag",
                                      "../Data/IRS/walking.bag",
                                      "../Data/IRS/sitdown.bag",
                                      "../Data/IRS/standup.bag"};
    
    IRS_Detector irs(files[0], false); 
    // IRS_Detector irs("-", false); // for real-time detection
    // irs.trackSkeleton();
    irs.detectActivity();
    return 0;
}