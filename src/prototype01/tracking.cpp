#include "tracking.h"
#include <stdio.h>
#include <sstream>

using namespace cv;

Tracking::Tracking() : 
	learning_rate(0.1),
	mog2BackgoundSubstractor(new BackgroundSubtractorMOG2()),
	bgsubstr(myBGSubMog2()),
    m_contours(std::vector< std::vector<Point> >()),
    prevFrame_gray(Mat()),
    cornersToTrack(std::vector<Point2f>()),
    trackedCorners(std::vector<Point2f>()),
    maxCorners(30),
    qualityLevel(0.1f),
    minDistance(20),
    subPixWinSize(Size (10,10)),
    winSize(Size(20,20)),
    boundingBoxDirections(std::vector<Vec2f>())
{
	bgsubstr.setNMixtures(10);
	bgsubstr.setBackgroundRatio(0.9);
	bgsubstr.setHistorySize(45);
	bgsubstr.setfTau(0.5);
	bgsubstr.setVarThresholdGen(55);
	bgsubstr.setVarThreshold(4);
	bgsubstr.setfVarInit(60);
	bgsubstr.setfVarMin(50);
	bgsubstr.setfVarMax(70);
}

// ##########################
// ####     private     #####
// ##########################


Tracking::TrackingPoint::TrackingPoint() :
    color(Scalar(0,0,0))
{

}

Tracking::TrackingPoint::TrackingPoint(Point2f p_point) :
    color(Scalar(0,0,0))
{
    this->x = p_point.x;
    this->y = p_point.y;
}

void Tracking::findBigBlobs(InputOutputArray image, double thresh)
{
    std::vector<std::vector <Point> > contours;
    std::vector<Vec4i> hierarchy;
    std::vector<int> small_blobs;
	double contour_area;

	// find all contours in the binary image
	findContours(image, contours, hierarchy, CV_RETR_CCOMP,
													  CV_CHAIN_APPROX_SIMPLE);

	// Find indices of contours whose area is less than `threshold` 
	if ( !contours.empty()) {
		for (size_t i=0; i<contours.size(); ++i) {
			contour_area = contourArea(contours[i]) ;
			if ( contour_area < thresh)
				small_blobs.push_back(i);
			//cv::drawContours(image, contours, i, cv::Scalar(255,255,255), 
													 //CV_FILLED, 8);
		}
	}

	// fill-in all small contours with zeros
	for (size_t i=0; i < small_blobs.size(); ++i) {
		cv::drawContours(image, contours, small_blobs[i], cv::Scalar(0), 
													 CV_FILLED, 8);
	}
}

void Tracking::Dilation(InputArray src, OutputArray dst, int size,int kernelType, int iterations)
{
	Mat element = getStructuringElement( kernelType,
						Size( 2*size + 1, 2*size+1 ),
						Point( size, size ) );
	/// Apply the dilation operation 'iterations' times
	for(int i = 0; i<iterations;++i)
	{
		if(i < 1)
		{
			dilate(src, dst, element);
		}
		else
		{
			dilate(dst, dst, element);
		}
	}
}

void Tracking::Erosion(InputArray src, OutputArray dst, int size,int kernelType, int iterations)
{
	Mat element = getStructuringElement( kernelType,
						Size( 2*size + 1, 2*size+1 ),
						Point( size, size ) );
	/// Apply the erosion operation 'iterations' times
	for(int i = 0; i<iterations;++i)
	{
		if(i < 1)
		{
			erode(src, dst, element);
		}
		else
		{
			erode(dst, dst, element);
		}
	}
}

void Tracking::calcObjectContours(Mat& binaryIn)
{
	m_contours.clear();
	vector<Vec4i> hierarchy;
	findContours(binaryIn,m_contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
}

void Tracking::calcBoundingBoxes( Mat& binaryIn )
{
	boundingBoxes.clear();
	/*vector<Vec4i> hierarchy;
    vector<vector <Point> > contours;
	findContours(binaryIn,contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);*/
	calcObjectContours(binaryIn);
	for(size_t i = 0; i < m_contours.size(); ++i)
	{
		boundingBoxes.push_back(boundingRect(m_contours[i]));
	}
}

void Tracking::drawBoundingBoxes( Mat& drawOnImage )
{
	//## display parameters
	const Scalar boundingBoxColor = Scalar(0,255,0);
	const int boundingBoxThickness = 4;
	for(std::vector<Rect>::const_iterator it = boundingBoxes.begin(); it != boundingBoxes.end(); ++it)
	{
        rectangle(drawOnImage,*it,boundingBoxColor,boundingBoxThickness);
	}
}

void Tracking::drawContours( Mat& drawOnImage )
{
    // ## display parameters
	const Scalar contourColor = Scalar(0,0,255);
	const int contourThickness = 2;
	cv::drawContours(drawOnImage, m_contours, -1, contourColor, contourThickness);
}

// !!! HIGHLY INEFFICIENT AND DANGEROUS
void Tracking::discardNotMovingCorners()
{
    std::vector<TrackingPoint> filteredTrackedCorners = std::vector<TrackingPoint>();
    std::vector<TrackingPoint> filteredCornersToTrack = std::vector<TrackingPoint>();
    bool survivedCorners[30] = { false };
    // NAIV implementation
    for( size_t i = 0; i < boundingBoxes.size(); ++i )
    {
        std::vector< Point2f >::iterator toTrackIt = cornersToTrack.begin();
        size_t loopIndex = 0;
        for( std::vector< Point2f >::iterator trackedIt = trackedCorners.begin(); trackedIt != trackedCorners.end(); ++trackedIt)
        {
            if(boundingBoxes[i].contains(*trackedIt))
            {
                if(!survivedCorners[loopIndex])
                {
                    filteredTrackedCorners.push_back(*trackedIt);
                    filteredCornersToTrack.push_back(*toTrackIt);
                    survivedCorners[loopIndex] = true;
                }
            }
            ++toTrackIt;
            ++loopIndex;
        }
    }
    trackedCorners.assign(filteredTrackedCorners.begin(), filteredTrackedCorners.end());
    cornersToTrack.assign(filteredCornersToTrack.begin(), filteredCornersToTrack.end());
}

void Tracking::track_LK( InputArray gray, InputArray mask )
{

    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
    std::vector<uchar> status;
    std::vector<float> err;

    // formerly tracked corners should now be tracked again
    std::swap(trackedCorners,cornersToTrack);
    // make shure only the currently tracked corners are recognized
    trackedCorners.clear();

    if(cornersToTrack.size() <= 0.6 * maxCorners)
    {
        std::vector<Point2f> newCorners = std::vector<Point2f>();
        // ## find features to track with LK optFlow
        goodFeaturesToTrack(gray,newCorners, maxCorners - cornersToTrack.size(),
                            qualityLevel, minDistance,mask,3,true,0.04);
        if(!newCorners.empty())
        {
            cornerSubPix(gray, newCorners, subPixWinSize, Size(-1,-1), termcrit);
        }
        cornersToTrack.insert(cornersToTrack.end(), newCorners.begin(), newCorners.end());
    }
    if(prevFrame_gray.empty())
    {
        prevFrame_gray = gray.getMat().clone();
    }
    if(!cornersToTrack.empty())
    {
        cv::calcOpticalFlowPyrLK(prevFrame_gray, gray, cornersToTrack, trackedCorners,
                                 status, err, winSize, 3, termcrit);
        std::vector< Point2f >::iterator validTrackedIter = trackedCorners.begin();
        std::vector< Point2f >::iterator allToTrackIter = cornersToTrack.begin();
        std::vector< Point2f >::iterator validToTrackIter = cornersToTrack.begin();
        size_t loopIndex = 0;
        size_t validCount = 0;
        for(std::vector< Point2f >::iterator allTrackedIter = trackedCorners.begin();
            allTrackedIter != trackedCorners.end(); ++allTrackedIter)
        {
            if(status[loopIndex])
            {
                *validTrackedIter = *allTrackedIter;
                *validToTrackIter = *allToTrackIter;
                ++validTrackedIter;
                ++validToTrackIter;
                ++validCount;
            }
            ++loopIndex;
            ++allToTrackIter;
        }
        trackedCorners.resize(validCount);
        cornersToTrack.resize(validCount);
        discardNotMovingCorners();
    }
}

void Tracking::trackFarneback(InputArray gray)
{
    if(prevFrame_gray.empty())
    {
        prevFrame_gray = gray.getMat().clone();
    }
    calcOpticalFlowFarneback(prevFrame_gray, gray, m_flow_Farneback,0.1,2,6,4,7,1.1,0);
    cv::Mat xy[2]; //X,Y
    cv::split(m_flow_Farneback, xy);

    boundingBoxDirections.clear();
    for(std::vector<Rect>::iterator boundInter = boundingBoxes.begin(); boundInter != boundingBoxes.end(); ++ boundInter)
    {
        Mat subMatX = xy[0](*boundInter);
        Mat subMatY = xy[1](*boundInter);
        Scalar meanX = mean(subMatX);
        Scalar meanY = mean(subMatY);
        boundingBoxDirections.push_back(Vec2f(meanX.val[0],meanY.val[0]));
    }
}

// #########################
// ####     public      ####
// #########################

std::vector<cv::Rect> Tracking::getBoundariesOfMovement()
{
	if(m_fileLoader == NULL)
	{
		std::cerr << "No FileLoader assinged to Tracker";
		return vector<cv::Rect>();
	}
	else
	{
		currentFrame = m_fileLoader->getCurrentImage();
	}
	//########## declarations ################
	Mat gray;
	cvtColor(currentFrame,gray,CV_BGR2GRAY);

	//declare eroded and opened masks
	eroded = Mat(currentFrame.size(),CV_8UC1);
	opened = Mat(currentFrame.size(), CV_8UC1);

	//########  mask generation and refinement  ###############
    //update the background model and generate mask
	//mog2BackgoundSubstractor->operator()(currentFrame, foregroundMask, learning_rate);
	bgsubstr(currentFrame, foregroundMask, learning_rate);

	int erosion_size = 5;
	int morphIterations = 1;//4;

	//## Opening to refine mask
	//Erosion(fgMaskMOG2,eroded,erosion_size,MORPH_RECT);
	//imshow(windowName_erode,eroded);
	/*Dilation(eroded,opened,erosion_size,MORPH_ELLIPSE,morphIterations);
	Erosion(eroded,opened,erosion_size,MORPH_ELLIPSE,morphIterations);*/
		
	//opened = foregroundMask.clone();
	//findBigBlobs(opened);
	//Erosion(fgMaskMOG2,eroded,1,MORPH_RECT);
	medianBlur(foregroundMask, eroded,7);
	Dilation(eroded,opened,erosion_size,MORPH_RECT,morphIterations);
	Erosion(opened,opened,erosion_size,MORPH_RECT,morphIterations);
	findBigBlobs(opened,110);
	

	calcBoundingBoxes(opened);

	return boundingBoxes;
}

void Tracking::displayDebugWindows()
{
	const string windowName_Frame = "Frame";
	const string windowName_MOG2 = "FG Mask MOG 2";
	const string windowName_background = "background";
	const string windowName_erode = "eroded";
	const string windowName_debugDrawings= "DebugDrawings";

	//declare Mat for Background reference
	Mat bgImage = Mat(currentFrame.size().height,currentFrame.size().width,CV_64F, cvScalar(0.));
	//## get Backgroundimage and show results
	//mog2BackgoundSubstractor->getBackgroundImage(bgImage);
	bgsubstr.getBackgroundImage(bgImage);
	//## ## debugDrawings
	Mat debugImage = currentFrame.clone();
	//	##BoundingBoxes
    drawBoundingBoxes(debugImage);
	drawContours(debugImage);

    namedWindow(windowName_debugDrawings, CV_WINDOW_KEEPRATIO);
    imshow(windowName_debugDrawings, debugImage);

	//string foldername = "debug6";
	//string imagename = "frame_";
	//string fileformat = ".jpg";
	//string folderCreateCommand = "mkdir " + foldername;
	//system(folderCreateCommand.c_str());

	//std::stringstream ss;

	//ss  << foldername << "/" << imagename << m_fileLoader->getSequencePosition() << fileformat;
	//string path = ss.str();
	//ss.str("");
	//imwrite(path,debugImage);

	//namedWindow("fgMask", CV_WINDOW_KEEPRATIO);
	//imshow("fgMask", foregroundMask);

	//namedWindow(windowName_background,CV_WINDOW_KEEPRATIO);
	//imshow(windowName_background,bgImage);

	//namedWindow(windowName_erode,CV_WINDOW_KEEPRATIO);
	//imshow(windowName_erode,eroded);

}

void Tracking::setFileLoader( FileLoader* fileLoader )
{
	m_fileLoader = fileLoader;
}

