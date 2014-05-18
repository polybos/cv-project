#include "classification.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>

Classification::Classification()
    :m_fileLoader()
    ,m_boundaries()
    ,m_results()
    ,m_cascadePathCar( std::string("haarcascades/car.xml") )
    ,m_cascadePathHuman( std::string("haarcascades/human.xml") )
    ,m_cascadePathBicycle( std::string("haarcascades/bicycle.xml") )
    ,m_dbt_car( DetectionBasedTracker( m_cascadePathCar, DetectionBasedTracker::Parameters() ) )
    ,m_dbt_human( DetectionBasedTracker( m_cascadePathHuman, DetectionBasedTracker::Parameters() ) )
    ,m_dbt_bicycle( DetectionBasedTracker( m_cascadePathBicycle, DetectionBasedTracker::Parameters() ) )
    ,m_currentImage()

{
    std::cout << "m_cascadePathCar:" << m_cascadePathCar << std::endl;
    std::cout << "m_cascadePathHuman:" << m_cascadePathHuman << std::endl;
    std::cout << "m_cascadePathBicycle:" << m_cascadePathBicycle << std::endl;

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
    cv::Mat gray;
    cv::cvtColor( m_currentImage, gray, cv::COLOR_BGR2GRAY );

    m_dbt_car.process( gray );
    m_dbt_human.process( gray );
    m_dbt_bicycle.process( gray );

    std::vector< cv::Rect_<int> > detected_cars;
    m_dbt_car.getObjects( detected_cars );

    std::vector< cv::Rect_<int> > detected_human;
    m_dbt_human.getObjects( detected_human );

    std::vector< cv::Rect_<int> > detected_bicycles;
    m_dbt_bicycle.getObjects( detected_bicycles );

    if( detected_cars.size() >= 1 )
    {
        std::cout << "Car!!!!" << std::endl;
        return car;
    }
    if( detected_human.size() >= 1 )
    {
        std::cout << "Human!!!!" << std::endl;
        return human;
    }
    if( detected_bicycles.size() >= 1 )
    {
        std::cout << "Bicycle!!!!" << std::endl;
        return bicycle;
    }

    return undefined;
}

std::vector< std::pair<cv::Rect, TrafficClass> > Classification::getTrafficClasses()
{
    std::vector< std::pair<cv::Rect, TrafficClass> > result_vector;

    std::vector<cv::Rect>::iterator it;
    for( it = m_boundaries.begin(); it != m_boundaries.end(); ++it )
    {
        result_vector.push_back( std::pair<cv::Rect, TrafficClass>( *it, getTrafficClassOfBoundary( *it ) ) );
    }

    return result_vector;
}

void Classification::displayDebugWindows()
{

}

void Classification::runClassifier()
{
    // Todo! Some more code here ...
    getTrafficClasses();
}
