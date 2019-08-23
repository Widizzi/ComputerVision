#include "FridoPipeline.h"

using namespace cv;
using namespace std;


namespace frido {

FridoPipeline::FridoPipeline() {
}
/**
* Runs an iteration of the pipeline and updates outputs.
*/
void FridoPipeline::Process(Mat& source0){
	//Step CV_resize0:
	//input
	Mat cvResizeSrc = source0;
	Size cvResizeDsize(0, 0);
	double cvResizeFx = 0.5;  // default Double
	double cvResizeFy = 0.5;  // default Double
    	int cvResizeInterpolation = INTER_LINEAR;
	cvResize(cvResizeSrc, cvResizeDsize, cvResizeFx, cvResizeFy, cvResizeInterpolation, this->cvResizeOutput);
	//Step CV_Medianblur0;
	//input
	Mat cvMedianblurSrc = cvResizeOutput;
	double cvMedianblurKsize = 5.0;
	cvMedianblur(cvMedianblurSrc, cvMedianblurKsize, this->cvMedianblurOutput);
	//Step HSV_Threshold0:
	//input
	Mat hsvThresholdInput = cvMedianblurOutput;
	double hsvThresholdHue[] = {0.0, 180.0};
	double hsvThresholdSaturation[] = {0.0, 15.0};
	double hsvThresholdValue[] = {215.0, 255.0};
	hsvThreshold(hsvThresholdInput, hsvThresholdHue, hsvThresholdSaturation, hsvThresholdValue, this->hsvThresholdOutput);
	//Step CV_erode0:
	//input
	Mat cvErodeSrc = hsvThresholdOutput;
	Mat cvErodeKernel;
	Point cvErodeAnchor(-1, -1);
	double cvErodeIterations = 1.0; //default double
	int cvErodeBordertype = BORDER_CONSTANT;
	Scalar cvErodeBordervalue(-1);
	cvErode(cvErodeSrc, cvErodeKernel, cvErodeAnchor, cvErodeIterations, cvErodeBordertype, cvErodeBordervalue, this->cvErodeOutput);
	//Step Find_Contours0:
	//input
	Mat findContoursInput = cvErodeOutput;
	bool findContoursExternalOnly = false; //default Boolean
	findContours(findContoursInput, findContoursExternalOnly, this->findContoursOutput);	
	//Step Filter_Contours
	//Input
	vector<vector<Point> > filterContoursContours = findContoursOutput;
	double filterContoursMinArea = 0.0;
	double filterContoursMinPerimeter = 0.0;
	double filterContoursMinWidth = 10.0;
	double filterContoursMaxWidth = 100.0;
	double filterContoursMinHeight = 30.0;
	double filterContoursMaxHeight = 100.0;
	double filterContoursSolidity[] = {0, 100};
	double filterContoursMaxVertices = 1000000.0;
	double filterContoursMinVersices = 0.0;
	double filterContoursMinRatio = 0.0;
	double filterContoursMaxRatio = 1000.0;
	filterContours(filterContoursContours, filterContoursMinArea, filterContoursMinPerimeter, filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight, filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices, filterContoursMinVersices, filterContoursMinRatio, filterContoursMaxRatio, this->filterContoursOutput);


}

	/**
	 * This method is a generated getter for the output of a CV_resize.
	 * @return Mat output from CV_resize.
	 */
	Mat* FridoPipeline::GetCvResizeOutput(){
		return &(this->cvResizeOutput);
	}
	/**
	 * This method is a generated getter for the output of a CV_medianBlur.
	 * @return Mat output from CV_medianBlur.
	 */
	Mat* FridoPipeline::GetCvMedianblurOutput(){
		return &(this->cvMedianblurOutput);
	}
	/**
	 * This method is a generated getter for the output of a HSV_Threshold.
	 * @return Mat output from HSV_Threshold.
	 */
	Mat* FridoPipeline::GetHsvThresholdOutput(){
		return &(this->hsvThresholdOutput);
	}
	/**
	 * This method is a generated getter for the output of a CV_erode.
	 * @return Mat output from CV_erode.
	 */
	Mat* FridoPipeline::GetCvErodeOutput(){
		return &(this->cvErodeOutput);
	}
	/**
	 * This method is a generated getter for the output of a Find_Contorus.
	 * @return Mat output from Find_Contours.
	 */
	vector<vector<Point> >* FridoPipeline::GetFindContoursOutput(){
		return &(this->findContoursOutput);
	}
	/**
	 * This method is a generated getter for the output of a Filter_Contorus.
	 * @return Mat output from Filter_Contours.
	 */
	vector<vector<Point> >* FridoPipeline::GetFilterContoursOutput(){
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
	void FridoPipeline::cvResize(Mat &src, Size &dSize, double fx, double fy, int interpolation, Mat &dst) {
		resize(src, dst, dSize, fx, fy, interpolation);
	}

	/**
	 "
	 */
	void FridoPipeline::cvMedianblur(Mat &src, double kSize, Mat &dst) {
		medianBlur(src, dst, int(kSize));
	}

	/**
	 * Segment an image based on hue, saturation, and value ranges.
	 *
	 * @param input The image on which to perform the HSL threshold.
	 * @param hue The min and max hue.
	 * @param sat The min and max saturation.
	 * @param val The min and max value.
	 * @param output The image in which to store the output.
	 */
	void FridoPipeline::hsvThreshold(Mat &input, double hue[], double sat[], double val[], Mat &out) {
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
	void FridoPipeline::cvErode(Mat &src, Mat &kernel, Point &anchor, double iterations, int borderType, Scalar &borderValue, Mat &dst) {
		erode(src, dst, kernel, anchor, (int)iterations, borderType, borderValue);	
	}

	/**
	 * Find Contours in an image..
	 *
	 * @param input The image to find contours in.
	 * @param externalOnly if only external contours are to be found.
	 * @param contours vector of contours to put contours in.
	*/	
	void FridoPipeline::findContours(Mat &input, bool externalOnly, vector<vector<Point> > &contours) {
		vector<Vec4i> hierarchy;
		contours.clear();
		int mode = externalOnly ? RETR_EXTERNAL : RETR_LIST;
		int method = CHAIN_APPROX_SIMPLE;
		cv::findContours(input, contours, hierarchy, mode, method); 
	}

	/**
	 * Filters through contours.
	*/
	void FridoPipeline::filterContours(vector<vector<Point> > &inputContours, double minArea, double minPerimeter, double minWidth, double maxWidth, double minHeight, double maxHeight, double solidity[], double maxVertexCount, double minVertexCount, double minRatio, double maxRatio, vector<vector<Point> > &output) {
		vector<Point> hull;
		vector<Point> approx;
		output.clear();
		for(vector<Point> contour: inputContours) {
			Rect bb = boundingRect(contour);
			approxPolyDP(Mat(contour), approx, arcLength(Mat(contour), true)*0.02, true);
			if (bb.width < minWidth || bb.width > maxWidth) continue;
			if (bb.height < minHeight ||bb.height > maxHeight) continue;
			double area = contourArea(contour);
			if (area < minArea) continue;
			if ( arcLength(contour, true) < minPerimeter) continue;
			convexHull(Mat(contour, true), hull);
			double solid = 100 * area / contourArea(hull);
			if (solid < solidity[0] || solid > solidity[1]) continue;
			if (contour.size() < minVertexCount || contour.size() > maxVertexCount) continue;
			double ratio = (double) bb.width / (double) bb.height;
			if (ratio < minRatio || ratio > maxRatio) continue;
			if (approx.size() != 4) continue;
			output.push_back(contour);

		}
	}
} // end frido namespace

