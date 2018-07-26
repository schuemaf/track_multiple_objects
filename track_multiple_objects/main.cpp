//General
#include<iostream>
#include <sstream>
#include <string>
#include <vector>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Realsense
#include <librealsense2/rs.hpp>
#include "cv-helpers.hpp"
#include "example.hpp" 

//OpenCV
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cv.h>

// Class
#include "Obstacle.h"

//namespace
using namespace std;
using namespace cv;
using namespace cv::dnn;
using namespace rs2;

//initial min and max HSV filter values for sliders
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int leftBoarder = 300;
int rightBoarder = 900;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "depth";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

//This function gets called whenever atrackbar position is changed
void on_trackbar(int, void*)
{
}

// Helper funktion to count
string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

//Create sliders to afjuste de values
void createTrackbars() {
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
     
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

//1Display names and center
void drawObject(vector<Obstacle> theObjects, Mat &frame) {

	for (int i = 0; i < theObjects.size(); i++)
	{
		cv::circle(frame, Point(theObjects.at(i).getxPos(), theObjects.at(i).getyPos()), 10, cv::Scalar(0, 0, 255));
		cv::putText(frame, intToString(theObjects.at(i).getxPos()) + " , " + intToString(theObjects.at(i).getyPos()), cv::Point(theObjects.at(i).getxPos(), theObjects.at(i).getyPos() + 20), 1, 1, Scalar(0, 255, 0));
		cv::putText(frame, theObjects.at(i).getType(), cv::Point(theObjects.at(i).getxPos(), theObjects.at(i).getyPos() - 30), 1, 4, theObjects.at(i).getColor());
		
		
		cout << theObjects.at(i).getxPos() << endl;
		if (theObjects.at(i).getxPos() >leftBoarder && (theObjects.at(i).getxPos()) < rightBoarder) {

			cout << "move " << theObjects.at(i).getxPos() << endl;
		}
	}
}
//void calculateWay(vector<Obstacle>theObjects) {
//	for (int i = 0; i < theObjects.size(); i++) {
//		
//		if (theObjects.at(i).getxPos() >leftBoarder && (theObjects.at(i).getxPos()) < rightBoarder) {
//
//			cout<<"move "<<theObjects.at(i).getxPos() << endl;
//		}
//		
//	}
//
//}



// deletenoise and fill holes
void morphOps(Mat &thresh) {
	
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);
	
	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);

}

// Track objects
void trackFilteredObject(Mat threshold, Mat &cameraFeed) {


	vector<Obstacle> close;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {

					Obstacle close1;

					close1.setxPos(moment.m10 / area);
					close1.setyPos(moment.m01 / area);

					close.push_back(close1);

					objectFound = true;

				}
				else objectFound = false;

			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				drawObject(close, cameraFeed);
				
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}
void trackFilteredObject(Obstacle theObject, Mat threshold, Mat &cameraFeed) {

	vector<Obstacle> close;

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {
					Obstacle close1;

					close1.setxPos(moment.m10 / area);
					close1.setyPos(moment.m01 / area);
					close1.setType(theObject.getType());
					close1.setColor(theObject.getColor());

					close.push_back(close1);
					objectFound = true;

				}
				else objectFound = false;

			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				drawObject(close, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

int main() {
	//set modes:s
	bool trackObjects = true;
	bool useMorphOps = true;
	bool calibrationMode = false;
	vector<Obstacle> test;

	//matrix storage for binary threshold image
	Mat threshold;

	//create slider bars for HSV filtering if calibrationmode is set to true
	if (calibrationMode) {
		createTrackbars();
	}

	// Initialise Depth camera
	rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);

	// Declare texture on the GPU
	texture depth_image;

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	// Declare RealSense pipeline
	rs2::pipeline pipe;

	// Start streaming with default configuration
	pipe.start();

	while (1) // inf. loop
	{
		frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		frame depth = color_map(data.get_depth_frame()); // Find and colorize the depth data

															  // Convert RealSense frame to OpenCV matrix:
		auto depth_mat = frame_to_mat(depth);


		if (calibrationMode == true) {
			inRange(depth_mat, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
			//perform morphological operations on thresholded image to eliminate noise
			//and emphasize the filtered object(s)
			if (useMorphOps)
				morphOps(threshold);
			//pass in thresholded frame to our object tracking function
			//this function will return the x and y coordinates of the
			//filtered object
			if (trackObjects)
				trackFilteredObject(threshold, depth_mat);


		}
		else
		{

			Obstacle close("close"), mid("mid"), away("away");



			//near objects
			inRange(depth_mat, close.getHSVmin(), close.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(close, threshold, depth_mat);

			// mid objects
			inRange(depth_mat, mid.getHSVmin(), mid.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(mid, threshold, depth_mat);

			//far objects
			inRange(depth_mat, away.getHSVmin(), away.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(away, threshold, depth_mat);
		}

		// Draw rectangangle 
		rectangle(depth_mat, Point(340, 0), Point(940, 720), Scalar(255), 5, 8, 0);

		//show frames 
		if (calibrationMode) {
			imshow(windowName2, threshold);
		}
		imshow(windowName, depth_mat);
		/*calculateWay(test);*/
		


		//delay 30ms so that screen can refresh
		waitKey(30);

	}
	return 0;
}
