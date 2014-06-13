#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <map>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "fileLoader.h"

enum TrafficClass
{
    car = 0,
    human,
    bicycle,
    undefined,
    eTrafficClassSize
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

    // set parameter
    void setScaleFactor( double scaleFactor );
    void setMinNeighbours( unsigned int minNeighbours );
    void setMinSize( unsigned int minSize );

private:
    FileLoader *m_fileLoader;
    std::vector<cv::Rect> m_boundaries;
    std::vector< std::pair<cv::Rect, TrafficClass> >  m_results;
    std::string m_cascadePathCar;
    std::string m_cascadePathHuman;
    std::string m_cascadePathBicycle;
    cv::CascadeClassifier m_cc_car;
    cv::CascadeClassifier m_cc_human;
    cv::CascadeClassifier m_cc_bicycle;
    cv::Mat m_currentImage;

    // parameter of detectMultiScale
    double m_scaleFactor;
    unsigned int m_minNeighbours;
    unsigned int m_minSize;
};

#endif // CLASSIFICATION_H
