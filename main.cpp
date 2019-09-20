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

	vector<vector<Point> > contours;
	vector<double> preparedNetworkTables;

	VideoCapture cap(0); //0 for camera on port 0
	cap.open(0);

	// VideoCapture cap("../Material/Distance.mov"); //Path for movies
	// cap.open("../Material/Distance.mov");

	// VideoCapture cap("../Material/retroWeit.jpg"); //Path for images -> dont forget the waitKey!
	// cap.open("../Material/retroWeit.jpg");


	frido::FridoProcess myprocess;
	frido::FridoCalculation mycalc;
	frido::FridoIllustrate myillu;

	if(!cap.isOpened()) {

		cout << "no camera" << endl;
		return -1;	

	}

	while(true) {

		bool rGood = cap.read(frame);

		if(!rGood) {

			cout << "bad reach form camera" << endl;
				
		} else {
		
			myprocess.FridoProcess::Process(frame);

			contours = *myprocess.FridoProcess::GetFilterContoursOutput();
			
			if (contours.size() == 2) {
				mycalc.FridoCalculation::Calculate(contours);		

				preparedNetworkTables = *mycalc.FridoCalculation::GetPrepareNetworkTablesOutput();

				cout << "Distance: " << preparedNetworkTables[0] << " cm" << endl;

				cout << "Angle: " << preparedNetworkTables[1] << endl;

				cout << "X Distance: " << preparedNetworkTables[2] << endl;
				cout << "Y Distance: " << preparedNetworkTables[3] << endl;

				cout << "X Offset: " << preparedNetworkTables[4] << endl;
				cout << "Y Offset: " << preparedNetworkTables[5] << endl;

			}

			myillu.FridoIllustrate::Illustrate(&myprocess, &mycalc, true, false);

			
			// imshow("Contours", *myillu.GetContoursFrame());
			// imwrite("../Results/retroWeitHeight.jpg", *myillu.GetEntireFrame());
			imshow("Image", *myillu.GetEntireFrame());
//			imshow("raw", frame);	

			if(waitKey(5) >= 0) //waitKey(5) for Videos //waitKey(0) for Pictures
			break;
		}	

	}

}
