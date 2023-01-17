#include <fstream>
#include <string>
#include "convert.hpp"
#include "png/lodepng.h"
#include "jpg/toojpeg.h"


///Convert to 32-bit PNG
int outPNG(std::string dst_file, const unsigned char* src, int width, int height) {
    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".png";

    return !(lodepng::encode(dst_file, src, width, height, LCT_RGBA, 8));
}


///Convert to JPEG
int outJPG(std::string dst_file, const unsigned char* src, int width, int height) {
    unsigned char src24[width * height * 3] = {};

    for (int p = 0, s = 0; p < width * height * 4; ++p) {
        src24[s++] = src[p++];
        src24[s++] = src[p++];
        src24[s++] = src[p++];
    }

    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".jpg";
    out.open(dst_file, std::ios::binary);

    const bool isRGB = true;
    const int quality = 100;
    const bool downsample = false;
    const char* comment = "Converted from TIMP image using convertTIMP";

    auto toDst = [](unsigned char byt)->void{ out.put(byt); };
    auto isSuccess = TooJpeg::writeJpeg(toDst, src24, width, height,
                                        isRGB, quality, downsample, comment);

    out.close();

    return isSuccess;
}
