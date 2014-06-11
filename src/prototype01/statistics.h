#ifndef STATISTICS_H
#define STATISTICS_H

#include "classification.h"

class Statistics
{
public:
    Statistics();

    void incrementFrameCount( const unsigned int frameCount = 1  );
    void incrementBoundaryCount( const unsigned int boundaryCount = 1 );
    void incrementDetectedObjectsCount( TrafficClass trafficClass );

    void summaryOutput();

private:
    int m_frameCount;
    int m_completeFrameCount;
    int m_boundaryCount;
    int m_detectedObjectsCount[eTrafficClassSize];
};

#endif // STATISTICS_H
