//
//  VideoLoader.cpp
//  Assignment3
//
//  Created by Michael Frick on 04/12/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <iostream>
#include "VideoLoader.hpp"
#include "Constants.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

VideoCapture* loadVideo(int index) {
    // file location
    string fileLocation = constant::directory + "Input/";
    string videoFiles[] = { "ObjectAbandonmentAndRemoval1.avi", "ObjectAbandonmentAndRemoval2.avi", "LeftBag.mpg" };
    
    VideoCapture* video = new VideoCapture[1];
    
    string filename(fileLocation);
    filename.append(videoFiles[index]);
    
    video[0].open(filename);
    
    if(!video[0].isOpened()) {
        cout << "Cannot open video file: " << filename << endl;
    }

    return video;
}