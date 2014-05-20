#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <map>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

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
    ~Classification();

    void setFileLoader( FileLoader *fileLoader );
    void setBoundariesOfMovement( std::vector<cv::Rect> &boundaries );
    TrafficClass getTrafficClassOfBoundary( cv::Rect boundary );
    std::vector< std::pair<cv::Rect, TrafficClass> >  getTrafficClasses();
    void displayDebugWindows();

    // Main function, which tries to run classifiers
    void runClassifier();
//    void setParameter( string );

private:
    FileLoader *m_fileLoader;
    std::vector<cv::Rect> m_boundaries;
    std::vector< std::pair<cv::Rect, TrafficClass> >  m_results;
    std::string m_cascadePathCar;
    std::string m_cascadePathHuman;
    std::string m_cascadePathBicycle;
//    DetectionBasedTracker m_dbt_car;
//    DetectionBasedTracker m_dbt_human;
//    DetectionBasedTracker m_dbt_bicycle;
    cv::Mat m_currentImage;
};

#endif // CLASSIFICATION_H
