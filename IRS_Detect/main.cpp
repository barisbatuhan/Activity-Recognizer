#include "./Headers/IRS_Detector.h"

int main() {
    IRS_Detector irs("../Data/IRS/wavehands.bag", false);
    // IRS_Detector irs("-", true);
    // irs.trackSkeleton();
    irs.detectActivity();
}