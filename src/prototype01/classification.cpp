#include "classification.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
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
    ,m_dbt_car( cv::DetectionBasedTracker( m_cascadePathCar, cv::DetectionBasedTracker::Parameters() ) )
    ,m_dbt_human( cv::DetectionBasedTracker( m_cascadePathHuman, cv::DetectionBasedTracker::Parameters() ) )
    ,m_dbt_bicycle( cv::DetectionBasedTracker( m_cascadePathBicycle, cv::DetectionBasedTracker::Parameters() ) )
    ,m_currentImage()

{
    std::cout << "m_cascadePathCar:" << m_cascadePathCar << std::endl;
    std::cout << "m_cascadePathHuman:" << m_cascadePathHuman << std::endl;
    std::cout << "m_cascadePathBicycle:" << m_cascadePathBicycle << std::endl;


    DetectionBasedTracker::Parameters param;
    param.maxObjectSize = 400;
    param.maxTrackLifetime = 20;
    param.minDetectionPeriod = 7;
    param.minNeighbors = 3;
    param.minObjectSize = 20;
    param.scaleFactor = 1.1;

    m_dbt_car.setParameters( param );
    m_dbt_human.setParameters( param );
    m_dbt_bicycle.setParameters( param );

    m_dbt_car.run();
    m_dbt_human.run();
    m_dbt_bicycle.run();
}

Classification::~Classification()
{
    m_dbt_car.stop();
    m_dbt_human.stop();
    m_dbt_bicycle.stop();
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
    cv::Mat roiMat = m_currentImage;
    cv::Mat gray;
    cv::cvtColor( roiMat, gray, cv::COLOR_BGR2GRAY );


    m_dbt_car.process( gray );
    std::vector< cv::Rect > detected_cars;
    m_dbt_car.getObjects( detected_cars );

    if( detected_cars.size() >= 1 )
    {
        std::cout << "Car!!!!" << std::endl;
        return car;
    }


    m_dbt_human.process( gray );
    std::vector< cv::Rect > detected_human;
    m_dbt_human.getObjects( detected_human );

    if( detected_human.size() >= 1 )
    {
        std::cout << "Human!!!!" << std::endl;
        return human;
    }


    m_dbt_bicycle.process( gray );
    std::vector< cv::Rect> detected_bicycles;
    m_dbt_bicycle.getObjects( detected_bicycles );

    if( detected_bicycles.size() >= 1 )
    {
        std::cout << "Bicycle!!!!" << std::endl;
        return bicycle;
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
