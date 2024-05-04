#include <GL/gl.h>
#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <utility>

class ImageWindow
{
    std::string name;
    GLuint texture;
    GLfloat angle = 0.f;
    cv::VideoCapture capture;

    static void onDraw(void *param)
    {
        auto image = static_cast<ImageWindow *>(param);

        glLoadIdentity();
        glBindTexture(GL_TEXTURE_2D, image->texture);
        glRotatef(image->angle, 1.f, 1.f, 1.f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
    }

    static bool loadTexture(const cv::Mat &frame, GLuint texture)
    {
        if (!frame.data)
            return false;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

        return true;
    }

public:
    using UniPtr = std::unique_ptr<ImageWindow>;
    ImageWindow(std::string windowName, const cv::VideoCapture& videoCapture, int flags) : name(std::move(windowName)), capture(videoCapture)
    {
        cv::namedWindow(name, flags);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);

        cv::setOpenGlDrawCallback(name, onDraw, this);
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

    void show()
    {
        cv::Mat frame;

        while (cv::waitKey(30) != 'q')
        {
            capture >> frame;
            loadTexture(frame, texture);
            cv::updateWindow(name);
            angle += 4;
        }


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

int main()
{
    cv::VideoCapture capture;

    if (!capture.open(0))
        return -1;

    ImageWindow window{"Camera", capture, cv::WINDOW_OPENGL};

    window.show();

    return 0;
}
