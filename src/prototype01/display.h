#ifndef DISPLAY_H
#define DISPLAY_H

#include "tracking.h"
#include "classification.h"
#include "statistics.h"

class Display
{
public:
    Display();
	virtual ~Display();

    bool shouldQuit;

    void setFileLoader( FileLoader *fileLoader );
    void setTrackingObject( Tracking *tracking );
    void setClassificationObject( Classification *classification );
    void setClassificationResults( std::vector< std::pair<cv::Rect, TrafficClass> >& results );
    void displayResult();
    void displayTrackingOutput();
	void displayStatistics();
//    void displayClassificationOutput();

private:
    FileLoader *m_fileLoader;
    Tracking *m_tracking;
    Classification *m_classification;
	Statistics *m_statistics;

    std::vector< std::pair<cv::Rect, TrafficClass> > m_results;
};

#endif // DISPLAY_H
