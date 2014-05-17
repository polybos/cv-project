#ifndef DISPLAY_H
#define DISPLAY_H

#include "tracking.h"
#include "classification.h"

class Display
{
public:
    Display();

    void setTrackingObject( Tracking tracking );
    void setClassifcationObject( Classification classification );
    void displayResult();
//    void displayTrackingOutput();
//    void displayClassificationOutput();
};

#endif // DISPLAY_H
