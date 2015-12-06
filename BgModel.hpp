//
//  BgModel.hpp
//  Assignment3
//
//  Created by Michael Frick on 05/12/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef BgModel_hpp
#define BgModel_hpp

#include "BgModel.hpp"
#include <opencv2/opencv.hpp>

cv::Mat* differenceImage(cv::Mat* currentFrame, cv::Mat* background);

#endif /* BgModel_hpp */
