#ifndef TRACKINGREFORMING_H
#define TRACKINGREFORMING_H

#include "fileLoader.h"
#include "classification.h"

#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class TrackingReforming
{
public:
    TrackingReforming();

    void setFileLoader( FileLoader* fileLoader );

    void registerCurrentBoundaries( std::vector<cv::Rect>& boundaries );
    std::vector<cv::Rect> getNewBoundaries();
	void getNewClasses(std::vector< std::pair<cv::Rect, TrafficClass> >& classificationResults);

private:
    void calculateNewBoundaries( std::vector<cv::Rect>& boundaries );
    cv::Rect enlargeRect(cv::Rect oldBound, int paddingX, int paddingY);
    cv::Rect enlargeRect(cv::Rect old);
	void drawDebugBigBox(cv::Rect bigBox, cv::Mat& image);
	void showDebug(cv::Mat image);

    FileLoader* m_fileLoader;
    std::map<int, cv::Rect> m_calculatedBoundaries; //< map -> id to cv::Rect
	
	// workaround for Class assignment
	std::map<int,cv::Vec3i> m_calculatedClasses;

};

#endif // TRACKINGREFORMING_H
