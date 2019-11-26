#include "FridoPipeline.h"
#include <chrono>

using namespace cv;
using namespace std;


namespace frido {

	/* variables for time measurement */
	double startProcess;
	double endProcess;

	/* constructor sets the variables to zero to avoid old storage values */
	FridoProcess::FridoProcess() {
		startProcess = 0;
		endProcess = 0;
	}

	/* Function is called by the main class and handles the order of the image process */
	void FridoProcess::Process(Mat& source0){

		/* writest the system time into the start variable */
		startProcess = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

		//resize
		Size cvResizeDsize(640, 480); //Resolution (x pixels, y pixels)
		double cvResizeFx = 1.0; //Stretch factor for the x axis
		double cvResizeFy = 1.0; //Stretch factor for the y axis
		int cvResizeInterpolation = INTER_LINEAR; //enum for the reducing or stretching mode of the method
		cvResize(source0, cvResizeDsize, cvResizeFx, cvResizeFy, cvResizeInterpolation, this->cvResizeOutput);

		//medianblur
		double cvMedianblurKsize = 5.0; //kernel size as a square with the value for one side
		cvMedianblur(cvResizeOutput, cvMedianblurKsize, this->cvMedianblurOutput);

		//HSV_Threshold
		double hsvThresholdHue[] = {0.0, 180.0}; //hue value range from 0 to 255
		double hsvThresholdSaturation[] = {0.0, 60.0}; //saturation value range from 0 to 255
		double hsvThresholdValue[] = {120, 255.0}; //brightness value range from 0 to 255
		hsvThreshold(cvMedianblurOutput, hsvThresholdHue, hsvThresholdSaturation, hsvThresholdValue, this->hsvThresholdOutput);

		//erode
		Mat cvErodeKernel; //Structure of erosion figure default 3x3 Ppixel square
		Point cvErodeAnchor(-1, -1); //Sets Anchor into center of erosion figure
		double cvErodeIterations = 1.0; //Times to do the erode method on the image
		int cvErodeBordertype = BORDER_CONSTANT; //enum to define how the image is expanded 
		Scalar cvErodeBordervalue(-1);  //used in case of a constant border, defines the color of the border
		cvErode(hsvThresholdOutput, cvErodeKernel, cvErodeAnchor, cvErodeIterations, cvErodeBordertype, cvErodeBordervalue, this->cvErodeOutput);

		//findContours
		bool findContoursExternalOnly = false; //defines which contorus should be found (changes between all and external only)
		findContours(cvErodeOutput, findContoursExternalOnly, this->findContoursOutput);	

		//filterContours
		double filterContoursMinArea = 1000.0; //in pixels
		double filterContoursMinWidth = 20.0; //in pixels
		double filterContoursMaxWidth = 150.0; //in pixels
		double filterContoursMinHeight = 50.0; //in pixels
		double filterContoursMaxHeight = 250.0; //in pixels
		filterContours(findContoursOutput, filterContoursMinArea, filterContoursMinWidth, filterContoursMaxWidth,
		filterContoursMinHeight, filterContoursMaxHeight, this->filterContoursOutput);
		
		/* writes the system time into the end variable and compares the time variables to calculate the time used in the Process class */
		endProcess = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
		cout << "Process Runtine: " << endProcess - startProcess << endl;
	}

	/**
	 * This method returns the resize output as a pointer.
	 * @return Mat output from resize.
	 */
	Mat* FridoProcess::GetCvResizeOutput(){
		return &(this->cvResizeOutput);
	}
	/**
	 * This method returns the medianBlur output as a pointer.
	 * @return Mat output from medianBlur.
	 */
	Mat* FridoProcess::GetCvMedianblurOutput(){
		return &(this->cvMedianblurOutput);
	}
	/**
	 * This method returns the HSV_Threshold output as a pointer.
	 * @return Mat output from HSV_Threshold.
	 */
	Mat* FridoProcess::GetHsvThresholdOutput(){
		return &(this->hsvThresholdOutput);
	}
	/**
	 * This method returns the erode output as a pointer.
	 * @return Mat output from erode.
	 */
	Mat* FridoProcess::GetCvErodeOutput(){
		return &(this->cvErodeOutput);
	}
	/**
	 * This method returns the found contours output as a pointer.
	 * @return Mat output from findContours.
	 */
	vector<vector<Point> >* FridoProcess::GetFindContoursOutput(){
		return &(this->findContoursOutput);
	}
	/**
	 * This method returns the filtered contours output as a pointer..
	 * @return Mat output from filterContours.
	 */
	vector<vector<Point> >* FridoProcess::GetFilterContoursOutput(){
		return &(this->filterContoursOutput);
	}
		

	/**
	 * Resizes an Image.
	 * @param src The image to resize.
	 * @param dSize size to set the image.
	 * @param fx scale factor along X axis.
	 * @param fy scale factor along Y axis.
	 * @param interpolation type of interpolation to use.
	 * @param dst output image.
	 */
	void FridoProcess::cvResize(Mat &src, Size &dSize, double fx, double fy, int interpolation, Mat &dst) {
		resize(src, dst, dSize, fx, fy, interpolation);
	}

	/** Sets a blur to the image
	 *  @param src the image to blur
	 *  @param kSize kernel size for blur
	 *  @param dst output image
	 */
	void FridoProcess::cvMedianblur(Mat &src, double kSize, Mat &dst) {
		medianBlur(src, dst, int(kSize));
	}

	/**
	 * Segment an image based on hue, saturation, and value ranges.
	 * @param input The image on which to perform the HSL threshold.
	 * @param hue The min and max hue.
	 * @param sat The min and max saturation.
	 * @param val The min and max value.
	 * @param output The image in which to store the output.
	 */
	void FridoProcess::hsvThreshold(Mat &input, double hue[], double sat[], double val[], Mat &out) {
		cvtColor(input, out, COLOR_BGR2HSV);
		inRange(out,Scalar(hue[0], sat[0], val[0]), Scalar(hue[1], sat[1], val[1]), out);
	}

	/**
	 * Expands area of lower value in an image.
	 * @param src the Image to erode
	 * @param kernel the kernel for erosion.
	 * @param anchor the center of the kernel.
	 * @param iterations the number of times to perform the erosion.
	 * @param borderType pixel extrapolation method.
	 * @param borderValue value to be used for a constant border.
	 * @param dst Output Image.
	 */
	void FridoProcess::cvErode(Mat &src, Mat &kernel, Point &anchor, double iterations, int borderType, Scalar &borderValue, Mat &dst) {
		erode(src, dst, kernel, anchor, (int)iterations, borderType, borderValue);	
	}

	/**
	 * Find Contours in an image..
	 * @param input The image to find contours in.
	 * @param externalOnly if only external contours are to be found.
	 * @param contours vector of contours to put contours in.
	*/	
	void FridoProcess::findContours(Mat &input, bool externalOnly, vector<vector<Point> > &contours) {
		vector<Vec4i> hierarchy; //hirarchy is a vetor for each contour with four integers for next, previous, parent and child countour
		contours.clear();
		int mode = externalOnly ? RETR_EXTERNAL : RETR_LIST; //Chooses mode dependig of bool state from variable externalOnly
		int method = CHAIN_APPROX_SIMPLE; //Does not store all points in the contour
		cv::findContours(input, contours, hierarchy, mode, method);
	}

	/**
	 * filter the found contours by some conditions
	 * @param inputContours contours found in findcontours method
	 * @param minArea the minimum surface of a contourArea
	 * @param minWidth minimum width in pixel
	 * @param maxWidth maximum width in pixel
	 * @param minHeight minimum height in pixel
	 * @param maxHeight maximum height in pixel
	 * @param output filtered contours output
	*/
	void FridoProcess::filterContours(vector<vector<Point> > &inputContours, double minArea, double minWidth, double maxWidth, double minHeight, double maxHeight, vector<vector<Point> > &output) {
		vector<Point> approx;
		output.clear();
		for(vector<Point> contour: inputContours) {
			Rect bb = boundingRect(contour);
			/** 
			 * approxPolyDP creates a polygon out of a bad shaped contour polygon.
			 * @param Input vector of Points for a contour
			 * @param approx output in form of a vector of points
			 * @param accuracy defines how far the approximate contour can be away from the input contour
			 * @param closed specifies if the input is in a closed shape
			 * 
			 * arcLength calculates the perimeter of the contour
			 * @param Input vector of Points for a contour
			 * @param closed specifies if the input is in a closed shape
			**/
			approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true)*0.02, true); 
			if (bb.width < minWidth || bb.width > maxWidth) continue;
			if (bb.height < minHeight ||bb.height > maxHeight) continue;
			double area = contourArea(contour);
			if (area < minArea) continue;
			/* Filters the contour if there are less then 4 or more then 5 corners */
			if (approx.size() < 4 || approx.size() > 6) continue;
			/* pushes the contour back to the output if the contour didnt get kicked out because of an invalid condition */
			output.push_back(contour);

		}
	}
} // end frido namespace

