#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include "convert.hpp"


///Convert to Netpbm PBM, PGM, or PPM
int outPNM(std::string dst_file, const unsigned char* src, int width, int height, int depth, int colours, int version) {
    bool isSuccess = true,
         isText = (version < 4) ? true : false;
    int type = (version == 1 || version == 4) ? 0 :
               (version == 2 || version == 5) ? 1 :
               (version == 3 || version == 6) ? 2 : 2;

    dst_file = dst_file.substr(0, dst_file.find_last_of('.'));
    dst_file += ((type == 0) ? ".pbm" : (type == 1) ? ".pgm" : ".ppm");
    out.open(dst_file, std::ios::binary);

    out << std::setfill(' ') << std::right;
    try {
        out << "P"                      << std::to_string(version)  << '\n'
            << std::to_string(width)                                << ' '
            << std::to_string(height)                               << ' '
            << ((type) ? "255" : "")                                << '\n';

        for (int p = 0, w = 0; p < width * height * 4; ++p, ++w) {
            unsigned char charR, charG, charB;
            charR = src[p++];
            charG = src[p++];
            charB = src[p++];
            unsigned int avRGB = charR + charG + charB;

            if (!type) {
                if (avRGB > (255 * 3 / 2.0)) avRGB = 0;
                else avRGB = 1;
            }
            else avRGB /= 3;

            if (isText) {
                if (w >= width) { out << '\n'; w = 0; }

                if (type) out << std::setw(3);
                if (type < 2) {
                    out << std::to_string(avRGB) << ' ';
                }
                else {
                    out << std::to_string((unsigned)charR) << ' ' << std::setw(3)
                        << std::to_string((unsigned)charG) << ' ' << std::setw(3)
                        << std::to_string((unsigned)charB) << ' ';
                }
            }
            else {
                if (type < 2) out.write((char*)&avRGB, 1);
                else {
                    out.put(charR);
                    out.put(charG);
                    out.put(charB);
                }
            }
        }
    }
    catch (std::ofstream::failure &e) {
        isSuccess = false;
    }
    out.close();

    return isSuccess;
}

///Convert to Netpbm PAM
int outPAM(std::string dst_file, const unsigned char* src, int width, int height, int depth, int colours) {
    bool isSuccess = true;

    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".pam";
    out.open(dst_file, std::ios::binary);

    out << std::setfill(' ') << std::right;
    try {
        out << "P7\n"
            << "WIDTH "     << std::to_string(width)    << '\n'
            << "HEIGHT "    << std::to_string(height)   << '\n'
            << "DEPTH "     << std::to_string(depth)    << '\n'
            << "MAXVAL "    << std::to_string(colours)  << '\n'
            << "TUPLTYPE "  << "RGB_ALPHA"              << '\n'
            << "ENDHDR\n";

        for (int p = 0; p < width * height * 4; ++p) {
            out.put(src[p]);
        }
    }
    catch (std::ofstream::failure &e) {
        isSuccess = false;
    }
    out.close();

    return isSuccess;
}

///Convert to Bitmap
int outBMP(std::string dst_file, const unsigned char* src, int width, int height, int depth) {
    bool isSuccess = true;
    struct bm {
        const char bm[2] {'B','M'};
        unsigned int bm_size = 54;
        const unsigned int bm_res0 = 0;
        const unsigned int bm_res1 = 0;
        unsigned int bm_offs_data = bm_size + 16;

        unsigned int bmih_size = 40;
        int bmih_width;
        int bmih_height;
        unsigned int bmih_planes = 1;
        unsigned int bmih_bpp = 8;
        unsigned int bmih_comp = 3;
        unsigned int bmih_data_size;
        int bmih_res_width = 39.3701;
        int bmih_res_height = 39.3701;
        unsigned int bmih_pal = 0;
        unsigned int bmih_important = 0;

        unsigned int bmih_r = 255 << 0;
        unsigned int bmih_g = 255 << 8;
        unsigned int bmih_b = 255 << 16;
        unsigned int bmih_a = 255 << 24;
    } bm;

    bm.bmih_data_size = width * height * 4;
    bm.bmih_width = width;
    bm.bmih_height = -height;
    bm.bmih_bpp *= depth;
    bm.bmih_res_width *= 300;
    bm.bmih_res_height *= 300;
    bm.bm_size += bm.bmih_data_size + 16;

    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".bmp";
    out.open(dst_file, std::ios::binary);

    try {
        out << bm.bm;
        out.write((const char*)&bm.bm_size, 4);
        out.write((const char*)&bm.bm_res0, 2);
        out.write((const char*)&bm.bm_res1, 2);
        out.write((const char*)&bm.bm_offs_data, 4);

        out.write((const char*)&bm.bmih_size, 4);
        out.write((const char*)&bm.bmih_width, 4);
        out.write((const char*)&bm.bmih_height, 4);
        out.write((const char*)&bm.bmih_planes, 2);
        out.write((const char*)&bm.bmih_bpp, 2);
        out.write((const char*)&bm.bmih_comp, 4);
        out.write((const char*)&bm.bmih_data_size, 4);
        out.write((const char*)&bm.bmih_res_width, 4);
        out.write((const char*)&bm.bmih_res_height, 4);
        out.write((const char*)&bm.bmih_pal, 4);
        out.write((const char*)&bm.bmih_important, 4);
        out.write((const char*)&bm.bmih_r, 4);
        out.write((const char*)&bm.bmih_g, 4);
        out.write((const char*)&bm.bmih_b, 4);
        out.write((const char*)&bm.bmih_a, 4);

        for (int p = 0; p < bm.bmih_data_size; ++p) {
            out.put(*src++);
        }
    }
    catch (std::ofstream::failure &e) {
        std::cout << e.what() << std::endl;
        isSuccess = false;
    }
    out.close();

    return isSuccess;
}
