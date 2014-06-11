#include "statistics.h"

#include <iostream>

Statistics::Statistics()
    :m_frameCount(0)
    ,m_boundaryCount(0)
{
    for( int i = 0; i < eTrafficClassSize; i++ )
    {
        m_detectedObjectsCount[i] = 0;
    }
}

void Statistics::incrementFrameCount( const unsigned int frameCount )
{
    m_frameCount += frameCount;
}

void Statistics::incrementBoundaryCount( const unsigned int boundaryCount )
{
    m_boundaryCount += boundaryCount;
}

void Statistics::incrementDetectedObjectsCount( TrafficClass trafficClass )
{
    assert( trafficClass < static_cast<int>( eTrafficClassSize ) && "Tried to increase a non-valid TrafficClass" );

    m_detectedObjectsCount[trafficClass]++;
}

void Statistics::summaryOutput()
{
    std::cout << "  ---------- Statistics ---------- " << std::endl;
    std::cout << " | Frames:     " << m_frameCount << std::endl;
    std::cout << " | Boundaries: " << m_boundaryCount << std::endl;
    std::cout << " | " << std::endl;
    std::cout << " | Classification " << std::endl;
    std::cout << " |  \\--------------- Bicycle: " << m_detectedObjectsCount[bicycle] << std::endl;
    std::cout << " |   \\------------------ Car: " << m_detectedObjectsCount[car] << std::endl;
    std::cout << " |    \\--------------- Human: " << m_detectedObjectsCount[human] << std::endl;
    std::cout << " |________________________________ " << std::endl;
}
