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
	bool networkTables = false;
	vector<vector<Point> > contours;
	vector<double> preparedNetworkTables;
	vector<double> heights;
	int startMain = 0;
	int endMain = 0;
	shared_ptr<NetworkTable> vision;

	/* 0 for camera on port 0 */
	VideoCapture cap(0); 
	cap.open(0);

/*
	VideoCapture cap("../Material/60FPS.mp4"); //Path for movies
	cap.open("../Material/60FPS.mp4");
*/

/*
	VideoCapture cap("../Material/retroWeit.jpg"); //Path for images -> dont forget the waitKey!
	cap.open("../Material/retroWeit.jpg");
*/

	/* Generate Object of class to adress its methods */
	frido::FridoProcess myprocess;
	frido::FridoCalculation mycalc;
	frido::FridoIllustrate myillu;

	/* try to initialize the NetworkTables 
	 * if it fails disable the usage
	*/
	try {
		NetworkTable::SetIPAddress("10.64.17.2");
		NetworkTable::SetClientMode();
		NetworkTable::Initialize();
		vision = NetworkTable::GetTable("vision");
		networkTables = true;
		cout << "NetworkTables connected" << endl;
	} catch (std::exception& e) {
		networkTables = false;
		cout << "Exception: No NetworTables aviable" << endl;
	}

	/* check if an input is aviable */
	if(!cap.isOpened()) {
		 /* cancel the execution if no input is aviable */
		cout << "no input" << endl;
		return -1;	

	}

	while(true) {
		
		/* start time measurement with system clock */
		startMain = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

		/* try to read the frame from the input and save the result in the rGood bool. */
		bool rGood = cap.read(frame);

		/* if the frame couldnt been read dont execute the programm
		 * ! doesnt cancel the programm
		 */
		if(!rGood) {

			cout << "bad reach form camera" << endl;
				
		} else {
		
			/* start the Process class with the frame read before */ 
			myprocess.FridoProcess::Process(frame);

			/* save the value of the returned pointer from the filtered contours */
			contours = *myprocess.FridoProcess::GetFilterContoursOutput();
			
			/* check the amount of contours (calculation just works with two vision-targets parts) */
			if (contours.size() == 2) {
				/* set the value for the NetworkTable */
				target = true;

				/* start the calculation with the two contours */
				mycalc.FridoCalculation::Calculate(contours);		

				/* grab the output of the calculation class to print out and to send to the NetworkTable */
				preparedNetworkTables = *mycalc.FridoCalculation::GetPrepareNetworkTablesOutput();

				/* some prints to check the calculatet values */
				cout << "Distance: " << preparedNetworkTables[0] << " cm" << endl;
				cout << "Angle: " << preparedNetworkTables[1] << endl;
				cout << "X Offset: " << preparedNetworkTables[2] << endl;
				cout << "Y Offset: " << preparedNetworkTables[3] << endl;
//				heights = *mycalc.FridoCalculation::GetCalculateHeightsOutput();
//				cout << "aveage height: " << heights[0] << endl;

			} else {
				/* resets the target value for the NetworkTables if the amount of contours changes */
				target = false;
			}

			/* put the values to the NetworkTables if they were initialized */
			if (networkTables == true) {
				vision->PutNumber("Distance", preparedNetworkTables[0]);
				vision->PutNumber("Angle", preparedNetworkTables[1]);
				vision->PutNumber("XOffset", preparedNetworkTables[2]);
				vision->PutNumber("YOffset", preparedNetworkTables[3]);
				vision->PutBoolean("Target", target);
			}

			/* start the illustration class with the objects of the other classes
			 * first bools activates the entire drawing
			 * second bools activates the contours image
			 */
			myillu.FridoIllustrate::Illustrate(&myprocess, &mycalc, true, false);

			/* commands to show the generated images from the illustration class or the raw frame */
//			imshow("Contours", *myillu.GetContoursFrame());
//			imshow("raw", frame);
			imshow("Image", *myillu.GetEntireFrame());

			/* opportunity to save the generated output (just works with images) */
			imwrite("../Results/OffsetDIS50FOV?.jpg", *myillu.GetEntireFrame());

			/* set the system time to the end variable to calculate the time passed in the while loop */
			endMain = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
			cout << "Runtime main: " << endMain - startMain << endl;

			/* waitKey(1) for Videos / waitKey(0) for Pictures */
			if(waitKey(1) >= 0)
			break;
		}	

	}

	cap.release();

}
