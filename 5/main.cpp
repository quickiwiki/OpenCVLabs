#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <set>
#include <utility>

enum class FiltersType
{
    Blur,
    Grey,
    RGB,
    Sobel
};

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

    template<FiltersType T>
    static void onClick([[maybe_unused]] int state, void *userData)
    {
        auto window = static_cast<ImageWindow *>(userData);
        auto img = window->applyFilter(T);
        window->show(img);
    };

    std::set<FiltersType> filters;

public:
    using UniPtr = std::unique_ptr<ImageWindow>;
    ImageWindow(std::string windowName, cv::Mat windowImage, int flags) : name(std::move(windowName)), image(std::move(windowImage))
    {
        cv::namedWindow(name, flags);

        cv::createTrackbar(name, name, nullptr, count, onTrackbar, this);
        cv::setMouseCallback(name, onMouse, this);

        static bool panelInitialized = [this]()
        {
            cv::createButton("Blur", onClick<FiltersType::Blur>, this, cv::QT_CHECKBOX, false);
            cv::createButton("Grey", onClick<FiltersType::Grey>, this, cv::QT_RADIOBOX, false);
            cv::createButton("RGB", onClick<FiltersType::RGB>, this, cv::QT_RADIOBOX, true);
            cv::createButton("Sobel", onClick<FiltersType::Sobel>, this, cv::QT_PUSH_BUTTON, false);

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

    cv::Mat applyFilter(FiltersType filter)
    {
        cv::Mat result;

        if (filter == FiltersType::RGB)
            filters.erase(FiltersType::Grey);
        else if (filter == FiltersType::Grey)
            filters.erase(FiltersType::RGB);

        if (filters.contains(filter))
            filters.erase(filter);
        else
            filters.emplace(filter);

        image.copyTo(result);

        for (const auto currentFilter: filters)
            switch (currentFilter)
            {
                case FiltersType::Blur:
                    cv::blur(result, result, cv::Size(5, 5));
                    break;
                case FiltersType::Grey:
                    cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
                    break;
                case FiltersType::RGB:
                    image.copyTo(result);
                    break;
                case FiltersType::Sobel:
                    cv::Sobel(result, result, CV_8U, 1, 1);
                    break;
            }

        return result;
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
        const auto offset = 100 * i++;
        window->move(offset, offset);
        window->show();
    }

    cv::waitKey(0);

    return 0;
}
