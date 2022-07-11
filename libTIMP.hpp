#ifndef LIBTIMP_HPP
#define LIBTIMP_HPP

#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class timp {
    public:
        timp();
        timp(bool isDebug);

        void loadFile(std::string file);
        int setPixels();

        int getImgWidth();
        int getImgHeight();
        std::string getImgName();

        void displayImage();
        void debugImage();
        int convertImage(std::string outfile);

    private:
        sf::View letterBox(sf::View view, int windX, int windY);
        sf::Image image;
        sf::Image imgchunk;

        uint16_t width = 0, height = 0;
        int chunk_w = 0,chunk_h = 0;
        std::string fileName = "";

        bool debug = false;
        bool noPal;

        std::vector<sf::Color> pal_color;
        std::vector<uint8_t> pixels;
};

#endif
