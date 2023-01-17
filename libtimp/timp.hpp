#ifndef LIBTIMP_HPP
#define LIBTIMP_HPP

#include <string>

#define MAX_PAL     256
#define MAX_DIM     MAX_PAL * 2
#define TIMP        0x54494D50

class timp {
    public:
        timp();
        timp(bool isDebug);

        void setDebug(bool isDebug);
        void loadFile(std::string file);
        void setName(std::string name);
        void resetData();
        int setData();

        int getImgWidth();
        int getImgHeight();
        int getImgDepth();
        std::string getImgName();

        void getImgRGBA8(unsigned char *out);
        void getImgRGBA32(uint32_t *out);

    private:
        int getLeInt(unsigned char *&data, int len);
        int getBeInt(unsigned char *&data, int len);

        uint16_t val0,                                  // Unknown value 0x04
                 val1;                                  // Unknown value 0x06
        uint32_t val3,                                  // Unknown value 0x08
                 val4,                                  // Unknown value 0x0C
                 val5;                                  // Unknown value 0x10
        uint16_t width,                                 // Width of image
                 height,                                // Height of image
                 val6,                                  // Unknown value 0x16
                 val7,                                  // Unknown value 0x18
                 val8;                                  // Unknown value 0x1A
        uint32_t val9,                                  // Unknown value 0x1C
                 offs_pal,                              // Offset image palette data
                 offs_pix,                              // Offset image pixel data
                 defined_pal;                           // Defined palette

        int cwidth, cheight;                            // Bits per pixel and bits per channel of image
        std::string fileName,                           // Full path of image
                    imgName;                            // Name of image

        bool debug = false;

        uint32_t img[MAX_DIM][MAX_DIM] = {};            // 2-D Array storing final image
        uint32_t pal_table[MAX_PAL] = {};               // Palette table of image
        uint8_t pix_index[MAX_DIM * MAX_DIM] = {};      // Index table for palette table of image
};

#endif
