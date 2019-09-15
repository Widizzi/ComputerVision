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
	vector<vector<Point> > convertedContours;

	vector<vector<Point> >* box_points_pointer;
	vector<vector<Point> > box_points;

	vector<Point> processedContours;
	vector<Point>* processedContoursPointer;

	vector<double>* anglePointer;
	vector<double> angle;

	double* distancePointer;
	double distance;

	Point left_points[4];
	Point right_points[4];

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
	frido::FridoCalculation mycalc;

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
			convertedContours = *contours;

			Scalar white_color(255, 255, 255);
			drawContours(blackImage, convertedContours, -1, white_color, 3);

			if (convertedContours.size() == 2) {
				mycalc.FridoCalculation::Calculate(convertedContours);

				box_points_pointer = mycalc.FridoCalculation::GetSortCornersOutput();
				box_points = *box_points_pointer;

				for (int i = 0; i < 4; i++) {
					left_points[i] = box_points[0][i];
				}
				
				for (int i = 0; i < 4; i++) {
					right_points[i] = box_points[1][i];
				}

				processedContoursPointer = mycalc.FridoCalculation::GetCalculatePointsOutput();
				processedContours = *processedContoursPointer;

				anglePointer = mycalc.FridoCalculation::GetCalculateAngleOutput();
				angle = *anglePointer;

				distancePointer = mycalc.FridoCalculation::GetCalculateDistanceOutput();
				distance = *distancePointer;

				Scalar rectColor(0, 255, 0);

				fillConvexPoly(processedFrame, left_points, 4, rectColor);
				fillConvexPoly(processedFrame, right_points, 4, rectColor);				

				Scalar circleColor(255, 255, 150);

				Scalar XcenterColor(0, 0, 0);
				Scalar YcenterColor(0, 0, 0);
				Scalar targetCenterColor(0, 255, 255);


				Scalar zeroColor(0, 0, 255);

				circle(processedFrame, zero[0], 5, zeroColor, 3);
			
				circle(processedFrame, processedContours[0], 5, zeroColor, 3);

				circle(processedFrame, box_points[0][0], 5, circleColor, 3);
				circle(processedFrame, box_points[0][1], 5, circleColor, 3);
				circle(processedFrame, box_points[0][2], 5, circleColor, 3);
				circle(processedFrame, box_points[0][3], 5, circleColor, 3);

				circle(processedFrame, processedContours[1], 5, circleColor, 3);

				circle(processedFrame, box_points[1][0], 5, circleColor, 3);
				circle(processedFrame, box_points[1][1], 5, circleColor, 3);
				circle(processedFrame, box_points[1][2], 5, circleColor, 3);
				circle(processedFrame, box_points[1][3], 5, circleColor, 3);

			
				circle(processedFrame, processedContours[2], 5, XcenterColor, 3);
				line(processedFrame, box_points[0][0], box_points[1][0], zeroColor, 3);

				circle(processedFrame, processedContours[3], 5, XcenterColor, 3);
				line(processedFrame, box_points[0][2], box_points[1][2], zeroColor, 3);

				circle(processedFrame, processedContours[4], 5, zeroColor, 3);

				cout << "Distance: " << distance << " cm" << endl;

				cout << "Angle: " << angle[0] << endl;

				cout << "Y Distance: " << cos(angle[1] * PI / 180) * distance << endl;
				cout << "X Distance: " << sin(angle[1] * PI / 180) * distance << endl;

				cout << "Offset: " << processedContours[4] - zero[1] << endl;
	
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
