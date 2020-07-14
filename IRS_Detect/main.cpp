#include "./Headers/IRS_Detector.h"

int main() {
    IRS_Detector irs("../Data/IRS/walking.bag", false);
    // IRS_Detector irs("-", false);
    // irs.trackSkeleton();
    irs.detectActivity();
}