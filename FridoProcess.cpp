#include "FridoPipeline.h"
#include <chrono>

using namespace cv;
using namespace std;


namespace frido {

double start;
double end;

FridoProcess::FridoProcess() {
	start = 0;
	end = 0;
}
/**
* Runs an iteration of the pipeline and updates outputs.
*/
void FridoProcess::Process(Mat& source0){
	start = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
	//Step CV_resize0:
	//input
//	Mat cvResizeSrc = source0;
	Size cvResizeDsize(640, 480);
	double cvResizeFx = 1.0;  // default Double
	double cvResizeFy = 1.0;  // default Double
    int cvResizeInterpolation = INTER_LINEAR;
//	cvResize(cvResizeSrc, cvResizeDsize, cvResizeFx, cvResizeFy, cvResizeInterpolation, this->cvResizeOutput);
	cvResize(source0, cvResizeDsize, cvResizeFx, cvResizeFy, cvResizeInterpolation, this->cvResizeOutput);
	//Step CV_Medianblur0;
	//input
//	Mat cvMedianblurSrc = cvResizeOutput;
	double cvMedianblurKsize = 1.0;
//	cvMedianblur(cvMedianblurSrc, cvMedianblurKsize, this->cvMedianblurOutput);
	cvMedianblur(cvResizeOutput, cvMedianblurKsize, this->cvMedianblurOutput);
	//Step HSV_Threshold0:
	//input
//	Mat hsvThresholdInput = cvMedianblurOutput;
	double hsvThresholdHue[] = {0.0, 180.0};
	double hsvThresholdSaturation[] = {0.0, 60.0};
	double hsvThresholdValue[] = {120, 255.0};
//	hsvThreshold(hsvThresholdInput, hsvThresholdHue, hsvThresholdSaturation, hsvThresholdValue, this->hsvThresholdOutput);
	hsvThreshold(cvMedianblurOutput, hsvThresholdHue, hsvThresholdSaturation, hsvThresholdValue, this->hsvThresholdOutput);
	//Step CV_erode0:
	//input
//	Mat cvErodeSrc = hsvThresholdOutput;
	Mat cvErodeKernel; //Structure of Erosionfigure default 3x3 Pixel Square
	Point cvErodeAnchor(-1, -1); //Sets Anchor into center of Erosionfigure
	double cvErodeIterations = 1.0; //default double
	int cvErodeBordertype = BORDER_CONSTANT;
	Scalar cvErodeBordervalue(-1);
//	cvErode(cvErodeSrc, cvErodeKernel, cvErodeAnchor, cvErodeIterations, cvErodeBordertype, cvErodeBordervalue, this->cvErodeOutput);
	cvErode(hsvThresholdOutput, cvErodeKernel, cvErodeAnchor, cvErodeIterations, cvErodeBordertype, cvErodeBordervalue, this->cvErodeOutput);
	//Step Find_Contours0:
	//input
//	Mat findContoursInput = cvErodeOutput;
	bool findContoursExternalOnly = false; //default Boolean
//	findContours(findContoursInput, findContoursExternalOnly, this->findContoursOutput);
	findContours(cvErodeOutput, findContoursExternalOnly, this->findContoursOutput);	
	//Step Filter_Contours
	//Input
//	vector<vector<Point> > filterContoursContours = findContoursOutput;
	double filterContoursMinArea = 1000.0;
	double filterContoursMinPerimeter = 0.0;
	double filterContoursMinWidth = 20.0;
	double filterContoursMaxWidth = 150.0;
	double filterContoursMinHeight = 50.0;
	double filterContoursMaxHeight = 250.0;
	double filterContoursSolidity[] = {0, 100};
	double filterContoursMaxVertices = 1000000.0;
	double filterContoursMinVersices = 0.0;
	double filterContoursMinRatio = 0.0;
	double filterContoursMaxRatio = 10.0;
//	filterContours(filterContoursContours, filterContoursMinArea, filterContoursMinPerimeter, filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight, filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices, filterContoursMinVersices, filterContoursMinRatio, filterContoursMaxRatio, this->filterContoursOutput);
	filterContours(findContoursOutput, filterContoursMinArea, filterContoursMinPerimeter, filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight, filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices, filterContoursMinVersices, filterContoursMinRatio, filterContoursMaxRatio, this->filterContoursOutput);
	
	end = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
	cout << "Process Runtine: " << end - start << endl;
}

	/**
	 * This method is a generated getter for the output of a CV_resize.
	 * @return Mat output from CV_resize.
	 */
	Mat* FridoProcess::GetCvResizeOutput(){
		return &(this->cvResizeOutput);
	}
	/**
	 * This method is a generated getter for the output of a CV_medianBlur.
	 * @return Mat output from CV_medianBlur.
	 */
	Mat* FridoProcess::GetCvMedianblurOutput(){
		return &(this->cvMedianblurOutput);
	}
	/**
	 * This method is a generated getter for the output of a HSV_Threshold.
	 * @return Mat output from HSV_Threshold.
	 */
	Mat* FridoProcess::GetHsvThresholdOutput(){
		return &(this->hsvThresholdOutput);
	}
	/**
	 * This method is a generated getter for the output of a CV_erode.
	 * @return Mat output from CV_erode.
	 */
	Mat* FridoProcess::GetCvErodeOutput(){
		return &(this->cvErodeOutput);
	}
	/**
	 * This method is a generated getter for the output of a Find_Contorus.
	 * @return Mat output from Find_Contours.
	 */
	vector<vector<Point> >* FridoProcess::GetFindContoursOutput(){
		return &(this->findContoursOutput);
	}
	/**
	 * This method is a generated getter for the output of a Filter_Contorus.
	 * @return Mat output from Filter_Contours.
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
	void FridoProcess::filterContours(vector<vector<Point> > &inputContours, double minArea, double minPerimeter, double minWidth, double maxWidth, double minHeight, double maxHeight, double solidity[], double maxVertexCount, double minVertexCount, double minRatio, double maxRatio, vector<vector<Point> > &output) {
		vector<Point> hull;
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
/**			if ( arcLength(contour, true) < minPerimeter) continue;
			//calculate points of a perimeter which has no concave curves from the input contour and saves it in the hull vector
			convexHull(Mat(contour, true), hull); 
			// the more concave parts the raw contour has, the lower is the solid value
			double solid = 100 * area / contourArea(hull);
			if (solid < solidity[0] || solid > solidity[1]) continue;
			//Option to filter the contour by count of peaks
			if (contour.size() < minVertexCount || contour.size() > maxVertexCount) continue;
			double ratio = (double) bb.width / (double) bb.height;
			if (ratio < minRatio || ratio > maxRatio) continue;
**/			//Filters the contour if there are less then 4 or more then 5 corners
			if (approx.size() < 4 || approx.size() > 6) continue;
			//pushes the contour back to the output if the contour didnt get kicked out because of an invalid condition
			output.push_back(contour);

		}
	}
} // end frido namespace

