//*******************surf.cpp******************//
//********** SURF implementation in OpenCV*****//
//**loads video from webcam, grabs frames computes SURF keypoints and descriptors**//  //** and marks them**//

//****author: achu_wilson@rediffmail.com****//

#include <stdio.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;

vector<string> files;


// fills vector @files with all files (paths as string) of given directory
void fillFiles( const char * parentDir )
{
    using namespace boost::filesystem;
    path p ( parentDir );   // p reads clearer than argv[1] in the following code

    try
    {
        if (exists(p))    // does p actually exist?
        {
            if (is_regular_file(p))        // is p a regular file?
            {
                files.push_back( p.string() );
                cout << p << " size is " << file_size(p) << '\n';
            }

            else if (is_directory(p))      // is p a directory?
            {
                cout << p << " is a directory containing:\n";
                directory_iterator end_itr;
                for( directory_iterator itr(p); itr != end_itr; ++itr )
                {
                    if ( exists(itr->path()) )
                    {
                        cout << itr->path().string() << endl;
                        if ( is_regular_file(itr->path()) )
                        {
                            files.push_back( itr->path().string() );
                        }
                        else
                        {
                            fillFiles( itr->path().string().c_str() );
                        }
                    }

                }
            }

            else
                cout << p << " exists, but is neither a regular file nor a directory\n";
        }
        else
            cout << p << " does not exist\n";
    }

    catch (const filesystem_error& ex)
    {
        cout << ex.what() << '\n';
    }

}


int main( int argc, char ** argv )
{

    if( argc >= 2 )
    {
        fillFiles( argv[1] );
    }
    else
    {
        cerr << "usage: " << argv[0] << " PATH_TO_FILES" << endl;
        return 1;
    }


    cv::initModule_nonfree(); // THIS LINE IS IMPORTANT
    static CvScalar red_color[] ={0,0,255};



    for( ;; )
    for( vector<string>::iterator it = files.begin(); it != files.end(); ++it )
    {
        IplImage *image = cvLoadImage( (*it).c_str() );

        CvMemStorage* memoryBlock = cvCreateMemStorage();
        CvSeq* imageKeyPoints;
        CvSeq* imageDescriptors;

        // Only values with a hessian greater than 500 are considered for keypoints
        CvSURFParams params = cvSURFParams(50, 1);
        cvExtractSURF(image, 0, &imageKeyPoints, &imageDescriptors, memoryBlock, params);

        //draw the keypoints on the captured frame
        for( int i = 0; i < imageKeyPoints->total; i++ )
        {
            CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeyPoints, i );
            CvPoint center;
            int radius;
            center.x = cvRound(r->pt.x);
            center.y = cvRound(r->pt.y);
            radius = cvRound(r->size*1.2/9.*2);
            cvCircle( image, center, radius, red_color[0], 1, 8, 0 );
        }
        cvShowImage( "Image", image );

        cvWaitKey(10000);
    }

    cvDestroyWindow("Image");

    return 0;
}
