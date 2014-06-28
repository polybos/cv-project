#include "trackingreforming.h"

#include <vector>
#include <set>

#include<iostream>

TrackingReforming::TrackingReforming()
    :m_fileLoader()
    ,m_calculatedBoundaries()
	,m_calculatedClasses()
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
		m_calculatedBoundaries.clear();
        return;
    }
    int id = 0;
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

void TrackingReforming::getNewClasses(std::vector< std::pair<cv::Rect, TrafficClass> >& classificationResults)
{
	const int MIN_CLASSRECONGNITIONS = 3;

	 std::map<int, cv::Rect>::iterator boundaryIt;
	 int vectorIdx = 0;
	 for(boundaryIt = m_calculatedBoundaries.begin(); boundaryIt != m_calculatedBoundaries.end(); ++boundaryIt)
	 {
		 int id = boundaryIt->first;
		 //std::cout << "id: " << id << std::endl;
		 /*cv::Vec3i classCount = m_calculatedClasses.at(id);*/
		 
		 switch ( classificationResults[vectorIdx].second )
        {
        case car:
			m_calculatedClasses.at(id)[0] += 1;
            break;
        case human:
            m_calculatedClasses.at(id)[1] += 1;
            break;
        case bicycle:
            m_calculatedClasses.at(id)[2] += 1;
            break;
        default:
            break;
        }

		 // find the class the rect got most identified with
		int bestClassIdx = 0;
		for(int i = 0; i<3;++i)
		{
			if(m_calculatedClasses.at(id)[i] > m_calculatedClasses.at(id)[bestClassIdx])
			{
				bestClassIdx = i;
			}
		}

		// only change class, when at least MIN_CLASSRECOGNITIONS with this class were reported for this rect
		if(m_calculatedClasses.at(id)[bestClassIdx] < MIN_CLASSRECONGNITIONS)
		{
			++vectorIdx;
			continue;
		}
		  
		switch ( bestClassIdx )
        {
        case 0:
			classificationResults[vectorIdx].second = car;
            break;
        case 1:
			classificationResults[vectorIdx].second = human;
            break;
        case 2:
			classificationResults[vectorIdx].second = bicycle;
            break;
        default:
            break;
        }
		++vectorIdx;
	 }
}

// ---------------
// --- private ---


cv::Rect TrackingReforming::enlargeRect(cv::Rect oldBound, int paddingX, int paddingY)
{
	cv::Mat currentImage = m_fileLoader->getCurrentImage();
    
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

    return cv::Rect( x, y, width, height );
}


cv::Rect TrackingReforming::enlargeRect(cv::Rect oldBound)
{
    int paddingX = oldBound.width  * 0.60;
    int paddingY = oldBound.height * 0.1;

	return enlargeRect(oldBound, paddingX, paddingY);
}


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
	
	
	// ## DEBUG
	cv::Mat debugImage = currentImage.clone();




    for( it1 = m_calculatedBoundaries.begin(); it1 != m_calculatedBoundaries.end(); )
    {
        cv::Rect oldBound = it1->second;

        // calculate position and size for bigger border

        cv::Rect biggerBoundary = enlargeRect(oldBound);
        cv::Rect bB = biggerBoundary;

		// ## DEBUG
		cv::rectangle(debugImage,bB,cv::Scalar(180,0,180),4);
        std::string idSTR = cv::format("%i", it1->first);
        cv::putText(debugImage, idSTR, bB.tl(),0, (double)4/3.0, cv::Scalar(180,0,180), 4/1.25);


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

				// ## DEBUG
				cv::rectangle(debugImage, nB,cv::Scalar(255,255,0),2);


                for(int boundsIdx = 0; boundsIdx < boundariesChecked.size(); ++boundsIdx)
                {
                    if(boundariesChecked[boundsIdx].second == it1->first)
                    {
                        cv::Point thisTL = cv::Point(nB.x,nB.y);
                        cv::Point thisBR = cv::Point(nBx2,nBy2);
                        cv::Rect boxInSame = boundariesChecked[boundsIdx].first;
                        cv::Point boxInSameTR= boxInSame.tl() + cv::Point(boxInSame.width,0);
                        cv::Point boxInSameBL = boxInSame.br() - cv::Point(boxInSame.width,0);

                        cv::Rect maxOutlieArea = enlargeRect(it3->first,120,90);

						//cv::rectangle(debugImage, maxOutlieArea, cv::Scalar(0,0,255),2);

                        bool tlIn = maxOutlieArea.contains(boxInSame.tl());
                        bool trIn = maxOutlieArea.contains(boxInSameTR);
                        bool blIn = maxOutlieArea.contains(boxInSameBL);
                        bool brIn = maxOutlieArea.contains(boxInSame.br());


                        if(!(tlIn || trIn || blIn || brIn))
                        {
                            int newId;
                            for(newId = 0; m_calculatedBoundaries.count(newId); ++newId)
                            {}
                            m_calculatedBoundaries.insert( std::make_pair( newId,it3->first ) );
                            m_calculatedClasses.insert(std::make_pair( newId, cv::Vec3i(0,0,0) ) );
                            it3->second = newId;
                        }


                    }
                }

				// ## DEBUG
				cv::rectangle(debugImage, nB,cv::Scalar(110,110,0),2);
            }
        }
        // remove current "old" boundary, if ...
        if( newBoundaryInside == false      // ... no proper new one was found, or ...
         || ( biggerBoundary.x <= 3 && biggerBoundary.y <= 3 && currentImage.size().width - biggerBoundary.width <= 3 && currentImage.size().height - biggerBoundary.height <= 3 ) ) //< ... boundary is too big
        {
			m_calculatedClasses.erase(it1->first);
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

			cv::rectangle(debugImage,newBound,cv::Scalar(255,0,0),2);

            m_calculatedBoundaries.at( it1->first ) = newBound;
			++it1;
        }
    }
    // insert all non-checked to m_calculatedBoundaries
    for( it3 = boundariesChecked.begin(); it3 != boundariesChecked.end(); ++it3 )
    {
        if( it3->second < 0 )
        {
            // get first unused id
            unsigned int id = 0;

            while(m_calculatedBoundaries.count(id) > 0)
            {
                id++;
            }
            m_calculatedBoundaries.insert( std::make_pair( id,it3->first ) );
            m_calculatedClasses.insert(std::make_pair( id, cv::Vec3i(0,0,0) ) );

			// ## DEBUG
			cv::rectangle(debugImage,it3->first,cv::Scalar(10,10,10),3);
            std::string idStr = cv::format("%i",id);
			cv::putText(debugImage,idStr, (it3->first).tl(), 0, (double)4/3.0, cv::Scalar(10,10,10), 4/1.25);
        }
    }

    // look for boundary inside another boundary
    std::map<int, cv::Rect>::iterator it1_2;
    std::set<int> deleteKeys;
    for( it1 = m_calculatedBoundaries.begin(); it1 != m_calculatedBoundaries.end() && it1 != --m_calculatedBoundaries.end() ; ++it1 )
    {
        cv::Rect bound1 = it1->second;
        it1_2 = it1;
        for( ++it1_2; it1_2 != m_calculatedBoundaries.end(); ++it1_2 )
        {
            cv::Rect bound2 = it1_2->second;

            if( bound1.contains( bound2.tl() ) && bound1.contains( bound2.br() ) )
            {
                deleteKeys.insert( it1_2->first );
            }
            else
            {
                if( bound2.contains( bound1.tl() ) && bound2.contains( bound1.br() ) )
                {
                    deleteKeys.insert( it1->first );
                }
            }
        }
    }

    std::set<int>::iterator delIt;
    for( delIt = deleteKeys.begin(); delIt != deleteKeys.end(); ++delIt )
    {
        int index = *delIt;
        m_calculatedBoundaries.erase( index );
    }

	// ## DEBUG
    //cv::imshow("bBdebug", debugImage);


}

void TrackingReforming::drawDebugBigBox(cv::Rect bigBox, cv::Mat& image)
{

}

void TrackingReforming::showDebug(cv::Mat image)
{

}
