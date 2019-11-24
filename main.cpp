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

	//VideoCapture cap("../Material/60FPS.mp4"); //Path for movies
	//cap.open("../Material/60FPS.mp4");

	// VideoCapture cap("../Material/retroWeit.jpg"); //Path for images -> dont forget the waitKey!
	// cap.open("../Material/retroWeit.jpg");


	//Generate Object of class to adress its methods
	frido::FridoProcess myprocess;
	frido::FridoCalculation mycalc;
	frido::FridoIllustrate myillu;

/**	NetworkTable::SetIPAddress("10.64.17.2");
	NetworkTable::SetClientMode();
	NetworkTable::Initialize();
	shared_ptr<NetworkTable> vision = NetworkTable::GetTable("vision");
	cout << "NetworkTables connected" << endl;**/

	if(!cap.isOpened()) {

		cout << "no camera" << endl;
		return -1;	

	}

	while(true) {
		
		start = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

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

				cout << "X Offset: " << preparedNetworkTables[2] << endl;
				cout << "Y Offset: " << preparedNetworkTables[3] << endl;
//				heights = *mycalc.FridoCalculation::GetCalculateHeightsOutput();

//				cout << "aveage height: " << heights[0] << endl;

			} else {
				target = false;
			}

/**			vision->PutNumber("Distance", preparedNetworkTables[0]);
			vision->PutNumber("Angle", preparedNetworkTables[1]);
			vision->PutNumber("XOffset", preparedNetworkTables[2]);
			vision->PutNumber("YOffset", preparedNetworkTables[3]);
			vision->PutBoolean("Target", target);**/

			myillu.FridoIllustrate::Illustrate(&myprocess, &mycalc, true, false);

			
			// imshow("Contours", *myillu.GetContoursFrame());
//			imwrite("../Results/OffsetDIS50FOV?.jpg", *myillu.GetEntireFrame());
			imshow("Image", *myillu.GetEntireFrame());
//			imshow("raw", frame);
			end = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
			cout << "Runtime main: " << end - start << endl;
			if(waitKey(1) >= 0) //waitKey(1) for Videos //waitKey(0) for Pictures
			break;
		}	

	}

	cap.release();

}
