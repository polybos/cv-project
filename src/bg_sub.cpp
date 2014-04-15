/**
 * @file bg_sub.cpp
 * @brief Background subtraction tutorial sample code
 * @author Domenico D. Bloisi
 */

//opencv
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv\cv.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

// Global variables
Mat frame; //current frame
Mat fgMaskMOG; //fg mask generated by MOG method
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Mat bgImage;
Ptr<BackgroundSubtractor> pMOG; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
int keyboard; //input from keyboard

const string windowname_Frame = "Frame";
const string windowname_MOG = "FG Mask MOG";
const string windowname_MOG2 = "FG Mask MOG 2";
const string windowname_background = "background";



const double learning_rate = 0.1;

/** Function Headers */
void help();
void processVideo(char* videoFilename);
void processImages(char* firstFrameFilename);

void help()
{
    cout
    << "--------------------------------------------------------------------------" << endl
    << "This program shows how to use background subtraction methods provided by "  << endl
    << " OpenCV. You can process both videos (-vid) and images (-img)."             << endl
                                                                                    << endl
    << "Usage:"                                                                     << endl
    << "./bs {-vid <video filename>|-img <image filename>}"                         << endl
    << "for example: ./bs -vid video.avi"                                           << endl
    << "or: ./bs -img /data/images/1.png"                                           << endl
    << "--------------------------------------------------------------------------" << endl
    << endl;
}

/**
 * @function main
 */
int main(int argc, char* argv[])
{
    //print help information
    help();

    //check for the input parameter correctness
    if(argc != 3) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }

	int windowHeight = 350;
	int windowWidth = 900;

    //create GUI windows
	namedWindow(windowname_Frame,CV_WINDOW_NORMAL);
	resizeWindow(windowname_Frame,windowWidth,windowHeight);
	namedWindow(windowname_MOG,CV_WINDOW_NORMAL);
	resizeWindow(windowname_MOG,windowWidth,windowHeight);
    namedWindow(windowname_MOG2,CV_WINDOW_NORMAL);
	resizeWindow(windowname_MOG2,windowWidth,windowHeight);
	namedWindow(windowname_background,CV_WINDOW_NORMAL);
	resizeWindow(windowname_background,windowWidth,windowHeight);

    //create Background Subtractor objects
	pMOG = new BackgroundSubtractorMOG(); //MOG approach
	pMOG2 = new BackgroundSubtractorMOG2(); //MOG2 approach

    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);
    }
    else if(strcmp(argv[1], "-img") == 0) {
        //input data coming from a sequence of images
        processImages(argv[2]);
    }
    else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}

/**
 * @function processVideo
 */
void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture(videoFilename);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //update the background model
		pMOG->operator()(frame, fgMaskMOG, learning_rate);
		pMOG2->operator()(frame, fgMaskMOG2, learning_rate);
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        ss << capture.get(CV_CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
		pMOG->getBackgroundImage(bgImage);
        //show the current frame and the fg masks
		imshow(windowname_background, frame);
		imshow(windowname_MOG, fgMaskMOG);
		imshow(windowname_MOG2, fgMaskMOG2);
		imshow(windowname_background,bgImage);
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}

/**
 * @function processImages
 */

int numDigits(int number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

string createPrefix(int frameNr)
{
	int filenameSize = 10;
	stringstream oss;
	oss << frameNr;
	string output = oss.str();
	while(output.length() < filenameSize)
	{
		output = '0' + output;
	}
	return output;
}

//void generate_Framenumber(const char* filename, int* frameNumber, string* nextFilename)
//{
//	string fn(filename);
//	size_t index = fn.find_last_of("/");
//    if(index == string::npos) {
//        index = fn.find_last_of("\\");
//    }
//    size_t index2 = fn.find_last_of(".");
//    string prefix = fn.substr(0,index+1);
//    string suffix = fn.substr(index2);
//    string frameNumberString = fn.substr(index+1, index2-index-1);
//    istringstream iss(frameNumberString);
//    int frameNumberI = 0;
//    iss >> *frameNumber;
//	ostringstream oss;
//    oss << (frameNumber + 1);
//	string nextFrameNumberString = /*oss.str();*/ createPrefix(*frameNumber+1);
//    string nextFrameFilename = prefix + nextFrameNumberString + suffix;
//	nextFilename = nextFrameFilename.c_str();
//}
//
//string generate_nextFilename(int frameNumber)
//{
//	
//}

void processImages(char* fistFrameFilename) {
    //read the first file of the sequence
    frame = imread(fistFrameFilename);
    if(!frame.data){
        //error in opening the first image
        cerr << "Unable to open first image frame: " << fistFrameFilename << endl;
        exit(EXIT_FAILURE);
    }
    //current image filename
    string fn(fistFrameFilename);
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //update the background model
		pMOG->operator()(frame, fgMaskMOG, learning_rate);
		pMOG2->operator()(frame, fgMaskMOG2, learning_rate);
        //get the frame number and write it on the current frame
        size_t index = fn.find_last_of("/");
        if(index == string::npos) {
            index = fn.find_last_of("\\");
        }
        size_t index2 = fn.find_last_of(".");
        string prefix = fn.substr(0,index+1);
        string suffix = fn.substr(index2);
        string frameNumberString = fn.substr(index+1, index2-index-1);
        istringstream iss(frameNumberString);
        int frameNumber = 0;
        iss >> frameNumber;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        pMOG2->getBackgroundImage(bgImage);
        //show the current frame and the fg masks
        imshow("Frame", frame);
        imshow("FG Mask MOG", fgMaskMOG);
        imshow("FG Mask MOG 2", fgMaskMOG2);
		imshow("background",bgImage);
        //get the input from the keyboard
        keyboard = waitKey( 30 );
        //search for the next image in the sequence
        ostringstream oss;
        oss << (frameNumber + 1);
		string nextFrameNumberString = /*oss.str();*/ createPrefix(frameNumber+1);
        string nextFrameFilename = prefix + nextFrameNumberString + suffix;
        //read the next frame
        frame = imread(nextFrameFilename);
        if(!frame.data){
            //error in opening the next image in the sequence
            cerr << "Unable to open image frame: " << nextFrameFilename << endl;
            exit(EXIT_FAILURE);
        }
        //update the path of the current frame
        fn.assign(nextFrameFilename);
    }
}


void OptflowImage(char* filename)
{
	frame = imread(filename);
}
