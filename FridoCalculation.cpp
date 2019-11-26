#include "FridoPipeline.h"
#include <chrono>

#define PI 3.14159265
#define ZEROX 320
#define ZEROY 240
#define TARGET_HEIGHT 13.5

#define multiplier 8410.4
#define exponent 0.985
#define shift 0

#define distanceBetweenTargets 33.5

using namespace cv;
using namespace std;

namespace frido {

    /* time measurement variables */
    double startCalculation;
    double endCalculation;

	/* constructor sets the time variables to zero to avoid old storage values */
    FridoCalculation::FridoCalculation() {
        startCalculation = 0;
	    endCalculation = 0;
    }

    /* Function Calculate to do the whole calculation in a separate file. This Function calls all the steps in the right order. */
    void FridoCalculation::Calculate(vector<vector<Point> >& convertedContours) {

		/* writest the system time into the start variable */
        startCalculation = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();

        //sortContours:
        vector<Point> space_holder_contour; //contour to temporarily save a contours while reordering them
        sortContours(convertedContours, space_holder_contour, this->sortedContoursOutput);

        //MinAreaRect and Point Convert
        findMinAreaRect(sortedContoursOutput, this->findMinAreaRectOutput);

        //Sort Box Points:
        sortCorners(findMinAreaRectOutput, this->sortCornersOutput);

        //calculate Points:
        calculatePoints(sortCornersOutput, this->calculatePointsOutput);

        //calculate heights
        calculateHeights(calculatePointsOutput, sortCornersOutput, this->calculateHeightsOutput,  this->calculateOuterHeightPointsOutput);

        //calculate distance
        calculateDistance(calculateHeightsOutput, this->calculateDistanceOutput);
        
        //calculate angle
        calculateAngle(calculateHeightsOutput, this->calculateAngleOutput);

        //prepare Networktables
        Point zero; //center of the image
        zero.x = ZEROX;
        zero.y = ZEROY;
        prepareNetworkTables(calculateDistanceOutput, calculateAngleOutput, calculatePointsOutput, calculateHeightsOutput, zero, this->prepareNetworkTablesOutput);

		/* writes the system time into the end variable and compares the time variables to calculate the time used in the Calculation class */
        endCalculation = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
	    cout << "Calculation Runtine: " << endCalculation - startCalculation << endl;
    }

    /**
	 * This method returns the sortedContoursOutput output as a pointer.
	 * @return vector<vector<Point> > output from sortContours.
	 */
    vector<vector<Point> >* FridoCalculation::GetSortedContoursOutput() {
        return &(this->sortedContoursOutput);
    }
	/**
	 * This method returns the findMinAreaRectOutput output as a pointer.
	 * @return vector<vector<Point> > output from minAreaRect.
	 */    
    vector<vector<Point> >* FridoCalculation::GetFindMinAreaRectOutput() {
        return &(this->findMinAreaRectOutput);
    }
	/**
	 * This method returns the sortCornersOutput output as a pointer.
	 * @return vector<vector<Point> > output from sortCorners.
	 */    
    vector<vector<Point> >* FridoCalculation::GetSortCornersOutput() {
        return &(this->sortCornersOutput);
    }
	/**
	 * This method returns the calculatePointsOutput output as a pointer.
	 * @return vector<Point> output from calculatePoints.
	 */    
    vector<Point>* FridoCalculation::GetCalculatePointsOutput() {
        return &(this->calculatePointsOutput);
    }
	/**
	 * This method returns the calculatedHeightsOutput output as a pointer.
	 * @return vector<double> output from calculateHeights.
	 */    
    vector<double>* FridoCalculation::GetCalculateHeightsOutput() {
        return &(this->calculateHeightsOutput);
    }
	/**
	 * This method returns the calculateOuterHeightPointsOutput output as a pointer.
	 * @return vector<Point> output from calculateHeights.
	 */    
    vector<Point>* FridoCalculation::GetCalculateOuterHeightPointsOutput() {
        return &(this->calculateOuterHeightPointsOutput);
    }
	/**
	 * This method returns the calculateDistanceOutput output as a pointer.
	 * @return double output from calculateDistance.
	 */    
    double* FridoCalculation::GetCalculateDistanceOutput() {
        return &(this->calculateDistanceOutput);
    }
	/**
	 * This method returns the calculateAngleOutput output as a pointer.
	 * @return vector<double> output from calculateAngle.
	 */    
    vector<double>* FridoCalculation::GetCalculateAngleOutput() {
        return &(this->calculateAngleOutput);
    }
	/**
	 * This method returns the prepareNetworkTablesOutput output as a pointer.
	 * @return vector<double> output from prepareNetworkTables.
	 */    
    vector<double>* FridoCalculation::GetPrepareNetworkTablesOutput() {
        return &(this->prepareNetworkTablesOutput);
    }

    /* The sortContours function brings the contours in the right order. Left contours has to be on array slot 0, right Contour on slot 1 
     * @param sortedContoursInput input form the filtered contours output
     * @param space_holder_contour contour to temporarily save the contours while they are getting sorted
     * @param output sorted contours in a vector
    */
    void FridoCalculation::sortContours(vector<vector<Point> > &sortedContoursInput, vector<Point> &space_holder_contour, vector<vector<Point> > &output) {
        output.clear();
        /* changes the order of the contours by comparing a random point because every point from the left contour should have a lower x-value */
        if (sortedContoursInput[0][0].x > sortedContoursInput[1][0].x) {
                space_holder_contour = sortedContoursInput[0];
                sortedContoursInput[0] = sortedContoursInput[1];
                sortedContoursInput[1] = space_holder_contour;
        }
        output = sortedContoursInput;
    }

    /* Function findMinAreaRect searches for the rectangle in the contours and returns the corner Points of the contours in an arrray
     * @param targets the sorted contours to find the min area rect
     * @param output the points of the corners of the min area rect
    */
    void FridoCalculation::findMinAreaRect(vector<vector<Point> > &targets, vector<vector<Point> > &output) {
        output.clear();
        RotatedRect minAreaTarget;
        Point2f box_2f[4];
        vector<vector<Point> > out;
        out.resize(2, vector<Point>(4));
        int targetNR = 0;

        /* iterates through the two contours and saves the corner points of the min area rect */
        for(vector<Point> target : targets) {
            minAreaTarget = minAreaRect(target);
            minAreaTarget.points(box_2f);
            /* converts the 2f points into Points */
            for(int i = 0; i < 4; i++) {
                out[targetNR][i] = box_2f[i];
            }
            targetNR++;
        }
        output = out;
    }

    /* Function which sorts the Corners of the Contours in the Array for easy localisation 
     * @param boxes the corners of the min area rect
     * @param output the corners in the right order left target: (topleft, topright, bottemleft, bottemright) -> mirrored for the right target
    */
    void FridoCalculation::sortCorners(vector<vector<Point> > &boxes, vector<vector<Point> > &output) {
        output.clear();
        int yValueSort[4];
        Point box_sorted[4];
        Point space_holder_point;
        vector<vector<Point> > out;
        out.resize(2, vector<Point>(4));
        int targetNR = 0;
        for (vector<Point> box : boxes) {
            /* Write all y Values of a Contourbox in an Array */
            for (int i = 0; i < 4; i++) {
                yValueSort[i] = box[i].y;
            }

            /* Sort the Array by Value */
            sort(yValueSort, yValueSort + 4);

            /* Rewrite positions in the original Array with the sorted Array */
            for (int i = 0; i < 4; i++) {
                for (int a = 0; a < 4; a++) {
                    if (yValueSort[i] == box[a].y) {
                        box_sorted[i] = box[a];
                        break;
                    }
                }
            }

            /* Sorts the Boxpoints by X Value in position 1 + 2 and in position 3 + 4 */
            for (int i = 0; i > 4; i += 2) {
                if(box_sorted[i].x < box_sorted[i + 1].x) {
                    space_holder_point = box_sorted[i];
                    box_sorted[i] = box_sorted[i + 1];
                    box_sorted[i + 1] = space_holder_point;
                }
            }

            for (int i = 0; i < 4; i++) {
                out[targetNR][i] = box_sorted[i];
            }

            targetNR++;
        }

        output = out;
    }

    /* Here comes the calculation of all the important Points needed for the calculation
     * @param targetPoints the corners of the contours in a sorted vector
     * @param output the important points in the image
    */
    void FridoCalculation::calculatePoints(vector<vector<Point> > &targetPoints, vector<Point> &output) {
        output.clear();
        vector<Point> out(5);
        /* output[leftMid, rightMid, upperCenterX, lowerCenterX, centerTarget] */
        out[0].x = targetPoints[0][0].x + (targetPoints[0][3].x - targetPoints[0][0].x) / 2;  
        out[0].y = (targetPoints[0][0].y + targetPoints[0][1].y + targetPoints[0][2].y + targetPoints[0][3].y) / 4;
        out[1].x = targetPoints[1][0].x + (targetPoints[1][3].x - targetPoints[1][0].x) / 2;
        out[1].y = (targetPoints[1][0].y + targetPoints[1][1].y + targetPoints[1][2].y + targetPoints[1][3].y) / 4;
        out[2] = targetPoints[0][0] + (targetPoints[1][0] - targetPoints[0][0]) / 2;
        out[3] = targetPoints[0][2] + (targetPoints[1][2] - targetPoints[0][2]) / 2;
        out[4].y = out[2].y + (out[3].y - out[2].y) / 2;
        out[4].x = out[2].x + (out[3].x - out[2].x) / 2;

        output = out;

    }

    /* With the Values of the Points you can calculate the height of each targets and the value of the average height
     * @param points the important points calculated before
     * @param corners the sorted corners of the targets
     * @param output the calculated heights
     * @param outputPoint the four points for the solo target heights
    */
    void FridoCalculation::calculateHeights(vector<Point> &points, vector<vector<Point> > &corners, vector<double> &output, vector<Point> &outputPoint) {
        vector<double> out(3);
        vector<Point> outputPoints(4);

        /* the average height calculated with difference between the y-values of the point zero in the target */
        double average_height = points[3].y - points[2].y;

        /* delta x and delta y for the gradient triangle to calculate the gradient of the linear equation */
        double height_difference_one = corners[1][0].y - corners[0][0].y;
        double lenght_difference_one = corners[1][0].x - corners[0][0].x;

        double height_difference_two = corners[1][2].y - corners[0][2].y;
        double lenght_difference_two = corners[1][2].x - corners[0][2].x;

        /* calculate the gradient */
        double m_one = height_difference_one / lenght_difference_one;
        double m_two = height_difference_two / lenght_difference_two;

        /* y axis intercept calculation with the equation: y = m*x + q */
        double q_one = corners[0][0].y - m_one * corners[0][0].x;
        double q_two = corners[0][2].y - m_two * corners[0][2].x;

        /* calculation of the points on the calculated equations for the outer heights */
        Point upper_left;
        upper_left.x = corners[0][2].x + (corners[0][0].x - corners[0][2].x) / 2;
        upper_left.y = m_one * upper_left.x + q_one;

        Point upper_right;
        upper_right.x = corners[1][0].x + (corners[1][2].x - corners[1][0].x) / 2;
        upper_right.y = m_one * upper_right.x + q_one;

        Point lower_left;
        lower_left.x = corners[0][2].x + (corners[0][0].x - corners[0][2].x) / 2;
        lower_left.y = m_two * lower_left.x + q_two;

        Point lower_right;
        lower_right.x = corners[1][0].x + (corners[1][2].x - corners[1][0].x) / 2;
        lower_right.y = m_two * lower_right.x + q_two;

        /* calculation of the outer heights with the points */
        double height_left = lower_left.y - upper_left.y;
        double height_right = lower_right.y - upper_right.y;

        /* generates output */
        out[0] = average_height;
        out[1] = height_left;
        out[2] = height_right;
        output = out;

        /* generates outputPoint */
        outputPoints[0] = upper_left;
        outputPoints[1] = lower_left;
        outputPoints[2] = upper_right;
        outputPoints[3] = lower_right;
        outputPoint = outputPoints;

    }

    /* last thing to do is to calculate the distance of the Robot to the target. we can do that with a formula: offset + (a / (averageHeight / b) ^ 2)
     * @param heights the average height to calculate the distance
     * @param output the distane of the center of the target
    */
    void FridoCalculation::calculateDistance(vector<double> &heights, double &output) {
        /* the formula is y = 1 / x with some more detailed measured values */
        double distance = multiplier / pow((heights[0] + shift), exponent);
        output = distance;
    }

    /* to calculate the angle of the robot in relation to the target we have to calculate the difference between the distance from the left and the right target.
     * beacause of the right-angled triangle with the 33.5 cm physical distance between the two targets on the wall and the difference of the distance
     * you can calculate the angle with the arcus sinus and convert it into degrees with * 180 / PI;
     * @param heights the outer heights
     * @param output the angle and the absolut angle
    */
    void FridoCalculation::calculateAngle(vector<double> &heights, vector<double> &output) {
        vector<double> out(2);

        /* calculates the distances to the targets */
        double distanceLeftTarget = pow(multiplier / heights[1], 1.0 / exponent);
        double distanceRightTarget = pow(multiplier / heights[2], 1.0 / exponent);

        /* calculates the opposite side of the angle */
        double angleGegakt = distanceLeftTarget - distanceRightTarget;

        /* calculate angle with the arcus sinus */
        double angle = asin(angleGegakt / distanceBetweenTargets) * 180 / PI;

        double absAngle = abs(angle);
        out[0] = angle;
        out[1] = absAngle;
        output = out;

    }

    /* Makes a Vector with all the important values for the NetworkTables */
    void FridoCalculation::prepareNetworkTables(double &distance, vector<double> &angle, vector<Point> &points, vector<double> &heights, Point &zero, vector<double> &output) {
        vector<double> out(4);
        
        out[0] = distance; //distance of Camera to Target
        out[1] = angle[0]; //Angle of Camera to Target
        out[2] = (points[4].x - zero.x) / heights[0] * TARGET_HEIGHT; //X Offset to zeroPoint
        out[3] = (points[4].y - zero.y) / heights[0] * TARGET_HEIGHT; //Y Offset to zeroPoint

        output = out;

    }

}