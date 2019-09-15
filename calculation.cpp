#include "FridoPipeline.h"

using namespace cv;
using namespace std;

namespace frido {

    FridoCalculation::FridoCalculation() {
    }

    //Function Calculate to do the whole calculation in a separate file. This Function calls all the steps in the right order.
    void FridoCalculation::Calculate(vector<vector<Point> >& convertedContours) {
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
        Point points[4];
        vector<vector<Point> > out;
        out.resize(2, vector<Point>(4));
        int targetNR = 0;

        for(vector<Point> target : targets) {
            minAreaTarget = minAreaRect(target);
            minAreaTarget.points(box_2f);
            for(int i = 0; i < 4; i++) {
                points[i] = box_2f[i];
            }
            for(int i = 0; i < 4; i++) {
                out[targetNR][i] = points[i];
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
        out[0] = targetPoints[0][1] + (targetPoints[0][3] - targetPoints[0][1]) / 2;
        out[1] = targetPoints[1][1] + (targetPoints[1][3] - targetPoints[1][1]) / 2;
        out[2] = targetPoints[0][0] + (targetPoints[1][0] - targetPoints[0][0]) / 2;
        out[3] = targetPoints[0][2] + (targetPoints[1][2] - targetPoints[0][2]) / 2;
        out[4].y = out[2].y + (out[3].y - out[2].y) / 2;
        out[4].x = out[2].x + (out[3].x - out[2].x) / 2;

        output = out;

    }

}