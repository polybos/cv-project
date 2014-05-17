#include "fileLoader.h"
#include "tracking.h"
#include "classification.h"
#include "display.h"

int main( int argc, char** argv )
{
    FileLoader *m_fileLoader;
    Tracking *m_tracking;
    Classification *m_classification;
    Display *m_display;

    // todo!
    // m_fileLoader->setDirectory(  );

    delete( m_fileLoader );
    delete( m_tracking );
    delete( m_classification );
    delete( m_display );

    return 0;
}
