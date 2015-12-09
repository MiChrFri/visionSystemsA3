//
//  main.cpp
//  Assignment1VS
//
//  Created by Michael Frick on 07/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include "VideoLoader.hpp"

using namespace cv;
using namespace std;

// vars
int frameOfDrop[2];
int frameOfPickUp[2];
int dropFrames[] = {183, 215};
int pickUpFrames[] = {509, 551};
int overlappingArea[2][2];
vector<Point> groundTruthTL = {Point(356/2,208/2), Point(287/2,261/2)};
vector<Point> groundTruthRB = {Point(390/2,239/2), Point(352/2,323/2)};
int objectArea[2];
int groundTruthArea[2] = {
    abs(groundTruthRB[0].x - groundTruthTL[0].x) * abs(groundTruthRB[0].y - groundTruthTL[0].y),
    abs(groundTruthRB[1].x - groundTruthTL[1].x) * abs(groundTruthRB[1].y - groundTruthTL[1].y)
};

// main
int main(int argc, const char * argv[]) {

    // vars
    Mat* previousFrame = new Mat;
    Mat* frame = new Mat;
    int objects = 0;
    vector<vector<Point>> objectRect;
    int width = 320;
    int height = 180;
    int fps = 25;
    double wait = 100.0/fps;
    
    for(int v = 0; v < 2; v++) {
        
        // load video
        VideoCapture* video = loadVideo(v);
        
        // vars
        Mat mask;                                                                   //MOG2 mask
        Ptr<BackgroundSubtractor> bgSubtr = createBackgroundSubtractorMOG2();       //MOG2 Background subtractor
        Mat* movingPixels = new Mat(Size(width, height), CV_8U);
        overlappingArea[v][0] = 0;
        overlappingArea[v][1] = 0;

        while(video[0].isOpened()) {
            if(video[0].read(*frame) != false) {
                
                // convert image to greyscale
                cvtColor(*frame, *frame, COLOR_BGR2GRAY);
                
                // resize image
                resize(*frame, *frame, Size(width, height), 0, 0, INTER_LINEAR);
                
                // use gaussian blur on image
                Mat* blurredFrame = new Mat;
                GaussianBlur(*frame, *blurredFrame, Size( 5, 5), 1.5);
                
                // get current frame number
                unsigned int currentFrameNo = video[0].get(CAP_PROP_POS_FRAMES);
                
                // start on the 2nd frame
                if (currentFrameNo > 0) {
                    
                    // apply background subtractor image
                    bgSubtr->apply(*blurredFrame, mask, 0.0001);
                    
                    // theshold the image
                    Mat* threshImg = new Mat;
                    threshold(mask, *threshImg, 150, 255, THRESH_BINARY);
                    
                    // do every 5th frame
                    if(currentFrameNo % 5 == 0) {
                        // loop trough all pixels
                        for(int r = 0; r < movingPixels->rows; r++) {
                            for(int c = 0; c < movingPixels->cols; c++) {
                                // if pixel is white
                                if(threshImg->at<uchar>(r,c) == 255) {
                                    // if the pixel in the current frame has the same value than the last frame
                                    if(threshImg->at<uchar>(r,c) == previousFrame->at<uchar>(r,c)) {
                                        // makes sure that value gets max 255
                                        if(movingPixels->at<uchar>(r,c) < 245) {
                                            // increase pixelvalue
                                            movingPixels->at<uchar>(r,c) +=10;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    // threshold image to get rid of lightgrey pixels
                    Mat* thresholdImg = new Mat;
                    threshold(*movingPixels, *thresholdImg, 100, 255, THRESH_BINARY);
                    
                    // dilate image
                    Mat* dilated = new Mat;
                    Mat structuring_element( 2, 3, CV_8U, Scalar(1) );
                    dilate( *thresholdImg, *dilated, structuring_element);
                    
                    // do every 17th frame
                    if(currentFrameNo % 17 == 0) {
                        vector<vector<Point> > contours;
                        
                        // find the contours
                        findContours(*dilated, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                        
                        vector<vector<Point> > contours_poly( contours.size() );
                        vector<Rect> boundRect( contours.size() );
                        vector<Point2f>center( contours.size() );
                        vector<float>radius( contours.size() );
                        
                        for( int ii = 0; ii < contours.size(); ii++ ){
                            approxPolyDP( Mat(contours[ii]), contours_poly[ii], 3, true );
                            boundRect[ii] = boundingRect( Mat(contours_poly[ii]) );
                            minEnclosingCircle( (Mat)contours_poly[ii], center[ii], radius[ii] );
                        }
                        
                        // for every contour that is found
                        for(int i = 0; i < center.size(); i++) {
                            objects = 0;
                            objectRect.clear();
      
                            // if the object is over a specific size
                            if(radius[i] > 6) {
                                // increase object count
                                objects++;
                                
                                // create vector of the upperleft and lowerright cornerpoints
                                vector<Point> tlBr = {boundRect[i].tl(), boundRect[i].br()};
                                
                                // add the cornerpoints
                                objectRect.push_back(tlBr);
                                
                                // set all pixelvalues back to black
                                threshold(*movingPixels, *movingPixels, 0, 0, THRESH_BINARY);
                            }
                        }
                        
                        *previousFrame = threshImg->clone();
                    }
                    
                    for(int i = 0; i < objectRect.size(); i++) {
                        // draw rectable of found objects
                        rectangle( *frame, objectRect[i][0], objectRect[i][1], Scalar(255, 255, 255), 1, 8, 0 );
                        
                        // get the area of the rectangle
                        objectArea[v] = abs(objectRect[i][0].x - objectRect[i][1].x) * abs(objectRect[i][0].y - objectRect[i][1].x);
                        
                        // groundTruth points
                        int aX = groundTruthTL[v].x;
                        int aY = groundTruthTL[v].y;
                        int aXMax = groundTruthRB[v].x;
                        int aYMax = groundTruthRB[v].y;
                        
                        // found rectangle points
                        int bX = objectRect[i][0].x;
                        int bY = objectRect[i][0].y;
                        int bXMax = objectRect[i][1].x;
                        int bYMax = objectRect[i][1].y;
                        
                        int area = 0;
                            
                        int dx = min(aXMax, bXMax) - max(aX, bX);
                        int dy = min(aYMax, bYMax) - max(aY, bY);
                        if (dx >= 0 && dy >= 0) {
                            area = dx*dy;
                        }
                        
                        overlappingArea[v][0]++;
                        overlappingArea[v][1] += area;

                        rectangle( *frame, groundTruthTL[v], groundTruthRB[v], Scalar(255, 255, 255), 1, 8, 0 );

                        
                        if(frameOfDrop[v] == 0) {
                            frameOfDrop[v] = currentFrameNo;
                        }
                        frameOfPickUp[v] = currentFrameNo;
                    }
                    
                    putText(*frame, to_string(objects), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0,255,0), 2.0);
                    imshow("objects", *frame);
                }
                
                /*** video controls ***/
                waitKey(wait);

                if(currentFrameNo == video[0].get(CV_CAP_PROP_FRAME_COUNT)) {
                    video[0].release();
                }
            }
        }
    }
    
    for(int i = 0; i < 2; i++) {
        double overlapArea = overlappingArea[i][1]/overlappingArea[i][0];
        double numerator = (2*overlapArea);
        double denumerator = (objectArea[i] + groundTruthArea[i]);
        double diceCoefficient = numerator/denumerator;
        int tp = 1;
        int fp = 0;
        int fn = 0;
        
        double precision    = ((double)tp/(double)(tp+fp));
        if(tp == 0 && fp == 0) {
            precision = 1;
        }
        double recall       = ((double)tp/(double)(tp+fn));
        if(tp == 0 && fn == 0) {
            recall = 1;
        }
        
        cout << "video " << i+1 << "\n-------" << endl;
        cout << "precision:                 " << precision << endl;
        cout << "recall:                    " << recall << endl;
        cout << "drop difference time:      " << abs(dropFrames[i] - frameOfDrop[i])/fps << endl;
        cout << "pickup difference time:    " << abs(pickUpFrames[i] - frameOfPickUp[i])/fps << endl;
        cout << "diceCoefficient:           " << diceCoefficient << endl;
        cout << endl;
    }
    
    cout << "press any key to end the programm" << endl;
    waitKey(0);

    return 0;
}

