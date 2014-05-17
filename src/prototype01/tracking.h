#ifndef TRACKING_H
#define TRACKING_H

#include <vector>

#include <opencv2/core/core.hpp>

#include "fileLoader.h"

class Tracking
{
public:
    Tracking();

    void setFileLoader( FileLoader fileLoader );
    std::vector<cv::Rect> getBoundariesOfMovement;
    void displayDebugWindows();
//    unsigned int getIdOfBoundaries( cv::Rect boundary );
//    void setParameter( string );
};

#endif // TRACKING_H
