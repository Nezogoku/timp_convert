#include <string>
#include "CImg.h"
#include "display.hpp"

using cimg_library::CImg;
using cimg_library::CImgDisplay;

int display(std::string name, const unsigned char *src, int width, int height) {
    unsigned char linSrc[width * height * 4] = {};
    for (int p = 0, w = 0; w < width * height * 4; ++p) {
            linSrc[((width * height) * 0) + p] = src[w++];
            linSrc[((width * height) * 1) + p] = src[w++];
            linSrc[((width * height) * 2) + p] = src[w++];
            linSrc[((width * height) * 3) + p] = src[w++];
    }

    CImg<unsigned char> image(linSrc, width, height, 1, 4, true);

    if (name.empty()) name = "Image";
    CImgDisplay disp(image, name.c_str(), 0, false);

    while (!disp.is_closed() && !disp.is_keyESC()) {
        disp.wait();

        if (disp.is_resized()) {
            double wX = disp.window_width(),
                   wY = disp.window_height(),
                   iX = image.width(),
                   iY = image.height();

            double wAsp = wX / wY,
                   iAsp = iX / iY;

            int nX, nY;

            if (wAsp > iAsp) {
                nX = wX;
                nY = wX / iX * iY;
            }
            else if (wAsp < iAsp) {
                nX = wY / iY * iX;
                nY = wY;
            }
            else {
                nX = wX;
                nY = wY;
            }

            disp.resize(nX, nY, false);
            disp.display(image);
        }
    }

    return 1;
}
