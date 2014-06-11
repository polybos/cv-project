#include "display.h"

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

Display::Display()
    :shouldQuit(false)
    ,m_fileLoader()
    ,m_tracking()
    ,m_classification()
{
}

void Display::setFileLoader(FileLoader *fileLoader)
{
    m_fileLoader = fileLoader;
}

void Display::setTrackingObject( Tracking *tracking )
{
    m_tracking = tracking;
}

void Display::setClassificationObject( Classification *classification )
{
    m_classification = classification;
}

void Display::setClassificationResults(std::vector<std::pair<Rect, TrafficClass> >& results)
{
    m_results = results;
}

void Display::displayResult()
{
    // ToDo!
    const string windowName_results = "ResultWindow";

    cv::Mat img = m_fileLoader->getCurrentImage();


    for( int i = 0; i < m_results.size(); ++i)
    {
        Point tl = m_results.at(i).first.tl();
        Point br = m_results.at(i).first.br();

        CvScalar color = cvScalar( 100, 255, 100 );

        int thickness = 5;
        string label;

        switch ( m_results.at(i).second )
        {
        case car:
            label = "car";
            break;
        case human:
            label = "human";
            break;
        case bicycle:
            label = "bicycle";
            break;
        default:
            label = "not detected";
            thickness = 3;
            color = cvScalar( 50, 50, 50, 255 );
            break;
        }

        rectangle( img, tl, br, color, thickness );
        putText( img, label, tl-Point(-5,5), 0, (double)thickness/3.0, color, thickness/1.25);
    }

    namedWindow( windowName_results, WINDOW_AUTOSIZE );
    imshow( windowName_results, img);
}

void Display::displayTrackingOutput()
{
    m_tracking->displayDebugWindows();
}
