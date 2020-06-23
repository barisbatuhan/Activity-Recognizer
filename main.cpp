#include "./Headers/SkeletonTracker.h"

int main() {
    
    enum running_env {VS2019, CMD};
    enum process {SKEL_TRACK_FILE, SKEL_TRACK_RT, ACTIVITY_EST_RT, ACTIVITY_EST_FILE};

    // please set the environment and process correct
    running_env curr_env = CMD;
    process curr_proc = SKEL_TRACK_FILE;
    std::string cubemos_license_path = "C:\\Users\\BBT\\AppData\\Local";
    std::string filename = "wavehands.bag"; // please set the filename correctly if a process from the file will be made

    if(curr_proc == SKEL_TRACK_RT) {
        Skeleton_Tracker st(cubemos_license_path);
        st.track_skeleton();
    }
    else if(curr_proc == SKEL_TRACK_FILE) {
        if(curr_env == VS2019) {
            Skeleton_Tracker st(cubemos_license_path, filename);
            st.track_skeleton();
        }
        else if(curr_env == CMD) {
            filename = "./Data/" + filename;
            Skeleton_Tracker st(cubemos_license_path, filename);
            st.track_skeleton();
        }
    }
    else if(curr_proc == ACTIVITY_EST_RT) {
        if(curr_env == VS2019) {
            printf("[ERROR] This process is not implemented yet...\n");
        }
        else if(curr_env == CMD) {
            Skeleton_Tracker st(cubemos_license_path);
            st.activity_recognizer();
        }   
    }
    else if(curr_proc == ACTIVITY_EST_FILE) {  
        if(curr_env == VS2019) {
            printf("[ERROR] This process is not implemented yet...\n");
        }
        else if(curr_env == CMD) {
            filename = "./Data/" + filename;
            Skeleton_Tracker st(cubemos_license_path, filename);
            st.activity_recognizer();
        }
    }
    return 0;
}