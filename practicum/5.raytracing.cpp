/**
 * @file 5.raytracing.cpp
 * @brief A simple raytracer that generates a BMP image of a sphere with lighting.
 * @date 2026-01-21
 * @section usage
 *   g++ -std=c++11 -Wall -o 5.raytracing.exe 5.raytracing.cpp -lm -static-libstdc++ -static
 *   ./5.raytracing.exe
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cmath>

/**
 * @brief A simple BMP image class to create and save 24-bit BMP files.
 */

class BMPImage {
public:
    BMPImage(int width, int height);
    void save(const std::string& filename) const;
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
private:

    // BMP file headers with 1-byte alignment
    #pragma pack(push, 1)

    // BMP file header (14 bytes)
    struct BMPFileHeader {
        uint16_t signature = 0x4D42;        // signature "BM"
        uint32_t fileSize;                  // total file size
        uint16_t reserved1 = 0;
        uint16_t reserved2 = 0;
        uint32_t offsetData = 54;           // offset to pixel data
        BMPFileHeader(uint32_t fs) : fileSize(fs) {}
    };

    // Information header (40 bytes, BITMAPINFOHEADER version)
    struct BMPInfoHeader {
        uint32_t size = 40;                 // size of this header
        int32_t  width;                     // width in pixels
        int32_t  height;                    // height in pixels (positive — bottom to top)
        uint16_t planes = 1;                // number of color planes
        uint16_t bitCount = 24;             // 24 bits = 3 bytes per pixel (RGB)
        uint32_t compression = 0;           // no compression
        uint32_t imageSize = 0;             // can be 0 for BI_RGB
        int32_t  xPixelsPerMeter = 2835;    // Resolution ~72 dpi
        int32_t  yPixelsPerMeter = 2835;
        uint32_t colorsUsed = 0;             // number of colors used
        uint32_t colorsImportant = 0;        // number of important colors
        BMPInfoHeader(int32_t w, int32_t h) : width(w), height(h) {}
    };

    #pragma pack(pop)
    // End of 1-byte alignment

    int width_, height_;         // Image dimensions
    int rowStride_;              // Number of bytes per row (including padding)
    std::vector<uint8_t> data_;  // Pixel data in BGR format

    int getRowStride() const;
};

BMPImage::BMPImage(int width, int height) :
    width_(width), 
    height_(height), 
    rowStride_(getRowStride()), 
    data_(rowStride_ * height, 0)
{
}

int BMPImage::getRowStride() const
{
    int rowSize = width_ * 3;                 // 3 bytes per pixel
    int padding = (4 - (rowSize % 4)) % 4;    // if needed, pad to multiple of 4 bytes
    return rowSize + padding;                 // total bytes per row
}

void BMPImage::save(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Cannot open file for writing");

    // Write headers
    BMPInfoHeader infoHeader(width_, height_);
    BMPFileHeader fileHeader(sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + rowStride_ * height_);
    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
    if (file.fail()) throw std::runtime_error("Error writing BMP header");

    // Write pixel data
    file.write(reinterpret_cast<const char*>(data_.data()), data_.size());
    if (file.fail()) throw std::runtime_error("Error writing BMP data");
}

void BMPImage::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) 
        throw std::out_of_range("Pixel coordinates out of range");

    int row = height_ - 1 - y; // BMP stores rows bottom to top
    int index = row * rowStride_ + x * 3;

    data_[index + 0] = b; // BMP uses BGR format
    data_[index + 1] = g;
    data_[index + 2] = r;
}   

/**
 * @brief Create a raytraced BMP image of a sphere with lighting.
 * 
 * @param filename The output BMP file name.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @return true on success, false on failure.
 */

bool createRaytracedBMP(const std::string& filename, int width, int height)
{
    try {
        BMPImage image(width, height);

        // screen size
        double h_3d = 48.0;
        double w_3d = 64.0;

        // eye
        double x0 = 0.0;
        double y0 = 0.0;
        double z0 = 0.0;

        // sphere center and radius
        double xc = 0.0;
        double yc = 0.0;
        double zc = 10.0;
        double r = 7.0;

        // oriented light source
        double vs[3]{ 1.0, -2.0, -0.5 };

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                struct { int r, g, b; } color;
                double l = (-w_3d * 0.5) + (i + 0.5) * (w_3d / width);
                double m = (-h_3d * 0.5) + (j + 0.5) * (h_3d / height);
                double n = 10.0;
                double a = l * l + m * m + n * n;
                double b = 2 * (l * (x0 - xc) + m * (y0 - yc) + n * (z0 - zc));
                double c = (x0 - xc) * (x0 - xc) + (y0 - yc) * (y0 - yc) + (z0 - zc) * (z0 - zc) - r * r;
                double d = b * b - 4 * a * c;

                if (d >= 0)
                {
                    double t_fin = fmin((-b - sqrt(d)) / (2 * a), (-b + sqrt(d)) / (2 * a));

                    double vn[3];
                    vn[0] = x0 + l * t_fin - xc;
                    vn[1] = y0 + m * t_fin - yc;
                    vn[2] = z0 + n * t_fin - zc;

                    double dif = (vs[0] * vn[0] + vs[1] * vn[1] + vs[2] * vn[2]) /
                           (sqrt(vs[0] * vs[0] + vs[1] * vs[1] + vs[2] * vs[2]) *
                           sqrt(vn[0] * vn[0] + vn[1] * vn[1] + vn[2] * vn[2]));
                    if (dif < 0) dif = 0;

                    color.r = (int)(1.0 *   0.0 * dif);
                    color.g = (int)(1.0 * 185.0 * dif);
                    color.b = (int)(1.0 * 241.0 * dif);
                }
                else
                {
                    color = { 0, 85, 81 }; // background
                }
                image.setPixel(i, j, color.r, color.g, color.b);
            }
        }
        image.save(filename);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return false;
    }
    return true;
}

int main()
{
    const int WIDTH  = 800;
    const int HEIGHT = 600;

    if (createRaytracedBMP("raytrace.bmp", WIDTH, HEIGHT)) {
        std::cout << "Done.\n";
    } else {
        std::cout << "Error creating file.\n";
        return 1;
    }

    return 0;
}