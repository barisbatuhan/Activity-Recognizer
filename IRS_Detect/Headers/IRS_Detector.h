#ifndef IRSDETECTOR_H
#define IRSDETECTOR_H

// python method call library
#include <Python.h>
// base class libraries
#include "../../Base/Realsense.h"
// cubemos support class
#include "IRS_Cubemos.h"

class IRS_Detector : public Realsense
{

public:
    IRS_Detector(std::string filePath = "-", bool verbose = false);
    ~IRS_Detector();
    void trackSkeleton();
    void detectActivity();

private:
    // skeleton tracker object
    IRS_Cubemos *cm;
};

IRS_Detector::IRS_Detector(std::string filePath, bool verbose) : Realsense(filePath)
{
    cm = new IRS_Cubemos(verbose);
}

IRS_Detector::~IRS_Detector()
{
    delete cm;
}

void IRS_Detector::trackSkeleton()
{
    rs2::align align_to_color(RS2_STREAM_DEPTH);
    cv::Mat capturedFrame;

    int frameCount = 0;
    std::string fpsTest = "Frame rate: ";

    // real skeleton calculation begins
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

    // in loop until escape key is hit
    while (cv::waitKey(1) != 27)
    {
        // capture image
        rs2::frameset data = pipe.wait_for_frames();
        data = align_to_color.process(data);

        rs2::frame colorFrame = data.get_color_frame();
        rs2::depth_frame depthFrame = data.get_depth_frame();

        capturedFrame = cv::Mat(
            cv::Size(colorFrame.as<rs2::video_frame>().get_width(), colorFrame.as<rs2::video_frame>().get_height()),
            CV_8UC3,
            (void *)colorFrame.get_data(),
            cv::Mat::AUTO_STEP);
        frameCount++;
        // exit the loop if the captured frame is empty
        if (capturedFrame.empty())
        {
            std::cerr << "No new frame could be captured using the input source. Exiting the loop." << std::endl;
            break;
        }
        std::vector<std::vector<Point>> skeletons;
        cm->detectFrame(skeletons, capturedFrame, depthFrame);

        if (frameCount % 25 == 0)
        {
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

void IRS_Detector::detectActivity()
{
    int skelHistory = 10;
    Py_Initialize();
    if (!Py_IsInitialized()) {
        std::cerr<< "[ERROR][DETECTOR] Python initalization failed.\n";
        return;
    }
    try
    {
        // module loader
        PyObject *pName = PyUnicode_FromString("channel");
        PyObject *pModule = PyImport_Import(pName);
        Py_DECREF(pName);
        // functions inside the module are declared
        PyObject *pConstructor = PyObject_GetAttrString(pModule, "load_model");
        PyObject *pAdder = PyObject_GetAttrString(pModule, "add_skel_data");
        PyObject *pPredictor = PyObject_GetAttrString(pModule, "predict");
        // model is loaded
        PyObject *pModel = PyObject_CallObject(pConstructor, NULL);

        rs2::align align_to_color(RS2_STREAM_DEPTH);
        cv::Mat capturedFrame;
        int frameCount = 0;

        while (cv::waitKey(1) != 27 && frameCount < 70)
        {
            // capture image
            rs2::frameset data = pipe.wait_for_frames();
            data = align_to_color.process(data);

            rs2::frame colorFrame = data.get_color_frame();
            rs2::depth_frame depthFrame = data.get_depth_frame();

            capturedFrame = cv::Mat(
                cv::Size(colorFrame.as<rs2::video_frame>().get_width(), colorFrame.as<rs2::video_frame>().get_height()),
                CV_8UC3,
                (void *)colorFrame.get_data(),
                cv::Mat::AUTO_STEP);
            frameCount++;
            // exit the loop if the captured frame is empty
            if (capturedFrame.empty())
            {
                std::cerr << "No new frame could be captured using the input source. Exiting the loop." << std::endl;
                break;
            }
            std::vector<std::vector<Point>> skeletons;
            std::cout << "Frame number: " << frameCount << " & ";
            cm->detectFrame(skeletons, capturedFrame, depthFrame);

            Py_ssize_t dim1 = 18;
            Py_ssize_t dim2 = 3;
            PyObject *pSkel = PyTuple_New(dim1);
            for (Py_ssize_t i = 0; i < dim1; i++)
            {
                PyObject *pJoint = PyTuple_New(dim2);
                PyTuple_SET_ITEM(pJoint, 0, PyFloat_FromDouble(skeletons[0][i].x));
                PyTuple_SET_ITEM(pJoint, 1, PyFloat_FromDouble(skeletons[0][i].y));
                PyTuple_SET_ITEM(pJoint, 2, PyFloat_FromDouble(skeletons[0][i].z));
                PyTuple_SET_ITEM(pSkel, i, pJoint);
            }
            PyObject *pAddResult = PyObject_CallFunctionObjArgs(pAdder, pModel, pSkel, NULL);
            if(pAddResult == NULL) {
                std::cerr << "[ERROR][DETECTOR] Skeleton data cannot be added to Python!" << std::endl;
                Py_Finalize();
                return;
            }
            if(frameCount > 40) {
                PyObject *pEvalResult = PyObject_CallFunctionObjArgs(pPredictor, pModel, NULL);
                if(pEvalResult == NULL) {
                    std::cerr << "[ERROR][DETECTOR] Problem occured while taking action result from model!" << std::endl;
                    Py_Finalize();
                    return;
                }
            }
            std::string cvWindowName = "Cubemos Skeleton Tracking with Intel Realsense Camera C/C++";
            cv::imshow(cvWindowName, capturedFrame);
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[ERROR][DETECTOR] " << ex.what() << std::endl;
    }
    catch (const std::string &ex)
    {
        std::cerr << "[ERROR][DETECTOR] " << ex << std::endl;
    }
    catch (...)
    {
        std::cerr << "[ERROR][DETECTOR] An error occurred." << std::endl;
    }
}

#endif