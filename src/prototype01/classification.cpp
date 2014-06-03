#include "classification.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur, Equalize Histogram
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>


Classification::Classification()
    :m_fileLoader()
    ,m_boundaries()
    ,m_results( std::vector< std::pair<cv::Rect, TrafficClass> >() )
    ,m_cascadePathCar( std::string("haarcascades/car.xml") )
    ,m_cascadePathHuman( std::string("haarcascades/human.xml") )
    ,m_cascadePathBicycle( std::string("haarcascades/bicycle.xml") )
    ,m_cc_car( cv::CascadeClassifier( ) )
    ,m_cc_human( cv::CascadeClassifier( ) )
    ,m_cc_bicycle( cv::CascadeClassifier( ) )
    ,m_currentImage()

{
    std::cout << "m_cascadePathCar:" << m_cascadePathCar << std::endl;
    std::cout << "m_cascadePathHuman:" << m_cascadePathHuman << std::endl;
    std::cout << "m_cascadePathBicycle:" << m_cascadePathBicycle << std::endl;

    if( !m_cc_car.load( m_cascadePathCar ) )
        std::cerr << "Error: Could not load cascade file: " << m_cascadePathCar     << std::endl;
    if( !m_cc_human.load( m_cascadePathHuman ) )
        std::cerr << "Error: Could not load cascade file: " << m_cascadePathHuman   << std::endl;
    if( !m_cc_bicycle.load( m_cascadePathBicycle ) )
        std::cerr << "Error: Could not load cascade file: " << m_cascadePathBicycle << std::endl;


}

Classification::~Classification()
{

}

void Classification::setFileLoader( FileLoader *fileLoader )
{
    m_fileLoader = fileLoader;
}

void Classification::setBoundariesOfMovement( std::vector<cv::Rect> &boundaries )
{
    m_boundaries = boundaries;
}

TrafficClass Classification::getTrafficClassOfBoundary( cv::Rect boundary )
{
    cv::Mat m_currentImage = m_fileLoader->getCurrentImage();

    // calculate position and size for bigger border
    int padding = 20;
    int x = ( boundary.x < padding ) ? 0 : boundary.x - padding;
    int y = ( boundary.y < padding ) ? 0 : boundary.y - padding;
    int width = 0;
    int distanceXandWidth = m_currentImage.size().width - x;
    if( distanceXandWidth > boundary.width + 2*padding )
    {
        width = boundary.width + 2*padding;
    }
    else
    {
        width = distanceXandWidth;
    }
    int height = 0;
    int distanceYandHeight = m_currentImage.size().height - y;
    if( distanceYandHeight > boundary.height + 2*padding )
    {
        height = boundary.height + 2*padding;
    }
    else
    {
        height = distanceYandHeight;
    }
    cv::Rect biggerBoundary = cv::Rect( x, y, width, height );

    // create region-of-interest and do some preprocessing
    cv::Mat roiMat = m_currentImage( biggerBoundary );
    cv::Mat gray;
    cv::cvtColor( roiMat, gray, cv::COLOR_BGR2GRAY );
    cv::equalizeHist( gray, gray );

    // start detection of objects
    // todo! find a way to detect multiple objects in one setting (actually returns only first detected object-class)
    std::vector< cv::Rect> detected_bicycles;
    m_cc_human.detectMultiScale( gray, detected_bicycles, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    if( detected_bicycles.size() >= 1 )
    {
        std::cout << "Bicycle!!!!" << std::endl;
        return bicycle;
    }

    std::vector< cv::Rect > detected_human;
    m_cc_human.detectMultiScale( gray, detected_human, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    if( detected_human.size() >= 1 )
    {
        std::cout << "Human!!!!" << std::endl;
        return human;
    }

    std::vector< cv::Rect > detected_cars;
    m_cc_car.detectMultiScale( gray, detected_cars, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );
    if( detected_cars.size() >= 1 )
    {
        std::cout << "Car!!!!" << std::endl;
        return car;
    }


    std::cout << "Undefined!!!!" << std::endl;
    return undefined;
}

std::vector< std::pair<cv::Rect, TrafficClass> > Classification::getTrafficClasses()
{
    return m_results;
}


void Classification::displayDebugWindows()
{

}

void Classification::runClassifier()
{
    m_results.clear();

    std::vector<cv::Rect>::iterator it;
    for( it = m_boundaries.begin(); it != m_boundaries.end(); ++it )
    {
        m_results.push_back( std::pair<cv::Rect, TrafficClass>( *it, getTrafficClassOfBoundary( *it ) ) );
    }
}
