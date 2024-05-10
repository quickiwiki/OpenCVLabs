#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <utility>

enum class ActionType
{
    Histogram,
    Equalizer,
    Lomo,
    Cartoon
};

static const int count = 100;
static const int maxColumns = 256;
class ImageWindow
{
    std::string name;
    cv::Mat image;
    int filterValue = 0;

    static void onTrackbar(int pos, void *userdata)
    {
        if (pos <= 0)
            return;

        auto window = static_cast<ImageWindow *>(userdata);
        window->applyBasicFilterValue(pos);
    };

    static void onMouse(int event, int x, int y, [[maybe_unused]] int flags, void *userInput)
    {
        if (event != cv::EVENT_LBUTTONDOWN)
            return;

        auto window = static_cast<ImageWindow *>(userInput);
        cv::circle(window->image, cv::Point(x, y), 10, cv::Scalar(0, 255, 0), 3);


        if (window->filterValue)
            window->applyBasicFilterValue(window->filterValue);
        else
            window->show();
    };

    template<ActionType T>
    static void onClick([[maybe_unused]] int state, void *userData)
    {
        auto window = static_cast<ImageWindow *>(userData);
        window->action(T);
    };

    void action(ActionType actionType)
    {

        switch (actionType)
        {

            case ActionType::Histogram:
                showHistogram();
                break;
            case ActionType::Equalizer:
                equalizeImage();
                break;
            case ActionType::Lomo:
                lomo();
                break;
            case ActionType::Cartoon:
                break;
        }
    }

    void showHistogram()
    {
        std::vector<cv::Mat> bgrPlanes;

        cv::split(image, bgrPlanes);

        int bins = maxColumns;
        const std::array<float, 2> range = {0, maxColumns};
        const float *histRange = {range.data()};
        cv::Mat blueHistogram, greenHistogram, redHistogram;

        cv::Mat mat;
        cv::calcHist(&bgrPlanes[0], 1, nullptr, cv::Mat(), blueHistogram, 1, &bins, &histRange);
        cv::calcHist(&bgrPlanes[1], 1, nullptr, cv::Mat(), greenHistogram, 1, &bins, &histRange);
        cv::calcHist(&bgrPlanes[2], 1, nullptr, cv::Mat(), redHistogram, 1, &bins, &histRange);

        int width = 512;
        int height = 300;

        cv::Mat histImage(height, width, CV_8UC3, cv::Scalar(20, 20, 20));

        cv::normalize(blueHistogram, blueHistogram, 0, height, cv::NORM_MINMAX);
        cv::normalize(greenHistogram, greenHistogram, 0, height, cv::NORM_MINMAX);
        cv::normalize(redHistogram, redHistogram, 0, height, cv::NORM_MINMAX);

        auto binWidth = cvRound(static_cast<float>(width) / static_cast<float>(bins));
        for (int i = 1; i < bins; ++i)
        {
            cv::line(histImage, cv::Point(binWidth * (i - 1), height - cvRound(blueHistogram.at<float>(i - 1))),
                     cv::Point(binWidth * (i), height - cvRound(blueHistogram.at<float>(i))),
                     cv::Scalar(255, 0, 0), 2, 8, 0);
            cv::line(histImage, cv::Point(binWidth * (i - 1), height - cvRound(greenHistogram.at<float>(i - 1))),
                     cv::Point(binWidth * (i), height - cvRound(greenHistogram.at<float>(i))),
                     cv::Scalar(0, 255, 0), 2, 8, 0);
            cv::line(histImage, cv::Point(binWidth * (i - 1), height - cvRound(redHistogram.at<float>(i - 1))),
                     cv::Point(binWidth * (i), height - cvRound(redHistogram.at<float>(i))),
                     cv::Scalar(0, 0, 255), 2, 8, 0);
        }

        cv::imshow(name + ' ' + "Histogram", histImage);
    }

    void equalizeImage()
    {
        cv::Mat result, ycrcb;
        cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
        std::vector<cv::Mat> channels;
        cv::split(ycrcb, channels);
        cv::equalizeHist(channels[0], channels[0]);
        cv::merge(channels, ycrcb);
        cv::cvtColor(ycrcb, result, cv::COLOR_YCrCb2BGR);

        cv::imshow(name + ' ' + "Equalized", result);
    }

    void lomo()
    {
        cv::Mat result;

        const double exp_e = std::exp(1.);

        cv::Mat lut(1, maxColumns, CV_8UC1);// 1, 256, CV_8UC1};


        for (int i = 0; i < maxColumns; ++i)
        {
            auto x = static_cast<float>(i) / maxColumns;
            lut.at<uchar>(i) = cvRound(maxColumns * (1 / (1 + std::pow(exp_e, -(x - 0.5) / 0.1))));
        }

        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        cv::LUT(channels[2], lut, channels[2]);

        cv::merge(channels, result);

        const auto cols = image.cols;
        const auto rows = image.rows;
        cv::Mat halo{rows, cols, CV_32FC3, cv::Scalar{0.3, 0.3, 0.3}};

        cv::circle(halo, cv::Point{cols / 2, rows / 2}, cols / 3, cv::Scalar{1, 1, 1}, -1);
        cv::blur(halo, halo, cv::Size{cols / 3, cols / 3});
        cv::Mat resultf;

        result.convertTo(resultf, CV_32FC3);

        cv::multiply(resultf, halo, resultf);

        resultf.convertTo(result, CV_8UC3);

        cv::imshow(name + ' ' + "Lomography", result);
    }

public:
    using UniPtr = std::unique_ptr<ImageWindow>;
    ImageWindow(std::string windowName, cv::Mat windowImage, int flags) : name(std::move(windowName)), image(std::move(windowImage))
    {
        cv::namedWindow(name, flags);

        cv::createTrackbar(name, name, nullptr, count, onTrackbar, this);
        cv::setMouseCallback(name, onMouse, this);

        static bool panelInitialized = [this]()
        {
            cv::createButton("Show histogram", onClick<ActionType::Histogram>, this, cv::QT_PUSH_BUTTON, false);
            cv::createButton("Equalize histogram", onClick<ActionType::Equalizer>, this, cv::QT_PUSH_BUTTON, false);
            cv::createButton("Lomography effect", onClick<ActionType::Lomo>, this, cv::QT_PUSH_BUTTON, false);
            cv::createButton("Cartoonize effect", onClick<ActionType::Cartoon>, this, cv::QT_PUSH_BUTTON, false);

            return true;
        }();
    }

    ~ImageWindow()
    {
        if (!getName().empty())
            cv::destroyWindow(name);
    }

    ImageWindow(ImageWindow &&window) = delete;
    ImageWindow &operator=(ImageWindow &&window) = delete;
    ImageWindow(ImageWindow const &window) = delete;
    ImageWindow &operator=(ImageWindow const &window) = delete;

    [[nodiscard]] std::string getName() const
    {
        return name;
    }

    void applyBasicFilterValue(int value)
    {
        if (!value)
            return;

        filterValue = value;

        cv::Mat imgBlur;
        assert(image.data);
        cv::blur(image, imgBlur, cv::Size(value, value));

        cv::imshow(name, imgBlur);
    }


    void show() const
    {
        cv::imshow(name, image);
    }

    void show(cv::Mat &img)
    {
        cv::imshow(name, img);
    }

    void move(int x, int y) const
    {
        cv::moveWindow(name, x, y);
    }
};

int main(int argc, char **argv)
{
    const char *keys = {
            "{help h usage? || print  this message}"
            "{@files | <none> | Image file list }"};

    cv::CommandLineParser parser{argc, argv, keys};

    if (parser.has("help") || !argc)
    {
        parser.printMessage();
        return 0;
    }

    if (!parser.check() || argc < 2)
    {
        parser.printErrors();
        return 0;
    }
    const std::filesystem::path filePath(argv[1]);
    std::cout << filePath << std::endl;
    auto image = cv::imread(filePath);
    if (!image.data)
    {
        std::cerr << filePath << " image dismissing!\n";
        return -1;
    }

    ImageWindow window{filePath.filename(), image, cv::WINDOW_AUTOSIZE};

    window.show();

    cv::waitKey(0);

    return 0;
}
