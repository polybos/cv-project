#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

#include <opencv2/core/core.hpp>

class FileLoader
{
public:
    FileLoader();

    void setDirectory( std::string );
    unsigned int getSequenceSize();
    unsigned int getSequencePosition();
    void setSequencePosition( unsigned int pos );
//    cv::Mat getSequence( unsigned int pos );
    cv::Mat getCurrentImage();
//    cv::Mat getNextImage();
//    cv::Mat getPreviousImage();
    void step();
};

#endif // FILELOADER_H
