#include "./Headers/IRS_Detector.h"

int main() {
    IRS_Detector irs("../Data/IRS/temp.bag", true);
    // IRS_Detector irs("-", true);
    irs.trackSkeleton();
}