#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "convert.hpp"


///Create table of ASCII values from pixels {WIP}
std::vector<std::string> getASCII(std::vector<unsigned long int> src, int chars) {
    // Skip \ and "
    const char* ASCII_DENSITY = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,^`'. ";


    std::vector<std::string> ascii;

    return ascii;
}


///Convert to ASCII art (RGB) {WIP}
int outASCII(std::string dst_file, std::vector<unsigned long int> src, int width, int height) {


    return 0;
}

///Convert to CSV (RGBA) {WIP}
int outCSV(std::string dst_file, std::vector<unsigned long int> src, int width, int height) {
    bool isSuccess = true;

    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".csv";
    std::ofstream out(dst_file);

    out << std::setfill('0') << std::right;
    for (int p = 0, w = 0, h = 0; p < src.size(); ++p) {
        try { out << "#" << std::setw(8) << std::hex << src[p] << std::dec; }
        catch (std::exception &e) {
            isSuccess = false;
            break;
        }

        if (++w == width) {
            w = 0;
            out << '\n';
            if (++h == height) break;
        }
        else out << ',';
    }
    out.close();
    out << std::dec;

    return isSuccess;
}

///Convert to XPM3 (RGB)
int outXPM(std::string dst_file, std::vector<unsigned long int> src, int width, int height) {
    bool isSuccess = true;

    std::vector<unsigned long int> pixels = src;
    std::sort(pixels.begin(), pixels.end());

    auto iter = std::unique(pixels.begin(), pixels.end());
    pixels.resize(iter - pixels.begin());

    std::vector<std::string> ascii = getASCII(pixels, 2);

    dst_file = dst_file.substr(0, dst_file.find_last_of('.')) + ".xpm";
    std::ofstream out(dst_file);

    std::string dst_intern_name = dst_file.substr(dst_file.find_last_of("\\/") + 1);
    dst_intern_name = dst_intern_name.substr(0, dst_intern_name.find_last_of('.')) + "_xpm";

    try {
        out << "/* XPM */\n"
            << "static char * " << dst_intern_name << "[] = {\n"
            << "\"" << width << ' ' << height << ' ' << pixels.size() << "2\",\n";

        for (auto p : src) {
            out.put(p);
        }
    }
    catch (std::exception &e) {
        isSuccess = false;
    }
    out.close();

    return isSuccess;
}
