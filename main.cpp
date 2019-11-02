#include <iostream>
#include <algorithm>
#include "FridoPipeline.h"
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <chrono>

#include <ntcore.h>
#include <networktables/NetworkTable.h>


#define PI 3.14159265

using namespace std;
using namespace cv;
using std::shared_ptr;

int main(void) {

	Mat frame;
	bool target;
	vector<vector<Point> > contours;
	vector<double> preparedNetworkTables;
	vector<double> heights;
	int start = 0;
	int end = 0;

	VideoCapture cap(0); //0 for camera on port 0
	cap.open(0);

	//VideoCapture cap("../Material/Test.mov"); //Path for movies
	//cap.open("../Material/Test.mov");

	// VideoCapture cap("../Material/retroWeit.jpg"); //Path for images -> dont forget the waitKey!
	// cap.open("../Material/retroWeit.jpg");


	frido::FridoProcess myprocess;
	frido::FridoCalculation mycalc;
	frido::FridoIllustrate myillu;

	NetworkTable::SetIPAddress("10.64.17.2");
	NetworkTable::SetClientMode();
	NetworkTable::Initialize();
	shared_ptr<NetworkTable> vision = NetworkTable::GetTable("vision");
	cout << "NetworkTables connected" << endl;

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
				target = true;
				mycalc.FridoCalculation::Calculate(contours);		

				preparedNetworkTables = *mycalc.FridoCalculation::GetPrepareNetworkTablesOutput();

				cout << "Distance: " << preparedNetworkTables[0] << " cm" << endl;

				cout << "Angle: " << preparedNetworkTables[1] << endl;

	//			cout << "X Distance: " << preparedNetworkTables[2] << endl;
	//			cout << "Y Distance: " << preparedNetworkTables[3] << endl;

				cout << "X Offset: " << preparedNetworkTables[4] << endl;
	//			cout << "Y Offset: " << preparedNetworkTables[5] << endl;
//				heights = *mycalc.FridoCalculation::GetCalculateHeightsOutput();

//				cout << "aveage height: " << heights[0] << endl;
//				cout << "New Distance: " << pow(7087 / heights[0], 1.0 / 0.941) << endl;
//				cout << "Old Distance: " << 37 + (308.5 / pow(2, (heights[0] / 41.5))) << endl;

			} else {
				target = false;
			}

			vision->PutNumber("Distance", preparedNetworkTables[0]);
			vision->PutNumber("Angle", preparedNetworkTables[1]);
			vision->PutNumber("XDistance", preparedNetworkTables[2]);
			vision->PutNumber("YDistance", preparedNetworkTables[3]);
			vision->PutNumber("XOffset", preparedNetworkTables[4]);
			vision->PutNumber("YOffset", preparedNetworkTables[5]);
			vision->PutBoolean("Target", target);

			myillu.FridoIllustrate::Illustrate(&myprocess, &mycalc, true, false);

			
			// imshow("Contours", *myillu.GetContoursFrame());
			// imwrite("../Results/retroWeitHeight.jpg", *myillu.GetEntireFrame());
			imshow("Image", *myillu.GetEntireFrame());
//			imshow("raw", frame);
			start = end;
			end = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
		//	cout << "Runtime: " << end - start << endl;
			if(waitKey(1) >= 0) //waitKey(1) for Videos //waitKey(0) for Pictures
			break;
		}	

	}

	cap.release();

}
