#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


int main(int argc, char **argv)
{
    const char *keys = {
            "{help h usage? || print  this message}"
            "{@video || Video file, if not defined try to use web camera}"};

    cv::CommandLineParser parser{argc, argv, keys};

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    auto videoFile = parser.get<cv::String>(0);
    cv::VideoCapture capture;

    if (!videoFile.empty())
        capture.open(videoFile);
    else
        capture.open(0);

    if (!capture.isOpened())
        return -1;

    const std::string windowName = "Video";

    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cv::Mat frame;

        capture >> frame;

        if (!frame.empty())
            cv::imshow(windowName, frame);

        if (cv::waitKey(30) >= 0)
            break;
    }

    cv::waitKey(0);
    cv::destroyWindow(windowName);

    capture.release();

    return 0;
}
