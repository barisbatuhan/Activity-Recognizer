CC=clang++
CFLAGS=	-w
HEADERS=../Base/Cubemos.h ./Headers/CubemosStandalone.h ./Headers/DataBuilder.h

IRS_PATH=C:\Program Files (x86)\Intel RealSense SDK 2.0
CM_PATH=C:\Program Files\Cubemos\SkeletonTracking

INCLUDE=-I"$(CM_PATH)\include" \
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
	  	-I"$(IRS_PATH)\third-party\opencv-3.4\modules\videostab\include" \
		-I"$(IRS_PATH)\third-party\opencv-3.4\modules\world\include"


LIBS=	-L"$(CM_PATH)\lib" \
		-L"$(IRS_PATH)\third-party\opencv-3.4\lib\x64\Release"

LINKERS=-lcubemos_base -lcubemos_engine -lcubemos_skeleton_tracking \
    	-lcubemos_intel_inference_engine_plugin -lglu32 -lopengl32 \
		-lopencv_calib3d341     -lopencv_imgcodecs341  -lopencv_shape341 \
		-lopencv_core341        -lopencv_imgproc341    -lopencv_stitching341 \
		-lopencv_dnn341         -lopencv_ml341         -lopencv_superres341 \
		-lopencv_features2d341  -lopencv_objdetect341  -lopencv_video341 \
		-lopencv_flann341       -lopencv_photo341      -lopencv_videoio341 \
		-lopencv_highgui341     -lopencv_python2       -lopencv_videostab341

.SILENT: all

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $(LINKERS) $(HEADERS) main.cpp
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\bin\x64\Release\*.dll" "."
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\data\*.*" "."
	xcopy /y "$(IRS_PATH)\third-party\opencv-3.4\data\*.*" "."
	.\a.exe
	del *.dll  *.caffemodel *.prototxt
	del a.exe .\Headers\*.h.gch ..\Base\*.h.gch