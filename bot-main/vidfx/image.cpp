#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <random>
#include <iostream>

using namespace cv;

class RandomVars {
public:
	float i;
	float j;
	float k;
	float u;
	float v;
	float w;
	
	int x;
	int y;
	
	RandomVars() {
		std::random_device dev;
		std::mt19937 rng(dev());
		
		std::uniform_int_distribution<std::mt19937::result_type> dist1(0,1);
		x = dist1(rng);
		y = dist1(rng);
		
		std::uniform_int_distribution<std::mt19937::result_type> dist2(1,7);
		i = dist2(rng);
		j = dist2(rng);
		k = dist2(rng);
	}
} rng;

Mat sharp_image(Mat img) {
	Mat gaussian;
	cv::GaussianBlur(img, gaussian, Size(0, 0), 2.0);
	Mat sharpened;
	cv::addWeighted(img, 7.0, gaussian, -6.0, 0.0, sharpened);
	return sharpened;
}

Mat sharp_image2(Mat img) {
	Mat gaussian;
	cv::GaussianBlur(img, gaussian, Size(0, 0), 2.0);
	Mat sharpened;
	cv::addWeighted(img, 7.0, gaussian, -6.0, 0.0, sharpened);
	return sharpened;
}

Mat adjust_saturation(Mat img) {
	std::vector<Mat> hsv;
	cv::split(img, hsv);
	
	
	hsv[0] /= rng.i;
	hsv[1] *= rng.j;
	hsv[2] *= rng.k;
	
	
	Mat resimage;
	cv::merge(hsv, resimage);
	
	return resimage;
}

Mat adjust_saturation2(Mat img) {
	std::vector<Mat> hsv;
	cv::split(img, hsv);
	
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist3(1,7);
	float r = dist3(rng);
	float x = dist3(rng);
	float z = dist3(rng);
	
	for(int i = 0; i < hsv[0].total(); i++){
       hsv[0].data[i]/=r;
	   r = dist3(rng);
    }
	for(int i = 0; i < hsv[1].total(); i++){
       hsv[1].data[i]*=x;
	   x = dist3(rng);
    }
	for(int i = 0; i < hsv[2].total(); i++){
       hsv[2].data[i]*=z;
	   z = dist3(rng);
    }
	
	Mat resimage;
	cv::merge(hsv, resimage);
	
	return resimage;
}

Mat deep_fry(Mat img) {
	Mat sharper = sharp_image(img);
	Mat fried = adjust_saturation(sharper);
	return fried;
}

Mat deep_fry2(Mat img) {
	Mat sharper = sharp_image2(img);
	Mat fried = adjust_saturation2(sharper);
	return fried;
}

Mat image_fx(Mat input_img, bool force_deep_fry_1 = false) {
	Mat image = input_img.clone();
	
	// Change image to be in HSV color space for adjust_saturation.
	if(rng.x == 1) {
		cv::cvtColor(image, image, COLOR_BGR2HSV);
	}
	Mat fried;
	// Force deep fry parameter: used from the imagepipe.cpp because deep_fry2 compresses
	// poorly, so videos always use the first method instead
	if(rng.y == 0 || force_deep_fry_1) {
		fried = deep_fry(image);
	}
	else {
		fried = deep_fry2(image);
	}
	
	return fried;
}

// Only define a main function in this file if we are compiling it to be
// run as an executable. Otherwise, the main() function will be defined
// by imagepipe.cpp.
#ifdef IMG_FX

int main(int argc, char **argv)
{
	if(argc < 3) {
		std::cout << "usage: image <input file> <output file>" << std::endl;
		return -1;
	}
	
	std::string in_path(argv[1]);
	std::string out_path(argv[2]);
	
	Mat image = cv::imread(in_path, IMREAD_COLOR);
	
	Mat fried = image_fx(image);
	
	imwrite(out_path, fried);
	
    return 0;
}

#endif