#ifndef CONVERT_HPP
#define CONVERT_HPP

#include <fstream>
#include <string>


static std::ofstream out;

//Compressed formats
int outPNG(std::string dst_file, const unsigned char* src, int width, int height);
int outJPG(std::string dst_file, const unsigned char* src, int width, int height);


//Uncompressed formats
int outPNM(std::string dst_file, const unsigned char* src, int width, int height, int depth, int colours, int version);
int outPAM(std::string dst_file, const unsigned char* src, int width, int height, int depth, int colours);
int outBMP(std::string dst_file, const unsigned char* src, int width, int height, int depth);


#endif
