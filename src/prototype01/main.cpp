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

    // Step2: Calculate tracking of moved objects
    // ToDo!

    // Step3: Calculate classification of tracked objects
    // ToDo!

    // Step4: Display results
    // ToDo!


    // Some cleanup
    delete( m_fileLoader );
    delete( m_tracking );
    delete( m_classification );
    delete( m_display );

    return 0;
}
