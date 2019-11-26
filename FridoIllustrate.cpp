#include "FridoPipeline.h"
#include <chrono>

using namespace cv;
using namespace std;

namespace frido {

    /* time measurement variables */
    double startIllustrate;
    double endIllustrate;

    /* colors */
    Scalar rectColor(0, 255, 0);
    Scalar circleColor(255, 255, 150);
    Scalar contourColor(0, 255, 255);
    Scalar zeroColor(0, 0, 255);
    Scalar white_color(255, 255, 255);

    Point zero;

    /* Creates the empty imgages */
    Mat entireFrame;
    Mat contoursFrame;

    /* Variables for the Entirely Image */
    vector<vector<Point> > box_points;
    vector<Point> height_points(4);
    vector<Point> contours(5);
    Point left_points[4];
    Point right_points[4];

    /* Variables for the Contours Image */
    vector<vector<Point> > filteredContours;

    /* constructor sets the predefined variables */
    FridoIllustrate::FridoIllustrate() {
        startIllustrate = 0;
        endIllustrate = 0;

        zero.x = 320;
        zero.y = 240;

        contoursFrame.setTo(0);;

        box_points.resize(2, vector<Point>(4));
    }

    /* Function is called by the main class and handles the start of the two painting methods */
    void FridoIllustrate::Illustrate(FridoProcess* myprocess, FridoCalculation* mycalc, bool entirely, bool contours) {

        /* writest the system time into the start variable */
        startIllustrate = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

        /* Raw Image after resize with the calculate lines and colored Targets on it */
        if (entirely == true && myprocess->GetFilterContoursOutput()->size() == 2) {
            drawEntire(myprocess, mycalc);
        } else {
            /* returns the resized frame if the statement isnt correct */
            entireFrame = *myprocess->GetCvResizeOutput();
        }

        /* draws the contours from the filterContours method on a black image */
        if (contours == true) {
            drawContours(myprocess);
        }

        /* writes the system time into the end variable and compares the time variables to calculate the time used in the Illustrate class */
        endIllustrate = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        cout << "Illustrate Runtine: " << startIllustrate - endIllustrate << endl;
    }

    /**
     * This method returns the entireFrame output as a pointer.
     * @return Mat entireFrame from drawEntire.
     */
    Mat* FridoIllustrate::GetEntireFrame() {
        return &(entireFrame);
    }

    /**
     * This method returns the contoursFrame output as a pointer.
     * @return Mat contoursFrame from drawContours.
     */
    Mat* FridoIllustrate::GetContoursFrame() {
        return &(contoursFrame);
    }

    /* draws the calculated points and heights as well as the lines on the resized frame */
    void FridoIllustrate::drawEntire(FridoProcess* myprocess, FridoCalculation* mycalc) {
        /* Gets the outputs from the calculate and the process function to draw the lines and Points */
        entireFrame = *myprocess->GetCvResizeOutput();
        contours = *mycalc->GetCalculatePointsOutput();
        box_points = *mycalc->GetSortCornersOutput();
        height_points = *mycalc->GetCalculateOuterHeightPointsOutput();

        /* needed to fill the rectangle, only arrays possible and no vectors */
        for (int i = 0; i < 4; i++) {
            left_points[i] = box_points[0][i];
            right_points[i] = box_points[1][i];
        }
        /* Draws the rectangles on the targets */
        fillConvexPoly(entireFrame, left_points, 4, rectColor);
        fillConvexPoly(entireFrame, right_points, 4, rectColor);

        /* Draws a point on the middle of the camerainput */
        circle(entireFrame, zero, 5, zeroColor, 3);

        /* Draws all the corners and the essential Points between the contours */
        for (int i = 0; i < 4; i++) {
            circle(entireFrame, box_points[0][i], 5, circleColor, 3);
            circle(entireFrame, box_points[1][i], 5, circleColor, 3);
            circle(entireFrame, contours[i], 5, contourColor, 3);
            circle(entireFrame, height_points[i], 5, circleColor, 3);
        }

        /* Draws the 5th point (the for loop has just 4 iterations because of the boxpoints so I did the last point separate) */
        circle(entireFrame, contours[4], 5, contourColor, 3);

        /* Draws the connectorlines between the targets */
        line(entireFrame, box_points[0][0], box_points[1][0], zeroColor, 3);
        line(entireFrame, box_points[0][2], box_points[1][2], zeroColor, 3);

    }

    /* draws the contours on a black image */
    void FridoIllustrate::drawContours(FridoProcess* myprocess) {
        /* Gets the outputs from the process function to draw the contours */
        filteredContours = *myprocess->GetFilterContoursOutput();

        /* method to draw all the contours from the filtered contours (-1 = all) */
        cv::drawContours(contoursFrame, filteredContours, -1, white_color, 3);
    }

}