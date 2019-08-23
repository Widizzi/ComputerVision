#include <iostream>
#include "FridoPipeline.h"
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(void) {

	Mat frame;

	Mat* pointer;
	Mat processedFrame;

	vector<vector<Point> >* contours;
	vector<vector<Point> > processedContours;  

	RotatedRect target_left;
	RotatedRect target_right;

	Point2f box_left_2f[4];
	Point box_left[4];
	Point2f box_right_2f[4];
	Point box_right[4];

	Point upperCenterX;
	Point lowerCenterX;

	int height = 0;
	string str;
	stringstream ss;

	Point zero[2];
	zero[0].x = 0;
	zero[0].y = 0;
	zero[1].x = zero[1].y = 50;

	Mat blackImage(480, 640, CV_8UC3);
	blackImage.setTo(0);

//	VideoCapture cap(0); //0 for camera on port 0
//	cap.open(0);

	VideoCapture cap("../Material/Distance.mov"); //Path for movies
	cap.open("../Material/Distance.mov");

	// VideoCapture cap("../Material/retroWeit.jpg"); //Path for images -> dont forget the waitKey!
	// cap.open("../Material/retroWeit.jpg");


	frido::FridoPipeline myfrido;

	if(!cap.isOpened()) {

		cout << "no camera" << endl;
		return -1;	

	}

	while(true) {

		bool rGood = cap.read(frame);

		if(!rGood) {

			cout << "bad reach form camera" << endl;
				
		} else {
		
			blackImage.setTo(0);
			myfrido.FridoPipeline::Process(frame);

			
			pointer = myfrido.FridoPipeline::GetCvResizeOutput();
			processedFrame = *pointer;

			contours = myfrido.FridoPipeline::GetFilterContoursOutput();
			processedContours = *contours;

			Scalar color(255, 255,255);
			drawContours(blackImage, processedContours, -1, color, 3);

			Scalar rectColor(0, 255, 0);

//			cout << processedContours.size() << endl;

			if (processedContours.size() > 0) {
				target_left = minAreaRect(processedContours[0]);
				target_left.points(box_left_2f);
				for(int i = 0; i < 4; i++) {
					box_left[i] = box_left_2f[i];
				}
				fillConvexPoly(processedFrame, box_left, 4, rectColor);
			}
			if (processedContours.size() > 1) {
				target_right = minAreaRect(processedContours[1]);
				target_right.points(box_right_2f);
				for(int i = 0; i < 4; i++) {
					box_right[i] = box_right_2f[i];
				}
				fillConvexPoly(processedFrame, box_right, 4, rectColor);
			}
				

			Scalar circleColor(255, 255, 150);

			Scalar XcenterColor(0, 0, 0);
			Scalar YcenterColor(0, 0, 0);
			Scalar targetCenterColor(0, 255, 255);


			Scalar zeroColor(0, 0, 255);

//			cout << zero[0] << endl;

			circle(processedFrame, zero[0], 5, zeroColor, 3);
			


			if (processedContours.size() > 0) {

				// cout << "Point_1_left " << box_left[0] << endl;
				// cout << "Point_2_left " << box_left[1] << endl;
				// cout << "Point_3_left " << box_left[2] << endl;
				// cout << "Point_4_left " << box_left[3] << endl;

				circle(processedFrame, box_left[0], 5, circleColor, 3);
				circle(processedFrame, box_left[1], 5, circleColor, 3);
				circle(processedFrame, box_left[2], 5, circleColor, 3);
				circle(processedFrame, box_left[3], 5, circleColor, 3);

				// cout << "Mid  " << (box_left[3] - box_left[0]) / 2 << endl;
				// cout << "Center " << box_left[0] + (box_left[3] - box_left[0]) / 2 << endl;
				// circle(processedFrame, box_left[0] + (box_left[3] - box_left[0]) / 2, 5, YcenterColor, 3);

				// circle(processedFrame, box_left[1] + (box_left[0] - box_left[1]) / 2, 5, XcenterColor, 3);
			
			}
		
			if (processedContours.size() > 1) {
			
				// cout << "Point_1_right " << box_right[0] << endl;
				// cout << "Point_2_right " << box_right[1] << endl;
				// cout << "Point_3_right " << box_right[2] << endl;
				// cout << "Point_4_right " << box_right[3] << endl;

				circle(processedFrame, box_right[0], 5, circleColor, 3);
				circle(processedFrame, box_right[1], 5, circleColor, 3);
				circle(processedFrame, box_right[2], 5, circleColor, 3);
				circle(processedFrame, box_right[3], 5, circleColor, 3);

			}

			if (processedContours.size() == 2) {
				upperCenterX = box_left[2] + (box_right[2] - box_left[2]) / 2;

				circle(processedFrame, upperCenterX, 5, XcenterColor, 3);
				line(processedFrame, box_left[2], box_right[2], zeroColor, 3);

				lowerCenterX = box_left[1] + (box_right[3] - box_left[1]) / 2;

				circle(processedFrame, lowerCenterX, 5, XcenterColor, 3);
				line(processedFrame, box_left[1], box_right[3], zeroColor, 3);

				height = lowerCenterX.y - upperCenterX.y;
				ss << height;
				str = ss.str();

				putText(processedFrame, str, zero[1], CV_FONT_HERSHEY_SIMPLEX, 1, circleColor, 1);
			}
			
//			imshow("Contours", blackImage);
			// imwrite("../Results/retroWeitHeight.jpg", processedFrame);
			imshow("Image", processedFrame);
//			imshow("raw", frame);	

			if(waitKey(5) >= 0) //waitKey(5) for Videos //waitKey(0) for Pictures
			break;
		}	

	}

}
