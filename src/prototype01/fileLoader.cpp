#include "fileLoader.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>


FileLoader::FileLoader()
    :m_allImgs()
    ,m_dir( std::string( "" ) )
    ,m_size( 0 )
    ,m_pos( 0 )
{
}

FileLoader::~FileLoader()
{

}

void FileLoader::setDirectory( std::string dir )
{
    m_dir = dir;
    loadImages();
}

unsigned int FileLoader::getSequenceSize()
{
    return m_size;
}

unsigned int FileLoader::getSequencePosition()
{
    return m_pos;
}

void FileLoader::setSequencePosition(unsigned int pos)
{
    assert( pos < m_size && "Whoops, position in sequence must not be larger than size of sequence!" );
    m_pos = pos;
}

cv::Mat FileLoader::getCurrentImage()
{
    if( m_size > 0 )
    {
        return m_allImgs[ m_pos ].clone();
    }
    else
        return cv::Mat();
}

void FileLoader::step()
{
    m_pos = ( m_pos + 1 >= m_size ) ? 0 : m_pos + 1;
}

// ---------------
// --- private ---

bool FileLoader::loadImages()
{
    std::stringstream ss;
    ss << m_dir;
    cv::VideoCapture sequence( ss.str() );
    if( !sequence.isOpened() )
    {
        std::cerr << "Failed to open image sequence!\n" << std::endl;
        m_allImgs.clear();
        return false;
    }

    //! Load all images sequentially into vector m_allImgs
    for( ; ; )
    {
        cv::Mat image;
        sequence >> image;

        if( image.empty() )
        {
            std::cout << "End of Sequence - Loaded " << m_allImgs.size() << " files" << std::endl;
            break;
        }

        m_allImgs.push_back( image.clone() );
    }
    m_size = m_allImgs.size();

    return true;
}
