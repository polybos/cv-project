#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <vector>

#include <opencv2/core/core.hpp>

#include "fileLoader.h"

enum TrafficClass
{
    car,
    human,
    bicycle,
    undefined
};

class Classification
{
public:
    Classification();

    void setFileLoader( FileLoader fileLoader );
    void setBoundariesOfMovement( std::vector<cv::Rect> boundaries );
    TrafficClass getTrafficClassOfBoundarie( cv::Rect boundarie );
    std::map<cv::Rect, TrafficClass> getTrafficClasses();
    void displayDebugWindows();
//    void setParameter( string );
};

#endif // CLASSIFICATION_H
