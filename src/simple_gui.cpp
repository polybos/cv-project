#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <vector>

/* Simple file loading from folder.
 *
 * Argument must be a folder including guarding symbol e.g. "\" or "/".
 */


std::vector<cv::Mat> imgs; //< contains all images


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Missing argument - image folder!\n" << std::endl;
        return 1;
    }


    std::stringstream ss;
    ss << argv[1] << "%010d.png";
    cv::VideoCapture sequence(ss.str());
    if (!sequence.isOpened())
    {
        std::cerr << "Failed to open image sequence!\n" << std::endl;
        return 1;
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

        imgs.push_back( image.clone() );
    }

    //! Show all loaded images
    for( std::vector<cv::Mat>::const_iterator it = imgs.begin(); it != imgs.end(); ++it )
    {
        cv::imshow("image | q or esc to quit", *it);

        char key = (char)cv::waitKey(500);
        if(key == 'q' || key == 'Q' || key == 27)
            break;
    }


    std::cout << "number of images: " << imgs.size() << std::endl;


    return 0;
}
