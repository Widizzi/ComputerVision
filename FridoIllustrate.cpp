#include "FridoPipeline.h"
#include <chrono>

using namespace cv;
using namespace std;

namespace frido {

    double anfang;
    double schluss;

    Scalar rectColor(0, 255, 0);
    Scalar circleColor(255, 255, 150);
    Scalar contourColor(0, 255, 255);
    Scalar zeroColor(0, 0, 255);
    Scalar white_color(255, 255, 255);

    Point zero;

    //Creates the empty imgages
    Mat entireFrame;
    Mat contoursFrame;

    //Variables for the Entirely Image
    vector<vector<Point> > box_points;
    vector<Point> height_points(4);
    vector<Point> contours(5);
    Point left_points[4];
    Point right_points[4];

    //Variables for the Contours Image
    vector<vector<Point> > filteredContours;

    FridoIllustrate::FridoIllustrate() {
        anfang = 0;
        schluss = 0;

        zero.x = 320;
        zero.y = 240;

        contoursFrame.setTo(0);;

        box_points.resize(2, vector<Point>(4));

    }

    void FridoIllustrate::Illustrate(FridoProcess* myprocess, FridoCalculation* mycalc, bool entirely, bool contours) {
        anfang = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

        //Raw Image after resize with the calculate lines and colored Targets on it
        if (entirely == true && myprocess->GetFilterContoursOutput()->size() == 2) {
            drawEntire(myprocess, mycalc);
        } else {
            entireFrame = *myprocess->GetCvResizeOutput();
        }
        if (contours == true) {
            drawContours(myprocess);
        }

        schluss = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
//	    cout << "Illustrate Runtine: " << schluss - anfang << endl;
    }

    Mat* FridoIllustrate::GetEntireFrame() {
		return &(entireFrame);
	}

    Mat* FridoIllustrate::GetContoursFrame() {
        return &(contoursFrame);
    }

    void FridoIllustrate::drawEntire(FridoProcess* myprocess, FridoCalculation* mycalc) {
        //Gets the outputs from the calculate and the process function to draw the lines and Points
        entireFrame = *myprocess->GetCvResizeOutput();
        contours = *mycalc->GetCalculatePointsOutput();
        box_points = *mycalc->GetSortCornersOutput();
        height_points = *mycalc->GetCalculateOuterHeightPointsOutput();

        //needed to fill the rectangle, only arrays possible and no vectors
        for (int i = 0; i < 4; i++) {
            left_points[i] = box_points[0][i];
            right_points[i] = box_points[1][i];
        }
        //Draws the rectangles on the targets
        fillConvexPoly(entireFrame, left_points, 4, rectColor);
        fillConvexPoly(entireFrame, right_points, 4, rectColor);
        //Draws a point on the middle of the camerainput
        circle(entireFrame, zero, 5, zeroColor, 3);
        //Draws all the corners and the essential Points between the contours
        for (int i = 0; i < 4; i++) {
            circle(entireFrame, box_points[0][i], 5, circleColor, 3);
            circle(entireFrame, box_points[1][i], 5, circleColor, 3);
            circle(entireFrame, contours[i], 5, contourColor, 3);
            circle(entireFrame, height_points[i], 5, circleColor, 3);
        }
        //Draws the 5th point (the for loop has just 4 iterations because of the boxpoints so I did the last point separate)
        circle(entireFrame, contours[4], 5, contourColor, 3);
        //Draws the connectorlines between the targets
        line(entireFrame, box_points[0][0], box_points[1][0], zeroColor, 3);
        line(entireFrame, box_points[0][2], box_points[1][2], zeroColor, 3);

    }

    void FridoIllustrate::drawContours(FridoProcess* myprocess) {
        //Gets the outputs from the process function to draw the contours
        filteredContours = *myprocess->GetFilterContoursOutput();

	    cv::drawContours(contoursFrame, filteredContours, -1, white_color, 3);
    }




}