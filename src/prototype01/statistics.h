#ifndef STATISTICS_H
#define STATISTICS_H

#include "classification.h"

class Statistics
{
public:
    Statistics();

    void clear();

    void setCompleteFrameCount( const unsigned int completeFrameCount );
    void incrementFrameCount( const unsigned int frameCount = 1  );
    void incrementBoundaryCount( const unsigned int boundaryCount = 1 );
    void incrementDetectedObjectsCount( TrafficClass trafficClass );

    const unsigned int getCompleteFrameCount();
    const unsigned int getFrameCount();
    const unsigned int getBoundaryCount();
    const unsigned int getDetectedObjectsCount( TrafficClass trafficClass );

    void summaryOutput();

private:
    unsigned int m_frameCount;
    unsigned int m_completeFrameCount;
    unsigned int m_boundaryCount;
    unsigned int m_detectedObjectsCount[eTrafficClassSize];
};

#endif // STATISTICS_H
