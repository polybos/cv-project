#include <string>

#include "fileLoader.h"
#include "tracking.h"
#include "classification.h"
#include "display.h"

int main( int argc, char** argv )
{
    // We need at least 1 given parameter
    assert( argc >= 2 && "Missing parameter - path with image files (opencv syntax)" );

    // Initialize objects with main functionality
    FileLoader *m_fileLoader = new FileLoader();
    Tracking *m_tracking = new Tracking();
    Classification *m_classification = new Classification();
    Display *m_display = new Display();

    // Step1: Load files of given directory
    std::string path = argv[1];
    m_fileLoader->setDirectory( path );

    // Step2: Let objects know each other ...
//    m_tracking->setFileLoader( m_fileLoader );
//    m_classification->setFileLoader( m_fileLoader );
//    m_display->setTrackingObject( m_tracking );
//    m_display->setClassifcationObject( m_classification );

    while( ! m_display->shouldQuit )
    {
        // Step3: Calculate tracking of moved objects
        // ToDo!

        // Step4: Calculate classification of tracked objects
        // ToDo!

        // Step5: Display results
        m_display->displayResult();

        // Switch to next image for next round
        m_fileLoader->step();
    }

    // Some cleanup
    delete( m_fileLoader );
    delete( m_tracking );
    delete( m_classification );
    delete( m_display );

    return 0;
}
