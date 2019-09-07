#include "FridoPipeline.h"

using namespace cv;
using namespace std;

frido::FridoPipeline myfrido;

namespace frido {

    FridoCalculation::FridoCalculation() {
    }

    void FridoCalculation::Calculate(vector<vector<Point> >& convertedContoursInput) {
        //Step CheckContours:
        //input
        vector<vector<Point> > convertedContours = convertedContoursInput;
        vector<Point> space_holder_contour;
        checkContours(convertedContours, space_holder_contour, this->checkedContoursOutput);
        
    }

    vector<vector<Point> >* FridoCalculation::GetCheckedContoursOutput() {
        return &(this->checkedContoursOutput);
    }

    void FridoCalculation::checkContours(vector<vector<Point> > &convertedContours, vector<Point> &space_holder_contour, vector<vector<Point> > &output) {
        output.clear();
        if (convertedContours.size() == 2) {	
            if (convertedContours[0][0].x > convertedContours[1][0].x) {
                space_holder_contour = convertedContours[0];
                convertedContours[0] = convertedContours[1];
                convertedContours[1] = space_holder_contour;
            }
        }
        output = convertedContours;
    }

}