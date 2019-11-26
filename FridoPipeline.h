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

/* namespace frido */
namespace frido {

class FridoProcess {
	/* private variables and methods to call in the FridoProcess class */
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
		void filterContours(vector<vector<Point> > &, double, double, double, double, double, vector<vector<Point> > &);	
	/* public getter pointers and the Process method */
	public:
		FridoProcess();
		void Process(Mat& source0);
		Mat* GetCvResizeOutput();
		Mat* GetCvMedianblurOutput();
		Mat* GetHsvThresholdOutput();
		Mat* GetCvErodeOutput();
		vector<vector<Point> >* GetFindContoursOutput();
		vector<vector<Point> >* GetFilterContoursOutput();	

};


class FridoCalculation {
	/* private variables and methods to call in the FridoCalculation class */
	private:
		vector<vector<Point> > sortedContoursOutput;
		vector<vector<Point> > findMinAreaRectOutput;
		vector<vector<Point> > sortCornersOutput;
		vector<Point> calculatePointsOutput;
		vector<double> calculateHeightsOutput;
		vector<Point> calculateOuterHeightPointsOutput;
		vector<double>  calculateAngleOutput;
		double calculateDistanceOutput;
		vector<double> prepareNetworkTablesOutput;
		void sortContours(vector<vector<Point> > &, vector<Point> &, vector<vector<Point> > &);
		void findMinAreaRect(vector<vector<Point> > &, vector<vector<Point> > &);
		void sortCorners(vector<vector<Point> > &, vector<vector<Point> > &);
		void calculatePoints(vector<vector<Point> > &, vector<Point> &);
		void calculateHeights(vector<Point> &, vector<vector<Point> > &, vector<double> &, vector<Point> &);
		void calculateAngle(vector<double> &, vector<double> &);
		void calculateDistance(vector<double> &, double &);
		void prepareNetworkTables(double &, vector<double> &, vector<Point> &, vector<double> &, Point &, vector<double> &);
	/* public getter pointers and the Calculate method */
	public:
		FridoCalculation();
		void Calculate(vector<vector<Point> >& convertedContoursInput);
		vector<vector<Point> >* GetCheckedContoursOutput(); 
		vector<vector<Point> >* GetSortedContoursOutput();
		vector<vector<Point> >* GetFindMinAreaRectOutput();
		vector<vector<Point> >* GetSortCornersOutput();
		vector<Point>* GetCalculatePointsOutput();
		vector<double>* GetCalculateHeightsOutput();
		vector<Point>* GetCalculateOuterHeightPointsOutput();
		vector<double>* GetCalculateAngleOutput();
		double* GetCalculateDistanceOutput();
		vector<double>* GetPrepareNetworkTablesOutput();
};


class FridoIllustrate {
	/* private variables and methods to call in the FridoIllustrate class */
	private:
		void drawEntire(FridoProcess*, FridoCalculation*);
		void drawContours(FridoProcess*);
	/* public getter pointers and the Process method */
	public:
		FridoIllustrate();
		void Illustrate(FridoProcess* myprocess, FridoCalculation* mycalc, bool, bool);
		Mat* GetEntireFrame();
		Mat* GetContoursFrame();
};


}

