#ifndef DISPLAY_H
#define DISPLAY_H

#include "tracking.h"
#include "classification.h"

class Display
{
public:
    Display();

    bool shouldQuit;

    void setFileLoader( FileLoader *fileLoader );
    void setTrackingObject( Tracking *tracking );
    void setClassificationObject( Classification *classification );
    void setClassificationResults( std::vector< std::pair<cv::Rect, TrafficClass> >& results );
    void displayResult();
    void displayTrackingOutput();
//    void displayClassificationOutput();

private:
    FileLoader *m_fileLoader;
    Tracking *m_tracking;
    Classification *m_classification;

    std::vector< std::pair<cv::Rect, TrafficClass> > m_results;
};

#endif // DISPLAY_H
