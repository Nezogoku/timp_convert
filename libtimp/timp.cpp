#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include "timp.hpp"

using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::ifstream;
using std::ios;
using std::string;


timp::timp(bool isDebug) { resetData(); debug = isDebug; }
timp::timp() { timp(false); }


void timp::setDebug(bool isDebug) { debug = isDebug; }

void timp::loadFile(string file) { fileName = file; }

void timp::setName(string name) { imgName = name; }

void timp::resetData() {
    fileName = "";
    imgName = "";

    width = 0;
    height = 0;
    cwidth = 0;
    cheight = 0;

    val0 = 0;
    val1 = 0;
    val3 = 0;
    val4 = 0;
    val5 = 0;
    val6 = 0;
    val7 = 0;
    val8 = 0;
    val9 = 0;
    offs_pal = 0;
    offs_pix = 0;
    defined_pal = 16;

    for (auto &f0 : img) for (auto &f1 : f0) f1 = 0x00;
    for (auto &t : pal_table) t = 0x00;
    for (auto &i : pix_index) i = 0x00;

    if (debug) cout << "Data has been reset" << endl;
}


int timp::getLeInt(unsigned char *&data, int len) {
    int t = getBeInt(data, len),
        out = 0;

    for (int s = 0; s < len; ++s) {
        out = (out << 8) | (t >> (8 * s)) & 0xFF;
    }

    return out;
}

int timp::getBeInt(unsigned char *&data, int len) {
    int out = 0;
    for (int s = 0; s < len; ++s) { out = (out << 8) | *data++; }
    return out;
}


int timp::setData() {
    ifstream tFILE;
    uint32_t chunk;

    //Set name if not set
    if (imgName.empty()) {
        imgName = fileName.substr(fileName.find_last_of("\\/") + 1);
        imgName = imgName.substr(0, imgName.find_last_of('.'));
        if (debug) cout << "Set name to \"" << imgName << "\"" << endl;
    }

    tFILE.open(fileName.c_str(), ios::binary);
    if (!tFILE.is_open()) {
        if (debug) cout << "Unable to open \"" << fileName << "\"" << endl;
        return 0;
    }

    tFILE.seekg(0x00, ios::end);
    int sizeOfData = tFILE.tellg();
    if (debug) cout << "Size of file 0x" << hex << sizeOfData << dec << endl;

    unsigned char *tempTIMP = new unsigned char[sizeOfData],
                  *timpStart = tempTIMP;
    tFILE.seekg(0x00, ios::beg);
    tFILE.read((char*)(tempTIMP), sizeOfData);
    tFILE.close();

    chunk = getBeInt(tempTIMP, 0x04);
    if (chunk != TIMP) {
        if (debug) cout << "Not a TIMP file" << endl;
        return 0;
    }

    val0 = getLeInt(tempTIMP, 0x02);    // Likely Major version
    val1 = getLeInt(tempTIMP, 0x02);    // Likely Minor version
    val3 = getLeInt(tempTIMP, 0x04);    // Likely max pal. colours
    val4 = getLeInt(tempTIMP, 0x04);    // Likely Reserved
    val5 = getLeInt(tempTIMP, 0x02);    // Likely bit depth
    width = getLeInt(tempTIMP, 0x02);   // Width in pixels
    height = getLeInt(tempTIMP, 0x02);  // Height in pixels
    val6 = getLeInt(tempTIMP, 0x02);    // Unk
    val7 = getLeInt(tempTIMP, 0x02);    // Unk
    val8 = getLeInt(tempTIMP, 0x02);    // Unk
    val9 = getLeInt(tempTIMP, 0x04);    // Unk
    offs_pal = getLeInt(tempTIMP, 0x04);// Offset of palette data
    offs_pix = getLeInt(tempTIMP, 0x04);// Offset of pixel data

    if (debug) {
        cout << '\n'
             << "Version: " << val0 << '.' << val1 << '\n'
             << "Palette colours: " << val3 << '\n'
             << "Unknown value 4: " << val4 << '\n'
             << "Unknown value 5: " << val5 << '\n'
             << "Width of image: " << width << "px" << '\n'
             << "Height of image: " << height << "px" << '\n'
             << "Unknown value 6: " << val6 << '\n'
             << "Unknown value 7: " << val7 << '\n'
             << "Unknown value 8: " << val8 << '\n'
             << "Unknown value 9: " << val9 << '\n'
             << "Palette data offset: 0x" << hex << offs_pal << '\n'
             << "Pixel data offset: 0x" << offs_pix << dec << endl;
    }

    //offs_pal == 0 when palette is not defined
    bool noPal = (offs_pal) ? false : true;

    if (noPal) {
        offs_pal = offs_pix;
        defined_pal = (sizeOfData - offs_pal) / 4;
    }
    else defined_pal = (offs_pix - offs_pal) / 4;


    //Apply swizzling
    cheight = 8;
    switch (defined_pal) {
        case 0:
            cwidth = 4;
            break;

        case 16 :
        case 32 :
        case 48 :
        case 96 :
        case 112:
            cwidth = 32;
            break;

        case 64 :
        case 236:
        case 256:
            cwidth = 16;
            break;

        default:
            if (defined_pal == (width * height)) {
                if (debug) cout << "Raw bitmap data" << endl;
                cwidth = width;
                cheight = height;
            }
            else {
                if (debug) cout << "Undefined Pal "
                                << defined_pal
                                << ", defaulting cwidth to 16"
                                << endl;
                cwidth = 16;
            }
            break;
    }

    if (debug) cout << endl;
    if (debug) cout << "Defined colours in image: " << defined_pal << '\n'
                    << "Chunk width in image: " << cwidth << '\n'
                    << "Chunk height in image: " << cheight << endl;


    int px_length = width * height;

    //Determine true pixel data size
    //File size might not necessarily indicate pixel size
    //IE archived or incomplete data
    if ((width % cwidth) != 0 || (height % cheight) != 0) {
        int amntX = std::ceil((float)width / cwidth);
        int amntY = std::ceil((float)height / cheight);

        px_length = (cwidth * amntX) * (cheight * amntY);
    }


    //Read palette table
    tempTIMP = timpStart + offs_pal;
    for(int i = 0; i < defined_pal; ++i) {
        if (tempTIMP >= timpStart + sizeOfData) break;

        pal_table[i] = getBeInt(tempTIMP, 0x04);
    }
    if (debug) cout << "Number of palettes in table: " << defined_pal << endl;

    //Read pix_index according to palette
    tempTIMP = timpStart + offs_pix;
    for(int i = 0, p = 0; i < px_length; ++i) {
        if (tempTIMP >= timpStart + sizeOfData) break;

        if (noPal) pix_index[p] = p++;
        else {
            uint8_t pixel = *tempTIMP++;

            //8 bits per pixel
            if (cwidth == 16) {
                if (pixel >= defined_pal) {
                    if (p) pixel = pix_index[p - 1];
                    else pixel = 0x00;
                }
                pix_index[p++] = pixel;
            }
            //4 bits per pixel
            else if (cwidth == 32) {
                pix_index[p++] = 0x0F & pixel;
                pix_index[p++] = 0x0F & (pixel >> 4);
            }
        }
    }


    int total = ((cwidth == 32) ? (px_length * 2) : px_length);
    if (debug) cout << "\nNumber of total pixels: " << total
                    << "\nOffset of last read byte: 0x"
                    << hex << (tempTIMP - timpStart) << dec
                    << '\n' << endl;

    //Assign pix_index with swizzling
    unsigned int px = 0, x = 0, y = 0;
    while (px < total) {

        for (int ch = 0; ch < cheight; ++ch) {
            for (int cw = 0; cw < cwidth; ++cw) {

                uint16_t dimx = cw + x,
                         dimy = ch + y;

                if (dimx < width && dimy < height) {
                    img[dimx][dimy] = pal_table[pix_index[px]];
                }

                px += 1;
            }
        }
        x += cwidth;

        if (x >= width) {
            x = 0;
            y += cheight;
        }
    }

    if (debug) cout << "Finished building image" << endl;
    return 1;
}


int timp::getImgWidth() { return width; }

int timp::getImgHeight() { return height; }

int timp::getImgDepth() { return 4; }

std::string timp::getImgName() { return imgName; }

void timp::getImgRGBA8(unsigned char *out) {
    uint32_t dim = width * height,
             temp[dim] = {};
    getImgRGBA32(temp);

    for (int d = 0, p = 0; d < dim; ++d) {
        for (int s = 0; s < 4; ++s) {
            out[p++] = (temp[d] >> (24 - (8 * s))) & 0xFF;
        }
    }
}

void timp::getImgRGBA32(uint32_t *out) {
    for (int ch = 0, oi = 0; ch < height; ++ch) {
        for (int cw = 0; cw < width; ++cw) {
            out[oi++] = img[cw][ch];
        }
    }
}
