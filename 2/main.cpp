#include <iostream>
#include <format>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int main() {
    auto filename = cv::samples::findFile("lena.jpg");
    auto colour = cv::imread(filename);
    auto gray = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (!colour.data)
    {
        std::cerr << "Could not open or find the image\n";
        return 1;
    }

    cv::imwrite("lenagray.jpg", gray);
    auto col = colour.cols - 1;
    auto row = colour.rows - 1;

    auto pixel = colour.at<cv::Vec3b>(row, col);

    std::cout << std::format("Pixel value (B, G, R): ({}, {}, {})\n", pixel[0], pixel[1], pixel[2]);
    cv::imshow("Lena BGR", colour);
    cv::imshow("Lena gray", gray);

    cv::waitKey(0);

    return 0;
}
