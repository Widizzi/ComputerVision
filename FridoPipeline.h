#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <math.h>

using namespace cv;
using namespace std;

namespace frido {

class FridoPipeline {
	private:
		Mat cvResizeOutput;
		Mat cvMedianblurOutput;
		Mat hsvThresholdOutput;
		Mat cvErodeOutput;
		vector<vector<Point> > findContoursOutput;
		vector<vector<Point> > filterContoursOutput;
		void cvResize(Mat &, Size &, double , double , int , Mat &);
		void cvMedianblur(Mat &, double, Mat&);
		void hsvThreshold(Mat &, double [], double [], double [], Mat &);
		void cvErode(Mat &, Mat &, Point &, double, int, Scalar &, Mat &);
		void findContours(Mat &, bool, vector<vector<Point> > &);
		void filterContours(vector<vector<Point> > &, double, double, double, double, double, double, double [], double, double, double, double, vector<vector<Point> > &);	
	public:
		FridoPipeline();
		void Process(Mat& source0);
		Mat* GetCvResizeOutput();
		Mat* GetCvMedianblurOutput();
		Mat* GetHsvThresholdOutput();
		Mat* GetCvErodeOutput();
		vector<vector<Point> >* GetFindContoursOutput();
		vector<vector<Point> >* GetFilterContoursOutput();	

};


class FridoCalculation {
	private:
		vector<vector<Point> > checkedContoursOutput;
		vector<vector<Point> > sortedContoursOutput;
		vector<vector<Point> > findMinAreaRectOutput;
		vector<vector<Point> > sortCornersOutput;
		vector<Point> calculatePointsOutput;
		void checkContours(vector<vector<Point> > &, vector<vector<Point> > &);
		void sortContours(vector<vector<Point> > &, vector<Point> &, vector<vector<Point> > &);
		void findMinAreaRect(vector<vector<Point> > &, vector<vector<Point> > &);
		void sortCorners(vector<vector<Point> > &, vector<vector<Point> > &);
		void calculatePoints(vector<vector<Point> > &, vector<Point> &);
	public:
		FridoCalculation();
		void Calculate(vector<vector<Point> >& convertedContoursInput);
		vector<vector<Point> >* GetCheckedContoursOutput(); 
		vector<vector<Point> >* GetSortedContoursOutput();
		vector<vector<Point> >* GetFindMinAreaRectOutput();
		vector<vector<Point> >* GetSortCornersOutput();
		vector<Point>* GetCalculatePointsOutput();
};


}

