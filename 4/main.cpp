#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <utility>

static const int count = 100;
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
        window->applyFilter(pos);
    };

    static void onMouse(int event, int x, int y, int flags, void *userInput)
    {
        if (event != cv::EVENT_LBUTTONDOWN)
            return;

        auto window = static_cast<ImageWindow *>(userInput);
        cv::circle(window->image, cv::Point(x, y), 10, cv::Scalar(0, 255, 0), 3);


        if (window->filterValue)
            window->applyFilter(window->filterValue);
        else
            window->show();
    };

public:
    using UniPtr = std::unique_ptr<ImageWindow>;
    ImageWindow(std::string windowName, cv::Mat windowImage, int flags) : name(std::move(windowName)), image(std::move(windowImage))
    {
        cv::namedWindow(name, flags);

        cv::createTrackbar(name, name, nullptr, count, onTrackbar, this);
        cv::setMouseCallback(name, onMouse, this);
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

    void applyFilter(int value)
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

    void move(int x, int y) const
    {
        cv::moveWindow(name, x, y);
    }
};

int main(int argc, char **argv)
{
    const char *keys = {
            "{help h usage? || print  this message}"
            "{files | <none> | Image file list }"};

    cv::CommandLineParser parser{argc, argv, keys};

    if (parser.has("help") || !argc)
    {
        parser.printMessage();
        return 0;
    }

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    std::vector<ImageWindow::UniPtr> windows;

    for (int i = 1; i < argc; ++i)
    {
        const std::filesystem::path filePath(argv[i]);
        std::cout << filePath << std::endl;
        auto image = cv::imread(filePath);
        if (!image.data)
        {
            std::cerr << filePath << " image dismissing!\n";
            return -1;
        }

        windows.emplace_back(std::make_unique<ImageWindow>(filePath.filename(), image, cv::WINDOW_AUTOSIZE));
    }

    for (int i = 0; const auto &window: windows)
    {
        const auto offset = 20 * i++;
        window->move(offset, offset);
        window->show();
    }

    cv::waitKey(0);

    return 0;
}
