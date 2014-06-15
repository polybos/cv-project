#ifndef TRACKINGREFORMING_H
#define TRACKINGREFORMING_H

#include "fileLoader.h"

#include <map>
#include <opencv2/core/core.hpp>

class TrackingReforming
{
public:
    TrackingReforming();

    void setFileLoader( FileLoader* fileLoader );

    void registerCurrentBoundaries( std::vector<cv::Rect>& boundaries );
    std::vector<cv::Rect> getNewBoundaries();

private:
    void calculateNewBoundaries( std::vector<cv::Rect>& boundaries );

    FileLoader* m_fileLoader;
    std::map<int, cv::Rect> m_calculatedBoundaries; //< map -> id to cv::Rect

};

#endif // TRACKINGREFORMING_H
