#include "parameterEvaluator.h"

#include <iostream>

ParameterEvaluator::ParameterEvaluator()
    :m_tracking()
    ,m_classification()
    ,m_display()
    ,m_statistics()
    ,m_classificationParameter()
    ,m_parameterStatistics()
{    
    setDefaultTestParameter();
}

void ParameterEvaluator::run()
{
    assert( m_tracking != NULL       && "ParameterEvaluator missing valid tracking object" );
    assert( m_classification != NULL && "ParameterEvaluator missing valid classification object" );
    assert( m_display != NULL        && "ParameterEvaluator missing valid display object" );
    assert( m_statistics != NULL     && "ParameterEvaluator missing valid statistics object" );

    unsigned int currentRound = 0;
    unsigned int maxRound = m_classificationParameter.size();

    std::cout << "Start parameter cycle with " << maxRound << " rounds, NOW!" << std::endl;

    while( !finished() )
    {
        ClassificationTriplet triplet = m_classificationParameter.top();
        m_classification->setScaleFactor( triplet.scaleFactor );
        m_classification->setMinNeighbours( triplet.minNeighbours );
        m_classification->setMinSize( triplet.minSize );

        mainLoop();
        saveStatistic();
        m_statistics->clear();
        printParameterStatistic( m_parameterStatistics.top() );

        m_fileLoader->setSequencePosition( 0 );
        m_classificationParameter.pop(); // at the end, because statistics need current parameter
        std::cout << "Finished round ";
        printElement( ++currentRound, log( maxRound ) );
        std::cout << " of " << maxRound << std::endl;
    }

}

void ParameterEvaluator::setFileLoaderObject(FileLoader *fileLoader)
{
    m_fileLoader = fileLoader;
}

void ParameterEvaluator::setTrackingObject( Tracking *tracking )
{
    m_tracking = tracking;
}

void ParameterEvaluator::setClassificationObject( Classification *classification )
{
    m_classification = classification;
}

void ParameterEvaluator::setDisplayObject( Display *display )
{
    m_display = display;
}

void ParameterEvaluator::setStatisticsObject( Statistics *statistics )
{
    m_statistics = statistics;
}

void ParameterEvaluator::outputStatistics()
{
    std::cout << std::endl;
    std::cout << " ---------- Statistics ---------- " << std::endl;
    std::cout << " Classification through " << m_parameterStatistics.size() << " cycles" << std::endl;
    std::cout << std::endl;

    while( m_parameterStatistics.size() > 0 )
    {
        printParameterStatistic( m_parameterStatistics.top() );
        m_parameterStatistics.pop();
    }
}

// ---------------
// --- private ---

void ParameterEvaluator::mainLoop()
{
    std::vector<cv::Rect> boundaries;
    std::vector< std::pair<cv::Rect, TrafficClass> > classificationResults;

    while( m_fileLoader->getSequencePosition() != m_fileLoader->getSequenceSize()-1 )
    {
        // Step3: Calculate tracking of moved objects
        boundaries = m_tracking->getBoundariesOfMovement();
        //debug output
//        m_tracking->displayDebugWindows();

        // Step4: Calculate classification of tracked objects
        m_classification->setBoundariesOfMovement( boundaries );
        m_classification->runClassifier();
        classificationResults = m_classification->getTrafficClasses();

        // Step5: Display results
        m_display->setClassificationResults( classificationResults );
        m_display->displayResult();

        // Step6: Log some things for statistics (for better comparision when testing parameters ...)
        m_statistics->incrementFrameCount();
        m_statistics->incrementBoundaryCount( boundaries.size() );
        for( std::vector< std::pair<cv::Rect, TrafficClass> >::iterator it = classificationResults.begin();
             it != classificationResults.end();
             ++it )
        {
            m_statistics->incrementDetectedObjectsCount( it->second );
        }

        // Switch to next image for next round
        m_fileLoader->step();
    }
}

bool ParameterEvaluator::finished()
{
    return ( m_classificationParameter.size() == 0 );
}

void ParameterEvaluator::saveStatistic()
{
    ParameterStatistic statistic;
    statistic.triplet = m_classificationParameter.top();
    for( int i = 0; i != eTrafficClassSize; i++ )
    {
        statistic.detectedObjectsCount[i] = m_statistics->getDetectedObjectsCount( static_cast<TrafficClass>( i ) );
    }

    m_parameterStatistics.push( statistic );
}

void ParameterEvaluator::setDefaultTestParameter()
{
    double scaleFactor;
    unsigned int minNeighbours;
    unsigned int minSize;
    ClassificationTriplet triplet;

    /* we want to test following in all combinations:
     *      * scaleFactor 1.1 - 1.4
     *      * minNeighbours 1 - 5
     *      * minSize 10 - 50 (step size of 5)
     */

    for( scaleFactor = 1.1; scaleFactor <= 1.4; scaleFactor += 0.1 )
    {
        for( minNeighbours = 1; minNeighbours <= 5; minNeighbours += 1 )
        {
            for( minSize = 15; minSize <= 45; minSize += 5 )
            {
                triplet.scaleFactor     = scaleFactor;
                triplet.minNeighbours   = minNeighbours;
                triplet.minSize         = minSize;
                m_classificationParameter.push( triplet );
            }
        }
    }

}

void ParameterEvaluator::printParameterStatistic( const ParameterStatistic parameterStatistic )
{
    const ClassificationTriplet triplet = parameterStatistic.triplet;

    std::cout << "scaleFactor = ";
    printElement( triplet.scaleFactor, 3 );
    std::cout << " minNeighbours = ";
    printElement( triplet.minNeighbours, 1 );
    std::cout << " minSize = ";
    printElement( triplet.minSize, 2);
    std::cout << " | ";

    std::cout << "cars = ";
    printElement( parameterStatistic.detectedObjectsCount[car], 3 );
    std::cout << " human = ";
    printElement( parameterStatistic.detectedObjectsCount[human], 3 );
    std::cout << " bicycle = ";
    printElement( parameterStatistic.detectedObjectsCount[bicycle], 3 );
    std::cout << std::endl;
}

template<typename T> void ParameterEvaluator::printElement(T t, const int& width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}
