//
//  main.cpp
//  Assignment1VS
//
//  Created by Michael Frick on 07/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include "VideoLoader.hpp"
#include "BgModel.hpp"

using namespace cv;
using namespace std;

// main
int main(int argc, const char * argv[]) {

    // vars
    Mat* previousFrame = new Mat;
    Mat* backgroundFrame = new Mat;
    Mat* frame = new Mat;
    
    int objects = 0;
    int objectsBefore = 0;
    vector<vector<int>> objectRect;
    
    int width = 320;
    int height = 180;
    
    Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
    Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    
    int fps = 25;
    double wait = 100.0/fps;
    
    for(int v = 1; v < 2; v++) {
        VideoCapture* video = loadVideo(v);
        
        Mat* movingStuff = new Mat(Size(width, height), CV_8U);

        while(video[0].isOpened()) {
            if(video[0].read(*frame) != false) {
                
                cvtColor(*frame, *frame, COLOR_BGR2GRAY);
                resize(*frame, *frame, Size(width, height), 0, 0, INTER_LINEAR);
                
                Mat* blurredFrame = new Mat;
                GaussianBlur(*frame, *blurredFrame, Size( 5, 5), 1.5);
                
                unsigned int currentFrameNo = video[0].get(CAP_PROP_POS_FRAMES);
                
                if(currentFrameNo == 1) {
                    *backgroundFrame = blurredFrame->clone();
                }
                else {
                    imshow("currentFrame", *blurredFrame);
                    moveWindow("currentFrame", 0, backgroundFrame->rows + 50);
                    
                    Mat* diff = new Mat;
                    
                    diff = differenceImage(blurredFrame, backgroundFrame);
                    imshow("blur", *diff);

                    //update the background model
                    pMOG2->apply(*blurredFrame, fgMaskMOG2, 0.0001);
                    
                    Mat* threshImg = new Mat;
                    threshold(fgMaskMOG2, *threshImg, 150, 255, THRESH_BINARY);
                    
                    if(currentFrameNo > 25 && currentFrameNo % 5 == 0) {
                        for(int r = 0; r < movingStuff->rows; r++) {
                            for(int c = 0; c < movingStuff->cols; c++) {
                                
                                if(threshImg->at<uchar>(r,c) == 255) {
                                    int currentPixlVal = movingStuff->at<uchar>(r,c);
                                    
                                    if(threshImg->at<uchar>(r,c) == previousFrame->at<uchar>(r,c)) {
                                        if(currentPixlVal < 245) {
                                            movingStuff->at<uchar>(r,c) +=10;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    Mat* threshIt = new Mat;
                    threshold(*movingStuff, *threshIt, 100, 255, THRESH_BINARY);
                    imshow("letdass see", *threshIt);
                    
                    /********** dilate */
                    Mat* dilated = new Mat;
                    Mat structuring_element( 2, 3, CV_8U, Scalar(1) );
                    dilate( *threshIt, *dilated, structuring_element);
                    
                    if(currentFrameNo > 2 && currentFrameNo % 15 == 0) {
                        
                        ////////////////CONTOURS START
                        vector<vector<Point> > contours;
                        
                        // find the contours
                        findContours(*dilated, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
                        
                        vector<vector<Point> > contours_poly( contours.size() );
                        vector<Point2f>center( contours.size() );
                        vector<float>radius( contours.size() );
                        
                        for( int ii = 0; ii < contours.size(); ii++ ){
                            approxPolyDP( Mat(contours[ii]), contours_poly[ii], 3, true );
                            minEnclosingCircle( (Mat)contours_poly[ii], center[ii], radius[ii] );
                        }
                        
                        for(int i = 0; i < center.size(); i++) {
                            Rect roi;
                            
                            objects = 0;
                            objectRect.clear();
      
                            if(radius[i] > 5) {
                                
                                objects++;
                                vector<int> xyw = {int(center[i].x), int(center[i].y), int(radius[i]) };
                                objectRect.push_back(xyw);
                                
                                *previousFrame = threshImg->clone();
                                *backgroundFrame = blurredFrame->clone();
                                
                                if(objectsBefore != objects){
                                    objectsBefore = objects;
                                }
                                
                                for(int r = 0; r < movingStuff->rows; r++) {
                                    for(int c = 0; c < movingStuff->cols; c++) {
                                        movingStuff->at<uchar>(r,c) = 0;
                                    }
                                }

                            }
                        }
                        
                        *previousFrame = threshImg->clone();
                    }
                    
                    for(int i = 0; i < objectRect.size(); i++) {
                        circle(*frame, Point(objectRect[i][0], objectRect[i][1]), objectRect[i][2], Scalar(255, 255, 255));
                    }
                    
                    putText(*frame, to_string(objects), Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0,255,0), 2.0);
                    imshow("objects", *frame);
                }
                
                /*** video controls ***/
                int key = waitKey(wait);

                // video control
                switch(key) {
                case 32:
                    if (fps != 0) {
                        cout << "paused" << endl;
                        fps = 0;
                    }
                    else {
                        cout << "play" << endl;
                        fps = wait;
                    }
                    break;
                case 63234:
                    cout << "<<" << endl;
                    if(wait <= 200) {
                        wait += 10;
                    }
                    break;
                case 63235:
                    cout << ">>" << endl;
                    if(wait >= 10) {
                        wait -= 10;
                    }
                    else {
                        wait = 1;
                    }
                    break;
                }
            }
        }
    }
    
    
    cout << "press any key to end the programm" << endl;
    waitKey(0);

    return 0;
}
