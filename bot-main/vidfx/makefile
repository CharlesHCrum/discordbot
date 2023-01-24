# Add the names of all the opencv libs that need to be linked. It's shorter
# to exclude the prefix here and add it later
OPENCV_LIBS=core imgcodecs imgproc

# Setup link parameters for all opencv libs
OPENCV_LIBS:=$(addprefix -lopencv_,$(OPENCV_LIBS))

all: vidfx imgfx

vidfx: image.cpp imagepipe.cpp
	g++ image.cpp imagepipe.cpp -o vidfx -O3 -I/mingw64/include/opencv4 $(OPENCV_LIBS)

imgfx: image.cpp
	g++ image.cpp -o imgfx -DIMG_FX -O3 -I/mingw64/include/opencv4 $(OPENCV_LIBS)

