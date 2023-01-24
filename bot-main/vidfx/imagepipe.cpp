#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <sstream>

#include <stdio.h>

using namespace cv;

int frame_width;
int frame_height;
double video_length;
int video_frame_count;

void probe_video_info(const char *filename) {
	std::stringstream cmdss;
	cmdss << "ffprobe "
	      << "-v error "
		  << "-select_streams v "
		  << "-show_entries stream=width,height,duration,nb_frames "
		  << "-of csv=p=0 "
		  << filename;
	
	std::string cmd(cmdss.str()); // Need to explictly store string
	FILE *probe = popen(cmd.c_str(), "r");
	fscanf(probe, "%d,%d,%lf,%d",
		&frame_width,
		&frame_height,
		&video_length,
		&video_frame_count);
	
	fflush(probe);
	pclose(probe);
}

FILE *open_in_video(const char *filename) {
	std::stringstream cmdss;
	
	cmdss << "ffmpeg "
	      << "-i " << filename << " "
		  << "-f rawvideo "
		  << "-vcodec rawvideo "
		  << "-pix_fmt bgr24 "
		  << "- "; // Output file = pipe to stdout

	std::string cmd(cmdss.str()); // Need to explictly store string
	return popen(cmd.c_str(), "rb");
}

FILE *open_out_video(const char *filename) {
	double fps = video_frame_count / video_length;
	double output_fps = fps;
	/*while(output_fps > 16) {
		output_fps /= 2.0;
	}*/
	
	std::cout << "Encoding at framerate: " << fps << std::endl;
	
	std::stringstream cmdss;
	
	cmdss << "ffmpeg "
	      << "-y "
		  << "-f rawvideo "
		  << "-vcodec rawvideo "
		  << "-pix_fmt bgr24 "
		  << "-s " << frame_width << "x" << frame_height << " "
		  << "-framerate " << fps << " "
		  << "-i - " // Input file = pipe from stdin
		  << "-f mp4 "
		  << "-c:v libx264 "
		  << "-crf 28 "
		  << "-preset veryfast "
		  //<< "-q:v 9 "
		  << "-r " << output_fps << " "
		  << "-an "
		  //<< "-vcodec h264 "
		  << "-pix_fmt yuv420p "
		  << filename; // Output file

	std::string cmd(cmdss.str()); // Need to explictly store string
	return popen(cmd.c_str(), "wb");
}

// This is the function implemented in image.cpp. That way we don't have to worry
// about having all the implementation in one file.
extern Mat image_fx(Mat img, bool);

int main(int argc, char **argv) {
	if(argc < 3) {
		printf("usage: %s <in.mp4> <out.mp4>\n", argv[0]);
		return -1;
	}
	
	probe_video_info(argv[1]);
	
	FILE *in = open_in_video(argv[1]);
	FILE *out = open_out_video(argv[2]);
	
	Mat in_mat(frame_width, frame_height, CV_8UC3);
	
	int byte_count = frame_width * frame_height * 3;
	
	// https://batchloaf.wordpress.com/2017/02/12/a-simple-way-to-read-and-write-audio-and-video-files-in-c-using-ffmpeg-part-2-video/
	for(;;) {
		size_t s = fread(in_mat.ptr(), 1, byte_count, in);
		if(s == 0) break;
		
		Mat fried = image_fx(in_mat, true);
	
		std::fwrite(fried.ptr(), 1, byte_count, out);
	}
	
	fflush(in);
	fflush(out);
	
	pclose(in);
	pclose(out);
	
	return 0;
}