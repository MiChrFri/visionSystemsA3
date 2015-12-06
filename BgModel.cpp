//
//  BgModel.cpp
//  Assignment3
//
//  Created by Michael Frick on 05/12/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "BgModel.hpp"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat* differenceImage(Mat* currentFrame, Mat* background) {
    Mat* output = new Mat;
    absdiff(*background, *currentFrame, *output);
    
    return output;
}