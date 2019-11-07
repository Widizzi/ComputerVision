#include "FridoPipeline.h"
#include <chrono>

#define PI 3.14159265
#define ZEROX 320
#define ZEROY 240
#define TARGET_HEIGHT 14

using namespace cv;
using namespace std;

namespace frido {

    double beginn;
    double ende;
    FridoCalculation::FridoCalculation() {
        beginn = 0;
	    ende = 0;
    }

    //Function Calculate to do the whole calculation in a separate file. This Function calls all the steps in the right order.
    void FridoCalculation::Calculate(vector<vector<Point> >& convertedContours) {
        beginn = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        //Step CheckContours:
        //Input
        vector<vector<Point> > checkedContoursInput = convertedContours;
        checkContours(checkedContoursInput, this->checkedContoursOutput);
        //Step SortContours:
        //input
        vector<vector<Point> > sortedContoursInput = checkedContoursOutput;
        vector<Point> space_holder_contour;
        sortContours(sortedContoursInput, space_holder_contour, this->sortedContoursOutput);
        //Step MinAreaRect and Point Convert
        //input
        vector<vector<Point> > findMinAreaRectInput = sortedContoursOutput;
        findMinAreaRect(findMinAreaRectInput, this->findMinAreaRectOutput);
        //Step Sort Box Points:
        //input
        vector<vector<Point> > sortCornersInput = findMinAreaRectOutput;
        sortCorners(sortCornersInput, this->sortCornersOutput);
        //Step calculate Points:
        //input
        vector<vector<Point> > calculatePointsInput = sortCornersOutput;
        calculatePoints(calculatePointsInput, this->calculatePointsOutput);
        //Step calculate heights
        //input
        vector<Point> calculateAverageHeightInput = calculatePointsOutput;
        vector<vector<Point> > calculateTargetHeightInput = sortCornersOutput;
        calculateHeights(calculateAverageHeightInput, calculateTargetHeightInput, this->calculateHeightsOutput);
        //Step calculate distance
        //input
        vector<double> calculateDistanceInput = calculateHeightsOutput;
        double multiplier = 7087.0;
        double sqrt = 0.941;
        calculateDistance(calculateDistanceInput, multiplier, sqrt, this->calculateDistanceOutput);
        //Step calculate angle
        //input
        vector<double> calculateAngleInput = calculateHeightsOutput;
        vector<Point> pointsInput = calculatePointsOutput;
        double distanceInput = calculateDistanceOutput;
        Point zero;
        zero.x = ZEROX;
        zero.y = ZEROY;
        calculateAngle(calculateAngleInput, pointsInput, distanceInput, zero, this->calculateAngleOutput);
        //Step prepare Networktables
        //input
        double ntDistanceInput = calculateDistanceOutput;
        vector<double> ntAngleInput = calculateAngleOutput;
        vector<Point> ntPointsInput = calculatePointsOutput;
        vector<double> ntHeightsInput = calculateHeightsOutput;
//        Point zero;
//        zero.x = ZEROX;
//        zero.y = ZEROY;
        prepareNetworkTables(ntDistanceInput, ntAngleInput, ntPointsInput, ntHeightsInput, zero, this->prepareNetworkTablesOutput);

        ende = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
//	    cout << "Calculation Runtine: " << ende - beginn << endl;
    }

    vector<vector<Point> >* FridoCalculation::GetCheckedContoursOutput() {
        return &(this->checkedContoursOutput);
    }
    vector<vector<Point> >* FridoCalculation::GetSortedContoursOutput() {
        return &(this->sortedContoursOutput);
    }
    vector<vector<Point> >* FridoCalculation::GetFindMinAreaRectOutput() {
        return &(this->findMinAreaRectOutput);
    }
    vector<vector<Point> >* FridoCalculation::GetSortCornersOutput() {
        return &(this->sortCornersOutput);
    }
    vector<Point>* FridoCalculation::GetCalculatePointsOutput() {
        return &(this->calculatePointsOutput);
    }
    vector<double>* FridoCalculation::GetCalculateHeightsOutput() {
        return &(this->calculateHeightsOutput);
    }
    vector<double>* FridoCalculation::GetCalculateAngleOutput() {
        return &(this->calculateAngleOutput);
    }
    double* FridoCalculation::GetCalculateDistanceOutput() {
        return &(this->calculateDistanceOutput);
    }
    vector<double>* FridoCalculation::GetPrepareNetworkTablesOutput() {
        return &(this->prepareNetworkTablesOutput);
    }

    //The checkContours Function checks if there are exactely two contours. Its needed to avoid errors in the calculation with nullpointer.
    void FridoCalculation::checkContours(vector<vector<Point> > &checkedContoursInput, vector<vector<Point> > &output) {
        output.clear();
//        if (checkedContoursInput.size() != 2) {	
//            return;
//        } else {
            output = checkedContoursInput;
//        }
    }

    //The sortContours function brings the contours in the right order. Left contours has to be on array slot 0, right Contour on slot 1
    void FridoCalculation::sortContours(vector<vector<Point> > &sortedContoursInput, vector<Point> &space_holder_contour, vector<vector<Point> > &output) {
        output.clear();
        if (sortedContoursInput[0][0].x > sortedContoursInput[1][0].x) {
                space_holder_contour = sortedContoursInput[0];
                sortedContoursInput[0] = sortedContoursInput[1];
                sortedContoursInput[1] = space_holder_contour;
        }
        output = sortedContoursInput;
    }

    //Function findMinAreaRect searches for the rectangle in the contours and returns the corner Points of the contours in an arrray
    void FridoCalculation::findMinAreaRect(vector<vector<Point> > &targets, vector<vector<Point> > &output) {
        output.clear();
        RotatedRect minAreaTarget;
        Point2f box_2f[4];
        vector<vector<Point> > out;
        out.resize(2, vector<Point>(4));
        int targetNR = 0;

        for(vector<Point> target : targets) {
            minAreaTarget = minAreaRect(target);
            minAreaTarget.points(box_2f);
            for(int i = 0; i < 4; i++) {
                out[targetNR][i] = box_2f[i];
            }
            targetNR++;
        }
        output = out;
    }

    //Function which sorts the Corners of the Contours in the Array for easy localisation
    void FridoCalculation::sortCorners(vector<vector<Point> > &boxes, vector<vector<Point> > &output) {
        output.clear();
        int yValueSort[4];
        Point box_sorted[4];
        Point space_holder_point;
        vector<vector<Point> > out;
        out.resize(2, vector<Point>(4));
        int targetNR = 0;
        for (vector<Point> box : boxes) {
            //Write all y Values of a Contourbox in an Array
            for (int i = 0; i < 4; i++) {
                yValueSort[i] = box[i].y;
            }

            //Sort the Array by Value
            sort(yValueSort, yValueSort + 4);

            //Rewrite positions in the original Array with the sorted Array
            for (int i = 0; i < 4; i++) {
                for (int a = 0; a < 4; a++) {
                    if (yValueSort[i] == box[a].y) {
                        box_sorted[i] = box[a];
                        break;
                    }
                }
            }

            //Sorts the Boxpoints by X Value in position 1 + 2 and in position 3 + 4
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

    //Here comes the calculation of all the important Points needed for the calculation
    void FridoCalculation::calculatePoints(vector<vector<Point> > &targetPoints, vector<Point> &output) {
        output.clear();
        vector<Point> out(5);
        //Output[leftMid, rightMid, upperCenterX, lowerCenterX, centerTarget]
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

    //With the Values of the Points you can calculate the height of each targets and the value of the average height
    void FridoCalculation::calculateHeights(vector<Point> &points, vector<vector<Point> > &corners, vector<double> &output) {
        vector<double> out(3);
        double average_height = points[3].y - points[2].y;
        double height_left = corners[0][2].y - corners[0][0].y;
        double height_right = corners[1][2].y - corners[1][0].y;
        out[0] = average_height;
        out[1] = height_left;
        out[2] = height_right;
        output = out;
    }

    //last thing to do is to calculate the distance of the Robot to the target. we can do that with a formula: offset + (a / (averageHeight / b) ^ 2)
    void FridoCalculation::calculateDistance(vector<double> &heights, double multiplier, double exponent, double &output) {
        double distance = pow(multiplier / heights[0], 1.0 / exponent);
        output = distance;
    }

    //to calculate the angle of the robot in relation to the target we have to calculate the difference between the height of the two targets and use a factor to display it in degrees
    void FridoCalculation::calculateAngle(vector<double> &heights, vector<Point> &points, double distance, Point &zero, vector<double> &output) {
        vector<double> out(2);
        //convertes the offset from pixels to cm. it uses the height of the target (14cm) as a reference
        double leftOffsetInCM = (points[0].x - zero.x) / heights[1] * TARGET_HEIGHT;
        double rightOffsetInCM = (points[1].x - zero.x) / heights[2] * TARGET_HEIGHT;
//        cout << "leftfOffset: " << leftOffsetInCM << endl;
//        cout << "rightOffset: " << rightOffsetInCM << endl;
        double distanceLeftTarget = pow(7087.6 / heights[1], 1.0 / 0.941);
        double distanceRightTarget = pow(7087.6 / heights[2], 1.0 / 0.941);
//        cout << "distacneleftTarget: " << distanceLeftTarget << endl;
//        cout << "distanceRightTarget: " << distanceRightTarget << endl;
        double leftTargetNoInfluence = sqrt( pow(distanceLeftTarget, 2) - pow(leftOffsetInCM, 2) );
        double rightTargetNoInfluence = sqrt( pow(distanceRightTarget, 2) - pow(rightOffsetInCM, 2) );
//        cout << "left no inf: " << leftTargetNoInfluence << endl;
//        cout << "right no inf: " << rightTargetNoInfluence << endl;
        double angleGegakt = distanceLeftTarget - distanceRightTarget;
//        double angleGegakt = leftTargetNoInfluence - rightTargetNoInfluence;
//        cout << "Gegenkat: " << angleGegakt << endl;
        double angleAnkat = abs(leftOffsetInCM - rightOffsetInCM);
//        cout << "ankat: " << angleAnkat << endl;
        double angle = atan(angleGegakt / angleAnkat) *180 / PI;

        double absAngle = abs(angle);
        out[0] = angle;
        out[1] = absAngle;
        output = out;

    }

    //Makes a Vector with all the important values for the NetworkTables that you can read out just this vector
    void FridoCalculation::prepareNetworkTables(double &distance, vector<double> &angle, vector<Point> &points, vector<double> &heights, Point &zero, vector<double> &output) {
        vector<double> out(6);
        
        out[0] = distance; //distance of Camera to Target
        out[1] = angle[0]; //Angle of Camera to Target
        out[2] = sin(angle[1] * PI / 180) * distance; //X Distance from Camera to Target
        out[3] = cos(angle[1] * PI / 180) * distance; //Y Distance from Camera to Target
        out[4] = (points[4].x - zero.x) / heights[0] * TARGET_HEIGHT; //X Offset to zeroPoint
        out[5] = points[4].y - zero.y; //Y Offset to zeroPoint

        output = out;

    }

}