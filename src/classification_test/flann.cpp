/* flann.cpp
 *
 * simple test with FLANN and two similar images
 *
 * code adpated from http://docs.opencv.org/doc/tutorials/features2d/feature_flann_matcher/feature_flann_matcher.html
 *
 */

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>

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


    if( files.size() < 2 )
    {
        cerr << "path " << argv[1] << " must have at least two files." << endl;
        return 1;
    }

    // first two file-pathes are images containing two cars

    Mat img_1 = imread( files[0], CV_LOAD_IMAGE_GRAYSCALE );
    Mat img_2 = imread( files[7], CV_LOAD_IMAGE_GRAYSCALE );



    if( !img_1.data || !img_2.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_1, keypoints_2;

    detector.detect( img_1, keypoints_1 );
    detector.detect( img_2, keypoints_2 );

    //-- Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;

    Mat descriptors_1, descriptors_2;

    extractor.compute( img_1, keypoints_1, descriptors_1 );
    extractor.compute( img_2, keypoints_2, descriptors_2 );

    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_1.rows; i++ )
    { double dist = matches[i].distance;
      if( dist < min_dist ) min_dist = dist;
      if( dist > max_dist ) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_1.rows; i++ )
    { if( matches[i].distance <= max(2*min_dist, 0.02) )
      { good_matches.push_back( matches[i]); }
    }

    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Show detected matches
    imshow( "Good Matches", img_matches );

    for( int i = 0; i < (int)good_matches.size(); i++ )
    { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }

    waitKey(0);

    return 0;
}
