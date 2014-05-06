#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <vector>

/* Classification tests.
 *
 * Argument must be a string including path and fileformat (e.g car%01d.png)
 */


//using namespace std;
using namespace cv;

std::vector<cv::Mat> imgs; //< contains all images

vector<Mat> load_Images(const char* directory)
{
    vector<Mat> out;
    std::stringstream ss;
    ss << directory;
    cv::VideoCapture sequence(ss.str());
    if (!sequence.isOpened())
    {
        std::cerr << "Failed to open image sequence!\n" << std::endl;
        out.clear();
        return out;
    }

    cv::Mat image;

    //! Load all images sequentially into vector imgs
    for(;;)
    {
        sequence >> image;

        if(image.empty())
        {
            std::cout << "End of Sequence" << std::endl;
            break;
        }

        out.push_back( image.clone() );
    }
    return out;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Missing argument - image folder!\n" << std::endl;
        return 1;
    }
    vector<Mat> test = load_Images(argv[1]);


    //! Show all loaded images
    for( std::vector<cv::Mat>::const_iterator it = test.begin(); it != test.end(); ++it )
    {
        cv::imshow("image | q or esc to quit", *it);

        char key = (char)cv::waitKey(4000);
        if(key == 'q' || key == 'Q' || key == 27)
            break;
    }

    //! Detecting play area ...

    //! ?




    return 0;
}

