#include "trackingreforming.h"

#include <vector>

TrackingReforming::TrackingReforming()
    :m_fileLoader()
    ,m_calculatedBoundaries()
{    
}

void TrackingReforming::setFileLoader( FileLoader *fileLoader )
{
    m_fileLoader = fileLoader;
}

void TrackingReforming::registerCurrentBoundaries( std::vector<cv::Rect>& boundaries )
{
    if( m_fileLoader->getSequencePosition() == 0 || boundaries.size() == 0 ) //< first frame with boundaries
    {
        return;
    }
    int id = 0;
    std::vector<cv::Rect>::iterator it;
    for( it = boundaries.begin(); it != boundaries.end(); ++it )
    {
        m_calculatedBoundaries.insert( std::make_pair( id++, *it ) );
    }

    calculateNewBoundaries( boundaries );
}

std::vector<cv::Rect> TrackingReforming::getNewBoundaries()
{
    std::map<int, cv::Rect>::iterator it;
    std::vector<cv::Rect> returnVector;
    for( it = m_calculatedBoundaries.begin(); it != m_calculatedBoundaries.end(); ++it )
    {
        returnVector.push_back( it->second );
    }

    return returnVector;
}

// ---------------
// --- private ---

void TrackingReforming::calculateNewBoundaries( std::vector<cv::Rect>& boundaries )
{
    std::map<int, cv::Rect>::iterator it1;
    std::vector<cv::Rect>::iterator it2;
    std::vector< std::pair<cv::Rect, int> >::iterator it3;

    // create new vector
    std::vector< std::pair<cv::Rect, int> > boundariesChecked;
    for( it2 = boundaries.begin(); it2 != boundaries.end(); ++it2 )
    {
        boundariesChecked.push_back( std::make_pair( *it2, -1 ) );
    }

    cv::Mat currentImage = m_fileLoader->getCurrentImage();

    for( it1 = m_calculatedBoundaries.begin(); it1 != m_calculatedBoundaries.end(); )
    {
        cv::Rect oldBound = it1->second;

        // calculate position and size for bigger border
        int paddingX = currentImage.size().width  * 0.35;
        int paddingY = currentImage.size().height * 0.15;
        int x = ( oldBound.x < paddingX ) ? 0 : oldBound.x - paddingX;
        int y = ( oldBound.y < paddingY ) ? 0 : oldBound.y - paddingY;
        int width = 0;
        int distanceXandWidth = currentImage.size().width - x;
        if( distanceXandWidth > oldBound.width + 2*paddingX )
        {
            width = oldBound.width + 2*paddingX;
        }
        else
        {
            width = distanceXandWidth;
        }
        int height = 0;
        int distanceYandHeight = currentImage.size().height - y;
        if( distanceYandHeight > oldBound.height + 2*paddingY )
        {
            height = oldBound.height + 2*paddingY;
        }
        else
        {
            height = distanceYandHeight;
        }
        cv::Rect biggerBoundary = cv::Rect( x, y, width, height );
        cv::Rect bB = biggerBoundary;

        // look for adequate boundaries from boundariesChecked and check if found
        bool newBoundaryInside = false;
        for( it3 = boundariesChecked.begin(); it3 != boundariesChecked.end(); ++it3 )
        {
            if( it3->second >= 0 )
            {
                continue;
            }

            // check if a new boundary is inside bigger old/calculated/cumulated boundary
            cv::Rect nB = it3->first; //< newBoundary
            unsigned int bBx2 = bB.x + bB.width;
            unsigned int bBy2 = bB.y + bB.height;
            unsigned int nBx2 = nB.x + nB.width;
            unsigned int nBy2 = nB.y + nB.height;
            if( nB.x >= bB.x && nB.y >= bB.y && nBx2 <= bBx2 && nBy2 <= bBy2 )
            {
                it3->second = it1->first; //< register new boundary to cumulated boundary
                newBoundaryInside = true;
            }
        }
        // remove current "old" boundary, if ...
        if( newBoundaryInside == false      // ... no proper new one was found, or ...
         || ( x <= 3 && y <= 3 && currentImage.size().width - width <= 3 && currentImage.size().height - height <= 3 ) ) //< ... boundary is too big
        {
            m_calculatedBoundaries.erase( it1++ );
        }
        else //< set new area of boundary
        {
            unsigned int x1 = currentImage.size().width;
            unsigned int x2 = 0;
            unsigned int y1 = currentImage.size().height;
            unsigned int y2 = 0;

            for( it3 = boundariesChecked.begin(); it3 != boundariesChecked.end(); ++it3 )
            {
                if( it3->second == it1->first )
                {
                    unsigned int newX1 = it3->first.x;
                    unsigned int newX2 = it3->first.x + it3->first.width;
                    unsigned int newY1 = it3->first.y;
                    unsigned int newY2 = it3->first.y + it3->first.height;
                    x1 = ( newX1 < x1 ) ? newX1 : x1;
                    x2 = ( newX2 > x2 ) ? newX2 : x2;
                    y1 = ( newY1 < y1 ) ? newY1 : y1;
                    y2 = ( newY2 > y2 ) ? newY2 : y2;
                }
            }
            unsigned int width = x2 - x1;
            unsigned int height = y2 - y1;
            width = ( width > 0 ) ? width : 0;
            height = ( height > 0 ) ? height : 0;
            cv::Rect newBound = cv::Rect( x1, y1, width, height );

            m_calculatedBoundaries.at( it1->first ) = newBound;
			++it1;
        }
    }
    // insert all non-checked to m_calculatedBoundaries
    for( it3 = boundariesChecked.begin(); it3 != boundariesChecked.end(); ++it3 )
    {
        if( it3->second == false )
        {
            // get first unused id
            unsigned int id = 0;

            for( std::map<int, cv::Rect>::iterator it = m_calculatedBoundaries.begin(); it != m_calculatedBoundaries.end() && id == it->first; ++it, ++id )
            {
                m_calculatedBoundaries.insert( std::make_pair( id,it3->first ) );
            }
        }
    }

}
