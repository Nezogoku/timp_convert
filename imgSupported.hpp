#ifndef IMGSUPPORTED_HPP
#define IMGSUPPORTED_HPP

#include <string>

#define FORMATS 7
const std::string formats[FORMATS + 1][8] = {
    {"DISPLAY"},
    {"BMP", "DIB"},
    {"PNM", "PBM", "PGM", "PPM"},
    {"PAM"},
    {"JPG", "JPEG", "JPE", "JIF", "JFIF", "JFI"},
    {"ICO"},
    {"CUR"},
    {"PNG"},
};

const std::string format_descriptions[FORMATS + 1][8] = {
    {""},
    {"Windows (Device-Independent) Bitmap"},
    {"Netpbm Portable Anymap Format, versions 1 through 6 correspond with:",
     "    Portable Bitmap Format, versions 1 (ASCII) and 4 (Binary)",
     "    Portable Greymap Format, versions 2 (ASCII) and 5 (Binary)",
     "    Portable Pixmap Format, versions 3 (ASCII) and 6 (Binary)"},
    {"Netpbm Portable Arbitrary Map"},
    {"Joint Photographic Experts Group format"},
    {"Windows Computer Icon"},
    {"Windows Mouse Cursor, versions 1 through 6 correspond with:",
     "    Auto-generated hotspot, version 1",
     "    Centred hotspot, version 2",
     "    Top-left hotspot, version 3",
     "    Top-right hotspot, version 4",
     "    Bottom-left hotspot, version 5",
     "    Bottom-right hotspot, version 6"},
    {"Portable Network Graphics format"},
};

#endif
