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
    // var
    VideoCapture* video = new VideoCapture[1];
    
    // file location
    string fileLocation = constant::directory + "Input/";
    
    // video files
    string videoFiles[] = { "ObjectAbandonmentAndRemoval1.avi", "ObjectAbandonmentAndRemoval2.avi"};
    
    // set filename
    string filename(fileLocation);
    filename.append(videoFiles[index]);
    
    // open video
    video[0].open(filename);
    
    // if opening files
    if(!video[0].isOpened()) {
        cout << "Cannot open video file: " << filename << endl;
    }

    return video;
}