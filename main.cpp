#include <iostream>
#include <algorithm>
#include "FridoPipeline.h"
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>


#define PI 3.14159265

using namespace std;
using namespace cv;

int main(void) {

	Mat frame;

	Mat* pointer;
	Mat processedFrame;

	vector<vector<Point> >* contours;
	vector<vector<Point> > processedContours;
	vector<Point> space_holder_contour;

	Point space_holder_point;
	int sortArray[4];

	RotatedRect target_left;
	RotatedRect target_right;

	Point2f box_left_2f[4];
	Point box_left[4];
	Point2f box_right_2f[4];
	Point box_right[4];

	Point box_left_sorted[4];
	Point box_right_sorted[4];

	Point upperCenterX;
	Point lowerCenterX;
	Point centerTarget;
	Point leftMid;
	Point rightMid;

	int height = 0;
	int absAngle = 0;
	int distance = 0;
	int angle = 0;

	int heightLeft = 0;
	int heightRight = 0;

	int distanceY = 0;
	int distanceX = 0;

	Point zero[2];
	zero[0].x = 0;
	zero[0].y = 0;
	zero[1].x = 320;
	zero[1].y = 240;

	Mat blackImage(480, 640, CV_8UC3);
	blackImage.setTo(0);

	VideoCapture cap(0); //0 for camera on port 0
	cap.open(0);

	// VideoCapture cap("../Material/Distance.mov"); //Path for movies
	// cap.open("../Material/Distance.mov");

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

			if (processedContours.size() == 2) {	
				if (processedContours[0][0].x > processedContours[1][0].x) {
					space_holder_contour = processedContours[0];
					processedContours[0] = processedContours[1];
					processedContours[1] = space_holder_contour;
				}
			}

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

				for (int i = 0; i < 4; i++) {
					sortArray[i] = box_left[i].y;
				}

				sort(sortArray, sortArray + 4);

				for (int i = 0; i < 4; i++) {
					for (int a = 0; a < 4; a++) {
						if (sortArray[i] == box_left[a].y) {
							box_left_sorted[i] = box_left[a];
							break;
						}
					}
				}

				for (int i = 0; i > 4; i += 2) {
					if(box_left_sorted[i].x < box_left_sorted[i + 1].x) {
						space_holder_point = box_left_sorted[i];
						box_left_sorted[i] = box_left_sorted[i + 1];
						box_left_sorted[i + 1] = space_holder_point;
					}
				}

				leftMid = box_left_sorted[1] + (box_left_sorted[3] - box_left_sorted[1]) / 2;

				circle(processedFrame, leftMid, 5, zeroColor, 3);

				// cout << "Point_1_left " << box_left_sorted[0] << endl;
				// cout << "Point_2_left " << box_left_sorted[1] << endl;
				// cout << "Point_3_left " << box_left_sorted[2] << endl;
				// cout << "Point_4_left " << box_left_sorted[3] << endl;

				circle(processedFrame, box_left_sorted[0], 5, circleColor, 3);
				circle(processedFrame, box_left_sorted[1], 5, circleColor, 3);
				circle(processedFrame, box_left_sorted[2], 5, circleColor, 3);
				circle(processedFrame, box_left_sorted[3], 5, circleColor, 3);
			
			}
		
			if (processedContours.size() > 1) {

				for (int i = 0; i < 4; i++) {
					sortArray[i] = box_right[i].y;
				}

				sort(sortArray, sortArray + 4);

				for (int i = 0; i < 4; i++) {
					for (int a = 0; a < 4; a++) {
						if (sortArray[i] == box_right[a].y) {
							box_right_sorted[i] = box_right[a];
							break;
						}
					}
				}

				for (int i = 0; i > 4; i += 2) {
					if(box_right_sorted[i].x < box_right_sorted[i + 1].x) {
						space_holder_point = box_right_sorted[i];
						box_right_sorted[i] = box_right_sorted[i + 1];
						box_right_sorted[i + 1] = space_holder_point;
					}
				}

				rightMid = box_right_sorted[1] + (box_right_sorted[3] - box_right_sorted[1]) / 2;

				circle(processedFrame, rightMid, 5, circleColor, 3);
			
				// cout << "Point_1_right " << box_right_sorted[0] << endl;
				// cout << "Point_2_right " << box_right_sorted[1] << endl;
				// cout << "Point_3_right " << box_right_sorted[2] << endl;
				// cout << "Point_4_right " << box_right_sorted[3] << endl;

				circle(processedFrame, box_right_sorted[0], 5, circleColor, 3);
				circle(processedFrame, box_right_sorted[1], 5, circleColor, 3);
				circle(processedFrame, box_right_sorted[2], 5, circleColor, 3);
				circle(processedFrame, box_right_sorted[3], 5, circleColor, 3);

			}

			if (processedContours.size() == 2) {
				upperCenterX = box_left_sorted[0] + (box_right_sorted[0] - box_left_sorted[0]) / 2;

				circle(processedFrame, upperCenterX, 5, XcenterColor, 3);
				line(processedFrame, box_left_sorted[0], box_right_sorted[0], zeroColor, 3);

				lowerCenterX = box_left_sorted[2] + (box_right_sorted[2] - box_left_sorted[2]) / 2;

				circle(processedFrame, lowerCenterX, 5, XcenterColor, 3);
				line(processedFrame, box_left_sorted[2], box_right_sorted[2], zeroColor, 3);

				height = lowerCenterX.y - upperCenterX.y;

				centerTarget.y = upperCenterX.y + (lowerCenterX.y - upperCenterX.y) / 2;
				centerTarget.x = upperCenterX.x + (lowerCenterX.x - upperCenterX.x) / 2;
				circle(processedFrame, centerTarget, 5, zeroColor, 3);
				
				heightLeft = box_left_sorted[3].y - box_left_sorted[0].y;
				heightRight = box_right_sorted[3].y - box_right_sorted[0].y;

				angle = (heightLeft - heightRight) / 2;
				absAngle = abs(angle);

				distance = 37.0 + (308.5 / pow(2, (height / 41.5)));


				cout << "Distance: " << distance << " cm" << endl;

				// cout << "Height Left: " << heightLeft << " ; Height Right: " << heightRight << endl;

				cout << "Angle: " << angle << endl;

				cout << "Y Distance: " << cos(absAngle * PI / 180) * distance << endl;
				cout << "X Distance: " << sin(absAngle * PI / 180) * distance << endl;

				cout << "Offset: " << centerTarget - zero[1] << endl;
	
			}
			
			// imshow("Contours", blackImage);
			// imwrite("../Results/retroWeitHeight.jpg", processedFrame);
			imshow("Image", processedFrame);
//			imshow("raw", frame);	

			if(waitKey(5) >= 0) //waitKey(5) for Videos //waitKey(0) for Pictures
			break;
		}	

	}

}
