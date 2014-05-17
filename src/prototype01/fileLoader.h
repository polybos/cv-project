#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

#include <opencv2/core/core.hpp>

class FileLoader
{
public:
    FileLoader();
    ~FileLoader();

    //! set directory of image files; all images are loaded internally immediately
    void setDirectory( std::string );
    unsigned int getSequenceSize();
    unsigned int getSequencePosition();
    void setSequencePosition( unsigned int pos );
//    cv::Mat getSequence( unsigned int pos );
    cv::Mat getCurrentImage();
//    cv::Mat getNextImage();
//    cv::Mat getPreviousImage();
    void step();

private:
    std::vector<cv::Mat> m_allImgs;
    std::string m_dir;
    unsigned int m_size;
    unsigned int m_pos;

    bool loadImages();
};

#endif // FILELOADER_H
