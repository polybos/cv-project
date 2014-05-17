#include "tracking.h"
#include <stdio.h>

using namespace cv;

Tracking::Tracking() : 
	learning_rate(0.1),
	mog2BackgoundSubstractor(new BackgroundSubtractorMOG2(100,16,false))
{
}

void Tracking::findBigBlobs(InputOutputArray image, double thresh)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<int> small_blobs;
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
			drawContours(image, contours, i, cv::Scalar(255,255,255), 
													 CV_FILLED, 8);
		}
	}

	// fill-in all small contours with zeros
	for (size_t i=0; i < small_blobs.size(); ++i) {
		drawContours(image, contours, small_blobs[i], cv::Scalar(0), 
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

void Tracking::calcBoundingBoxes( Mat& binaryIn )
{
	boundingBoxes.clear();
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours;
	findContours(binaryIn,contours,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for(size_t i = 0; i < contours.size(); ++i)
	{
		boundingBoxes.push_back(boundingRect(contours[i]));
	}
}

void Tracking::drawBoundingBoxes( Mat& drawOnImage )
{
	const Scalar boundingBoxColor = Scalar(0,255,0);
	const int boundingBoxThickness = 4;
	for(std::vector<Rect>::const_iterator it = boundingBoxes.begin(); it != boundingBoxes.end(); ++it)
	{
		rectangle(drawOnImage,*it,boundingBoxColor,boundingBoxThickness);
	}
}

//############	public	#####################

void Tracking::track_LK( InputArray gray, InputArray mask )
{
	const int maxCorners = 15;
	const float qualityLevel = 0.001;
	const float minDistance = 20;
	//## find features to track with LK optFlow
	goodFeaturesToTrack(gray,cornersToTrack, maxCorners, qualityLevel, minDistance,mask,3,false,0.04);
}


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
	//update the background model
	mog2BackgoundSubstractor->operator()(currentFrame, foregroundMask, learning_rate);

	int erosion_size = 2;
	int morphIterations = 4;

	//## Opening to refine mask
	//Erosion(fgMaskMOG2,eroded,erosion_size,MORPH_RECT);
	//imshow(windowName_erode,eroded);
	/*Dilation(eroded,opened,erosion_size,MORPH_ELLIPSE,morphIterations);
	Erosion(eroded,opened,erosion_size,MORPH_ELLIPSE,morphIterations);*/
		
	opened = foregroundMask.clone();
	//findBigBlobs(opened);
	//Erosion(fgMaskMOG2,eroded,1,MORPH_RECT);
	medianBlur(foregroundMask, eroded,3);
	findBigBlobs(eroded,110);
	Dilation(eroded,opened,erosion_size,MORPH_ELLIPSE,morphIterations);
	Erosion(opened,opened,erosion_size,MORPH_ELLIPSE,morphIterations);

	calcBoundingBoxes(opened);

	//########### Tracking ################
	track_LK(gray,opened);

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
	mog2BackgoundSubstractor->getBackgroundImage(bgImage);

	//## ## debugDrawings
	Mat debugImage = currentFrame.clone();
	//	##BoundingBoxes
	drawBoundingBoxes(debugImage);
		
	//	## Corners
	for( size_t i = 0; i < cornersToTrack.size(); i++ )
	{
		circle( debugImage, cornersToTrack[i], 10, Scalar( 255. ), -1 );
	}

	namedWindow(windowName_debugDrawings, CV_WINDOW_KEEPRATIO);
	imshow(windowName_debugDrawings, debugImage);


	//## show the current frame,the fg masks and background Image
	namedWindow(windowName_Frame);
	imshow(windowName_Frame, currentFrame);
	namedWindow(windowName_MOG2,CV_WINDOW_KEEPRATIO);
	imshow(windowName_MOG2, foregroundMask);
	namedWindow(windowName_background,CV_WINDOW_KEEPRATIO);
	imshow(windowName_background,bgImage);

	namedWindow(windowName_erode,CV_WINDOW_KEEPRATIO);
	imshow(windowName_erode,eroded);


	//## ## Debugwindows for morphilogic operations	
	/*const char* windowName_erode = "erded";
	namedWindow(windowName_erode,CV_WINDOW_KEEPRATIO);
	const char* windowName_opened = "opened";
	namedWindow(windowName_opened, CV_WINDOW_KEEPRATIO);
	imshow(windowName_opened,opened);
	imshow(windowName_erode,eroded);
	drawBoundingBoxes(opened);*/
}

void Tracking::setFileLoader( FileLoader* fileLoader )
{
	m_fileLoader = fileLoader;
}

