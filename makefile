CC=clang++
CFLAGS=	-O3 -w 
HEADERS=./Headers/CmPoint.h ./Headers/err_checkers.h ./Headers/SkeletonTracker.h

PY_PATH=C:\Users\BBT\AppData\Local\Programs\Python\Python37
IRS_PATH=C:\Program Files (x86)\Intel RealSense SDK 2.0
CM_PATH=C:\Program Files\Cubemos\SkeletonTracking

INCLUDE=-I"$(PY_PATH)\include" \
		-I"$(IRS_PATH)\include" \
		-I"$(IRS_PATH)\third-party" \
		-I"$(CM_PATH)\include" \
		-I"$(IRS_PATH)\third-party\glfw-imgui\include" \
		-I"$(IRS_PATH)\third-party\opencv-3.4\include" \
      	-I"$(IRS_PATH)\third-party\opencv-3.4\include\opencv" \
      	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\core\include" \
      	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\flann\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\imgproc\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\ml\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\objdetect\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\photo\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\video\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\dnn\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\imgcodecs\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\shape\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\videoio\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\highgui\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\superres\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\ts\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\features2d\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\calib3d\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\stitching\include" \
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\videostab\include"

LIBS=	-L"$(PY_PATH)\libs" \
		-L"$(IRS_PATH)\lib\x64" \
	 	-L"$(CM_PATH)\lib" \
		-L"$(IRS_PATH)\third-party\opencv-3.4\lib\x64\Release"

LINKERS=-lpython37 -lrealsense2 -lcubemos_base -lcubemos_engine -lcubemos_skeleton_tracking \
    	-lcubemos_intel_inference_engine_plugin -lglu32 -lopengl32 -lopencv_highgui341 -lopencv_core341 \
		-lopencv_imgproc341 -lopencv_dnn341 -lopencv_calib3d341 -lopencv_core341 -lopencv_dnn341 -lopencv_features2d341 \
		-lopencv_flann341 -lopencv_highgui341 

.SILENT: all

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $(LINKERS) $(HEADERS) main.cpp
	xcopy /y "$(IRS_PATH)\bin\x64\realsense2.dll" "."
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\bin\x64\Release\*.dll" "."
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\data\*.*" "."
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\data\*.*" "."
	.\a.exe
	del *.dll  *.caffemodel *.prototxt
	del a.exe Headers\CmPoint.h.gch Headers\err_checkers.h.gch Headers\SkeletonTracker.h.gch