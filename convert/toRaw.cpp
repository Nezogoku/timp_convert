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
        int bmih_res_width;
        int bmih_res_height;
        unsigned int bmih_pal = 0;
        unsigned int bmih_important = 0;

        unsigned int bmih_r = 255 << 0;
        unsigned int bmih_g = 255 << 8;
        unsigned int bmih_b = 255 << 16;
        unsigned int bmih_a = 255 << 24;
    } bm;

    bm.bmih_data_size = width * height * 4;
    bm.bmih_width = width;
    bm.bmih_height = -height;   // Make negative to store pixels top-bottom
    bm.bmih_bpp *= depth;
    bm.bmih_res_width = 39.3701 * 300;
    bm.bmih_res_height = 39.3701 * 300;
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


///Convert to ICO or CUR
int outICO(std::string dst_file, const unsigned char* src, int width, int height, int depth, int version) {
    bool isSuccess = true;
    struct ico {
        unsigned int ico_res = 0;
        unsigned int ico_typ;
        unsigned int ico_img = 1;

        unsigned int icoe_width;
        unsigned int icoe_height;
        unsigned int icoe_pal = 0;
        unsigned int icoe_res = 0;
        int icoe_pln_hor;
        int icoe_bpp_vrt;
        unsigned int icoe_data_size = 0;
        unsigned int icoe_data_offs = 22;

        unsigned int bmih_size = 40;
        int bmih_width;
        int bmih_height;
        unsigned int bmih_planes = 1;
        unsigned int bmih_bpp = 8;
        unsigned int bmih_comp = 0;
        unsigned int bmih_data_size = 0;
        int bmih_res_width = 0;
        int bmih_res_height = 0;
        unsigned int bmih_pal = 0;
        unsigned int bmih_important = 0;

        unsigned int ico_amnt_xor;
        unsigned int ico_amnt_and;
    } ico;

    ico.ico_typ = ((version < 1) ? 1 : 2);
    ico.icoe_width = ((width < 256) ? width : 0);
    ico.icoe_height = ((height < 256) ? height : 0);
    ico.icoe_pln_hor = ((version == 1) ? width - 1 :
                        (version == 2) ? width / 2 :
                        (version == 3) ? 0 :
                        (version == 4) ? width - 1 :
                        (version == 5) ? 0 :
                        (version == 6) ? width - 1 : 1);
    ico.icoe_bpp_vrt = ((version == 1) ? height - 1 :
                        (version == 2) ? height / 2 :
                        (version == 3) ? 0 :
                        (version == 4) ? 0 :
                        (version == 5) ? height - 1 :
                        (version == 6) ? height - 1 : (depth * 8));

    ico.bmih_width = width;
    ico.bmih_height = height * 2;   // Keep positive to store pixels bottom-top
    ico.bmih_bpp *= depth;

    ico.ico_amnt_xor = (ico.bmih_bpp * width * height) / 8;
    ico.ico_amnt_and = (1 * width * height) / 8;

    //unsigned int xor_imag[ico.ico_amnt_xor] = {};
    unsigned char and_mask[ico.ico_amnt_and * 2] = {};  //AND mask makes me want to commit arson

    if (version == 1) {
        unsigned char alpha;
        for (int h = height - 1; h >= 0; --h) {
            for (int w = (width - 1) / 2; w >= 0; --w) {
                alpha = src[(h * width + w) * 4 + 3];

                if (w < ico.icoe_pln_hor && h < ico.icoe_bpp_vrt && alpha == 0xFF) {
                    ico.icoe_pln_hor = w;
                    ico.icoe_bpp_vrt = h;
                }
            }
        }
    }

    dst_file = dst_file.substr(0, dst_file.find_last_of('.'));
    dst_file += ((version == 0) ? ".ico" : ".cur");

    out.open(dst_file, std::ios::binary);

    try {
        out.write((const char*)&ico.ico_res, 2);
        out.write((const char*)&ico.ico_typ, 2);
        out.write((const char*)&ico.ico_img, 2);

        out.write((const char*)&ico.icoe_width, 1);
        out.write((const char*)&ico.icoe_height, 1);
        out.write((const char*)&ico.icoe_pal, 1);
        out.write((const char*)&ico.icoe_res, 1);
        out.write((const char*)&ico.icoe_pln_hor, 2);
        out.write((const char*)&ico.icoe_bpp_vrt, 2);
        out.write((const char*)&ico.icoe_data_size, 4);
        out.write((const char*)&ico.icoe_data_offs, 4);

        auto beg = out.tellp();
        out.write((const char*)&ico.bmih_size, 4);
        out.write((const char*)&ico.bmih_width, 4);
        out.write((const char*)&ico.bmih_height, 4);
        out.write((const char*)&ico.bmih_planes, 2);
        out.write((const char*)&ico.bmih_bpp, 2);
        out.write((const char*)&ico.bmih_comp, 4);
        out.write((const char*)&ico.bmih_data_size, 4);
        out.write((const char*)&ico.bmih_res_width, 4);
        out.write((const char*)&ico.bmih_res_height, 4);
        out.write((const char*)&ico.bmih_pal, 4);
        out.write((const char*)&ico.bmih_important, 4);

        for (int h = 0, a = 0; h < height; ++h) {
            unsigned bits = 0, msk = 0;

            for (int w = 0; w < width; ++w) {
                unsigned char charR, charG, charB, charA;

                //Vertically invert image to store bottom-top
                //Because I'm too lazy to reverse the AND mask
                charR = src[((height - h - 1) * width + w) * 4 + 0];
                charG = src[((height - h - 1) * width + w) * 4 + 1];
                charB = src[((height - h - 1) * width + w) * 4 + 2];
                charA = src[((height - h - 1) * width + w) * 4 + 3];

                //Store pixels in LE ARGB order
                out.put(charB);
                out.put(charG);
                out.put(charR);
                out.put(charA);

                //AND masking code slightly editted variant from ico_tools.py
                msk = (msk << 1) | ((charA == 0xFF) ? 0 : 1);

                if (++bits == 8) {
                    and_mask[a++] = msk;
                    bits = 0;
                    msk = 0;
                }
            }

            if (bits) {
                msk <<= (8 - bits);
                and_mask[a++] = msk;
            }

            while ((a + 1) % 4 != 0) a += 1;
            ico.ico_amnt_and = a + 1;
        }

        out.write((const char*)(and_mask), ico.ico_amnt_and);

        auto trm = out.tellp();

        ico.icoe_data_size = unsigned(trm - beg);
        ico.bmih_data_size = ico.ico_amnt_xor + ico.ico_amnt_and;

        out.seekp(0x0E, out.beg);
        out.write((const char*)&ico.icoe_data_size, 4);
    }
    catch (std::ofstream::failure &e) {
        std::cout << e.what() << std::endl;
        isSuccess = false;
    }
    out.close();

    return isSuccess;
}
