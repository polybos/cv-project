#include "display.h"

Display::Display()
    :shouldQuit(false)
    ,m_tracking()
    ,m_classification()
{
}

void Display::setTrackingObject( Tracking *tracking )
{
    m_tracking = tracking;
}

void Display::setClassifcationObject( Classification *classification )
{
    m_classification = classification;
}

void Display::displayResult()
{
    // ToDo!
}

void Display::displayTrackingOutput()
{
    m_tracking->displayDebugWindows();
}
