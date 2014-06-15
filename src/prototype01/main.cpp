#include <string>

#include "classification.h"
#include "display.h"
#include "fileLoader.h"
#include "parameterEvaluator.h"
#include "statistics.h"
#include "tracking.h"
#include "trackingreforming.h"

// #define PARAMETERTEST

void waitForKeyboard(int &keyboard, bool &stepMode);

int main( int argc, char** argv )
{
    // We need at least 1 given parameter
    assert( argc >= 2 && "Missing parameter - path with image files (opencv syntax)" );

    // Initialize objects with main functionality
    FileLoader *m_fileLoader = new FileLoader();
    Tracking *m_tracking = new Tracking();
    TrackingReforming *m_trackingReforming = new TrackingReforming();
    Classification *m_classification = new Classification();
    Display *m_display = new Display();
    Statistics *m_statistics = new Statistics();

	std::vector<cv::Rect> boundaries;
    bool stepMode = true;

    // Step1: Load files of given directory
    std::string path = argv[1];
    m_fileLoader->setDirectory( path );

    // Step2: Let objects know each other ...
    m_tracking->setFileLoader( m_fileLoader );
    m_trackingReforming->setFileLoader( m_fileLoader );
    m_classification->setFileLoader( m_fileLoader );
    m_display->setFileLoader( m_fileLoader );
//    m_display->setTrackingObject( m_tracking );
//    m_display->setClassifcationObject( m_classification );

#ifdef PARAMETERTEST
    ParameterEvaluator *m_parameterEvaluator = new ParameterEvaluator();

    m_parameterEvaluator->setFileLoaderObject( m_fileLoader );
    m_parameterEvaluator->setTrackingObject( m_tracking );
    m_parameterEvaluator->setClassificationObject( m_classification );
    m_parameterEvaluator->setDisplayObject( m_display );
    m_parameterEvaluator->setStatisticsObject( m_statistics );
    m_parameterEvaluator->run();
    m_parameterEvaluator->outputStatistics();

#else // PARAMETERTEST
	int keyboard = 0;
    std::vector< std::pair<cv::Rect, TrafficClass> > classificationResults;

    while( m_fileLoader->getSequencePosition() != m_fileLoader->getSequenceSize()-1 && (char)keyboard != 27 )
    {
        // Step3.1: Calculate tracking of moved objects
		boundaries = m_tracking->getBoundariesOfMovement();
        //debug output
//        m_tracking->displayDebugWindows();

        // Step3.2: Improve boundaries
        m_trackingReforming->registerCurrentBoundaries( boundaries );
        boundaries.clear();
        boundaries = m_trackingReforming->getNewBoundaries();

        // Step4: Calculate classification of tracked objects
        m_classification->setBoundariesOfMovement( boundaries );
        m_classification->runClassifier();
        classificationResults = m_classification->getTrafficClasses();

        // Step5: Display results
        m_display->setClassificationResults( classificationResults );
        m_display->displayResult();
//        m_display->displayTrackingOutput();

        // Step6: Log some things for statistics (for better comparision when testing parameters ...)
        m_statistics->incrementFrameCount();
        m_statistics->incrementBoundaryCount( boundaries.size() );
        for( std::vector< std::pair<cv::Rect, TrafficClass> >::iterator it = classificationResults.begin();
             it != classificationResults.end();
             ++it )
        {
            m_statistics->incrementDetectedObjectsCount( it->second );
        }

        // Step7: Wait for keyboard
        waitForKeyboard( keyboard, stepMode );

        // Switch to next image for next round
        m_fileLoader->step();
    }

    // Print collected statistics ...
    m_statistics->summaryOutput();
#endif //PARAMETERTEST

    // Some cleanup
#ifdef PARAMETERTEST
    delete m_parameterEvaluator;
#endif // PARAMETERTEST
    delete m_statistics;
    delete m_display;
    delete m_classification;
    delete m_trackingReforming;
    delete m_tracking;
    delete m_fileLoader;

    return 0;
}



void waitForKeyboard(int &keyboard, bool &stepMode)
{
    if( !stepMode )
    {
        keyboard = cv::waitKey(40);
        if ( keyboard == 32 )
        {
            stepMode = true;
        }
    }
    else
    {
        keyboard = cv::waitKey(0);
        if( keyboard == 32 )
        {
            stepMode = false;
        }
    }
}
