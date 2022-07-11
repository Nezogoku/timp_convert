#include "libTIMP.hpp"

using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::ifstream;
using std::ios;
using std::string;
using std::vector;
using std::max;

using sf::View;
using sf::FloatRect;
using sf::RenderWindow;
using sf::Keyboard;
using sf::Event;
using sf::VideoMode;
using sf::Image;
using sf::Color;
using sf::Texture;
using sf::Sprite;

timp::timp() { }
timp::timp(bool isDebug) { debug = isDebug; }

void timp::loadFile(string file) { fileName = file; }

int timp::setPixels() {
    int sizeOfData = 0;

    noPal = false;
    vector<Color> ().swap(pal_color);
    vector<uint8_t> ().swap(pixels);

    ifstream TIMP(fileName, ios::binary);

    //Read width and height
    TIMP.seekg(0x12);
    TIMP.read(reinterpret_cast<char*>(&width), sizeof(uint16_t));
    if (debug) cout << "Width of image: " << width << "px" << endl;

    TIMP.seekg(0x14);
    TIMP.read(reinterpret_cast<char*>(&height), sizeof(uint16_t));
    if (debug) cout << "Height of image: " << height << "px" << endl;

    int pal_colors = 16;

    uint32_t pal_data,px_data;

    //Read offsets
    TIMP.seekg(0x20);
    TIMP.read(reinterpret_cast<char*>(&pal_data), sizeof(uint32_t));
    if (debug) cout << "Palette data offset: 0x" << hex << pal_data << dec << endl;

    TIMP.seekg(0x24);
    TIMP.read(reinterpret_cast<char*>(&px_data), sizeof(uint32_t));
    if (debug) cout << "Pixel data offset: 0x" << hex << px_data << dec << endl;

    //Pal_data == 0 when palette is not defined
    if (pal_data != 0x0) pal_colors = (px_data - 0x30) / 0x4;
    else pal_colors = 0;

    if (px_data == 0x10) noPal = true;

    //Apply swizzling
    if(pal_colors == 0) {
        noPal = true;
        chunk_w = 4;
        chunk_h = 8;
    }
    else if(pal_colors == 16) {
        chunk_w = 32;
        chunk_h = 8;
    }
    else if(pal_colors == 32) {
        chunk_w = 32;
        chunk_h = 8;
    }
    else if(pal_colors == 64) {
        chunk_w = 16;
        chunk_h = 8;
    }
    else if(pal_colors == 48) {
        chunk_w = 32;
        chunk_h = 8;
    }
    else if(pal_colors == 96) {
        chunk_w = 32;
        chunk_h = 8;
    }
    else if(pal_colors == 112) {
        chunk_w = 32;
        chunk_h = 8;
    }
    else if(pal_colors == 236) {
        chunk_w = 16;
        chunk_h = 8;
    }
    else if(pal_colors == 256) {
        chunk_w = 16;
        chunk_h = 8;
    }

    if (debug) cout << "Defined colours in image: " << pal_colors << endl;
    if (debug) cout << "Chunk width in image: " << chunk_w << endl;
    if (debug) cout << "Chunk height in image: " << chunk_h << endl;

    //Read palette
    int numBlank = 0;

    for(int i=0; i<pal_colors; i++) {
        uint8_t r,g,b,a;

        TIMP.seekg(pal_data + i*4);
        TIMP.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));

        TIMP.seekg(pal_data + i*4 + 1);
        TIMP.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));

        TIMP.seekg(pal_data + i*4 + 2);
        TIMP.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));

        TIMP.seekg(pal_data + i*4 + 3);
        TIMP.read(reinterpret_cast<char*>(&a), sizeof(uint8_t));

        pal_color.push_back(Color(r,g,b,a));
        if (((r == 0x00 && g == 0x00 && b == 0x00) ||
             (r == 0xFF && g == 0xFF && b == 0xFF)) &&
            (a == 0x00 || a == 0xFF)) numBlank += 1;
    }
    if (debug) cout << "Number of empty colours: " << numBlank << endl;
    if (debug) cout << "Number of pixels in array: " << pal_color.size() << endl;

    //Amount of pixels
    int px_length = width*height;

    //Determine true pixel data size
    int usedBytes = 0;

    if (width%chunk_w==0 && height%chunk_h==0) usedBytes = px_length;
    else {
        int amntX = ceil((float)width / chunk_w);
        int amntY = ceil((float)height / chunk_h);

        usedBytes = (chunk_w * amntX) * (chunk_h * amntY);
    }

    //If palette exists, read pixels according to palette
    if (!noPal) {
        for(int i=0; i<usedBytes; i++) {
            int pixel;

            TIMP.seekg(px_data+i);
            TIMP.read(reinterpret_cast<char*>(&pixel), sizeof(uint8_t));

            if(pal_colors == 64 || pal_colors >= 200) //8 bits per pixel
            {
                if (pixel >= pal_color.size()) pixel = pal_color.size() - 1;
                pixels.push_back(pixel);
            }
            else if(pal_colors < 200) //4 bits per pixel
            {
                pixels.push_back(pixel&0xF);
                pixels.push_back((pixel>>4)&0xF);
            }

            sizeOfData = TIMP.tellg();
        }
    }
    else //If palette doesn't exist, read colours from pixels
    {
        uint8_t maxPal = 0x00;
        for(int i=0; i<usedBytes; i++) {
            uint32_t pixel;

            uint8_t r,g,b,a;

            TIMP.seekg(px_data + i*4);
            TIMP.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));

            TIMP.seekg(px_data + i*4 + 1);
            TIMP.read(reinterpret_cast<char*>(&g), sizeof(uint8_t));

            TIMP.seekg(px_data + i*4 + 2);
            TIMP.read(reinterpret_cast<char*>(&b), sizeof(uint8_t));

            TIMP.seekg(px_data + i*4 + 3);
            TIMP.read(reinterpret_cast<char*>(&a), sizeof(uint8_t));

            pal_color.push_back(Color(r,g,b,a));
            pixels.push_back(pal_color.size()-1);

            maxPal = max(maxPal, r);
            maxPal = max(maxPal, g);
            maxPal = max(maxPal, b);
            maxPal = max(maxPal, a);

            sizeOfData = TIMP.tellg();
        }

        if (maxPal >= 48) pal_colors = 256;
        else if (maxPal >= 16) pal_colors = 48;
        else pal_colors = 16;
    }

    TIMP.close();

    if (debug) cout << "Number of pixels: " << pixels.size() << endl;
    if (debug) cout << "Offset of last read byte: 0x" << hex << sizeOfData << dec << endl;

    //Create empty image
    image.create(width,height,Color(0,0,0,0));

    int x=0,y=0;
    unsigned int p=0;

    //Assign pixels with swizzling
    while(p < pixels.size()) {
        for(int ch=0; ch<chunk_h; ch++) {
            for(int cw=0; cw<chunk_w; cw++) {
                if(x+cw < width && y+ch < height) {
                    if (pixels[p] >= pal_color.size()) pixels[p] = pal_color.size() - 1;

                    image.setPixel(x+cw,y+ch,pal_color[pixels[p]]);
                }

                //Pixels are incremented nonetheless so swizzling can work on textures with sizes that are not a power of 2
                p++;
            }
        }

        x+=chunk_w;

        if(x >= width)
        {
            x=0;
            y+=chunk_h;
        }
    }

    return 1;
}

int timp::getImgWidth() { return width; }
int timp::getImgHeight() { return height; }

std::string timp::getImgName() { return fileName.substr(fileName.find_last_of("\\/") + 1, fileName.find_last_of('.')); }

sf::View timp::letterBox(View view, int windX, int windY) {
    float windowRatio = windX / (float)windY;
    float viewRatio = view.getSize().x / (float)view.getSize().y;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.00;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.00;
    }

    view.setViewport(FloatRect(posX, posY, sizeX, sizeY));

    return view;
}

void timp::displayImage() {
    int x = image.getSize().x;
    int y = image.getSize().y;

    Texture newtxt;
    newtxt.loadFromImage(image);

    Sprite newsprt;
    newsprt.setTexture(newtxt);

    View newview;
    newview.setSize(x, y);
    newview.setCenter(newview.getSize().x / 2, newview.getSize().y / 2);

    RenderWindow display(VideoMode(x, y), getImgName());
    newview = letterBox(newview, x, y);

    while (display.isOpen()) {
        Event newevt;
        while (display.pollEvent(newevt)) {
            if (newevt.type == Event::Closed) display.close();

            else if (newevt.type == Event::Resized) {
                newview = letterBox(newview, newevt.size.width, newevt.size.height);
            }
        }

        display.clear();
        display.setView(newview);
        display.draw(newsprt);
        display.display();
    }
}

void timp::debugImage() {
    image.create(width,height,Color(0xAF,0xDA,0xF0,0xFF));
    imgchunk.create(chunk_w,chunk_h,Color(0xAF,0xDA,0xF0,0xFF));

    int tempx=0,tempy=0;
    unsigned int p=0;

    Texture newtxt,
            newtxchnk;
    newtxt.loadFromImage(image);
    newtxchnk.loadFromImage(imgchunk);

    Sprite newsprt,
           newspchnk;
    newsprt.setTexture(newtxt);
    newspchnk.setTexture(newtxchnk);

    View newview,
         newvwchnk;
    newview.setSize(width, height);
    newview.setCenter(newview.getSize().x / 2, newview.getSize().y / 2);

    newvwchnk.setSize(chunk_w,chunk_h);
    newvwchnk.setCenter(newvwchnk.getSize().x / 2, newvwchnk.getSize().y / 2);

    RenderWindow display(VideoMode(width, height), getImgName()),
                 chunkDisplay(VideoMode(chunk_w, chunk_h), getImgName() + string(" chunk"));

    newview = letterBox(newview, width, height);
    newvwchnk = letterBox(newvwchnk, chunk_w, chunk_h);

    while (display.isOpen() && chunkDisplay.isOpen()) {
        Event newevt,
              newvtchnk;
        while (display.pollEvent(newevt) || chunkDisplay.pollEvent(newvtchnk)) {
            if (newevt.type == Event::Closed || newvtchnk.type == Event::Closed) {
                display.close();
                chunkDisplay.close();
            }

            else if (newevt.type == Event::KeyPressed && newevt.key.code == Keyboard::Enter) {
                cout << "Current index of pixel array: " << p << endl;
                if (p < pixels.size()) {
                    imgchunk.create(chunk_w,chunk_h,Color(0xAF,0xDA,0xF0,0xFF));
                    int chunkx=0,chunky=0;

                    for(int ch=0; ch<chunk_h; ch++) {
                        for(int cw=0; cw<chunk_w; cw++) {
                            if(tempx+cw < width && tempy+ch < height) {
                                if (pixels[p] >= pal_color.size()) pixels[p] = pal_color.size() - 1;

                                image.setPixel(tempx+cw,tempy+ch,pal_color[pixels[p]]);
                                newtxt.loadFromImage(image);
                                newsprt.setTexture(newtxt);
                                newview.setSize(width, height);
                                newview.setCenter(newview.getSize().x / 2, newview.getSize().y / 2);

                                chunkx = tempx;
                                chunky = tempy;
                                while (chunkx >= chunk_w) chunkx -= chunk_w;
                                while (chunky >= chunk_h) chunky -= chunk_h;
                                imgchunk.setPixel(chunkx+cw,chunky+ch,pal_color[pixels[p]]);
                                newtxchnk.loadFromImage(imgchunk);
                                newspchnk.setTexture(newtxchnk);
                                newvwchnk.setSize(chunk_w, chunk_h);
                                newvwchnk.setCenter(newvwchnk.getSize().x / 2, newvwchnk.getSize().y / 2);

                                //cout << "0x" << hex << std::setfill('0') << std::setw(8) << pal_color[pixels[p]].toInteger() << dec << ' ';
                            }

                            p++;
                        }

                        //cout << endl;
                    }

                    tempx+=chunk_w;

                    if(tempx >= width) {
                        tempx=0;
                        tempy+=chunk_h;
                    }

                    //cout << endl;
                }
            }

            else if (newevt.type == Event::Resized) {
                newview = letterBox(newview, newevt.size.width, newevt.size.height);
            }

            else if (newvtchnk.type == Event::Resized) {
                newvwchnk = letterBox(newvwchnk, newvtchnk.size.width, newvtchnk.size.height);
            }
        }

        display.clear();
        display.setView(newview);
        display.draw(newsprt);
        display.display();

        chunkDisplay.clear();
        chunkDisplay.setView(newvwchnk);
        chunkDisplay.draw(newspchnk);
        chunkDisplay.display();
    }
}

int timp::convertImage(string outfile) {
    return image.saveToFile(outfile);
}
