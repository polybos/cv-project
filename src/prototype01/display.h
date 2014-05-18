#ifndef DISPLAY_H
#define DISPLAY_H

#include "tracking.h"
#include "classification.h"

class Display
{
public:
    Display();

    bool shouldQuit;

    void setTrackingObject( Tracking *tracking );
    void setClassifcationObject( Classification *classification );
    void displayResult();
    void displayTrackingOutput();
//    void displayClassificationOutput();

private:
    Tracking *m_tracking;
    Classification *m_classification;
};

#endif // DISPLAY_H
