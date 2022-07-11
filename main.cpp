#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgSupported.hpp"
#include "libTIMP.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;


string progName;
bool debugSet = false;

timp setImage(string infile) {
    timp userImages(debugSet);

    userImages.loadFile(infile);

    if (!userImages.setPixels()) {
        if (debugSet) cout << "Unable to collect image data from " << infile << endl;
        return timp();
    }

    return userImages;
}

int convertImage(timp image, string outfile) {
    return image.convertImage(outfile);
}

int displayImage(timp image) {
    if (image.getImgWidth() * image.getImgHeight() == 0x00) return 0;

    if (!debugSet) image.displayImage();
    else image.debugImage();

    return 1;
}

///Program information
void progInfo() {
    cout << "USAGE: " << progName << " [options] <file(s).tip>\n\n"
         << "Options:\n"
         << "     -h                            prints all commands, ignores all other commands\n"
         << "     -d                            activates debug mode\n"
         << "     -v <infile.tip>               opens TIP file for viewing, debug loads it in chunks\n"
         << "     -b [XXX] <infile(s).tip>      batch converts all TIP files to specified image type XXX, default is PNG\n"
         << "     -c <infile.tip> [<outfile>]   converts TIP file to new image, default is PNG\n\n"
         << "     If debug and display are activated,\n"
         << "     ENTER prints a chunk of the image being viewed\n\n"
         << "Supported formats include:\n";

    for (int typ = 0, num = 0; typ < formats.size(); ++typ, ++num) {
        if (num == 8) num = 0;

        cout << formats[typ] << ", ";
    }

    cout << endl;
}

int getUsrOpt(int argc, char *argv[]) {
    string inFile;
    string outFile;

    bool isHelp = false;

    for (int cnt = 1; cnt < argc; ++cnt) {
        string cmd = argv[cnt];
        if (cmd == "-h" || cmd == "-H") {
            isHelp = true;
            break;
        }
    }

    if (argc < 2 || isHelp) {
        progInfo();
        return 0;
    }
    else if (argc == 2) {
        inFile = argv[1];
        inFile.erase(remove(inFile.begin(), inFile.end(), '\"'), inFile.end());

        return displayImage(setImage(inFile));
    }

    for (int cnt = 1; cnt < argc; ++cnt) {
        bool typeDefined = false;

        string option = argv[cnt];
        string outOpt;
        char opt = option[0];

        if (option.size() == 2 && opt == '-') {
            opt = argv[cnt][1];
            if (debugSet) cout << "Input: " << opt << endl;

            switch (opt) {
                //Set debug mode 'd'
                case 'd':
                case 'D':
                    if (!debugSet) cout << "Debug mode set" << endl;
                    debugSet = true;
                    continue;

                //Batch convert 'b'
                case 'b':
                case 'B':
                    if ((cnt + 1) >= argc || argv[cnt + 1][0] == '-') {
                        if (debugSet) cout << "Not enough options for this operation" << endl;
                        continue;
                    }
                    outOpt = argv[++cnt];

                    for (int i = 0; i < formats.size(); ++i) {
                        if (outOpt == formats[i]) {
                            typeDefined = true;
                            break;
                        }
                    }

                    if (!typeDefined) {
                        cnt -= 1;
                        if (debugSet) cout << "Output not specified, defaulting to PNG" << endl;
                        outOpt = "PNG";
                    }

                    for_each(outOpt.begin(), outOpt.end(), [](char &ch) { ch = tolower(ch); });

                    while (cnt < argc) {
                        inFile = argv[++cnt];
                        inFile.erase(remove(inFile.begin(), inFile.end(), '\"'), inFile.end());
                        outFile = inFile.substr(0, inFile.find_last_of('.') + 1) + outOpt;

                        convertImage(setImage(inFile), outFile);
                    }
                    continue;

                //Normal convert 'c'
                case 'c':
                case 'C':
                    if ((cnt + 1) >= argc || argv[cnt + 1][0] == '-') {
                        if (debugSet) cout << "Not enough options for this operation" << endl;
                        continue;
                    }
                    inFile = argv[++cnt];
                    inFile.erase(remove(inFile.begin(), inFile.end(), '\"'), inFile.end());

                    if ((cnt + 1) >= argc || argv[cnt + 1][0] == '-') {
                        cnt -= 1;
                        outFile = inFile.substr(0, inFile.find_last_of('.') + 1) + "png";

                        if (debugSet) cout << "No output set, defaulting to "
                                           << outFile.substr(outFile.find_last_of("\\/") + 1)
                                           << endl;
                    }
                    else {
                        outFile = argv[++cnt];
                        outFile.erase(remove(outFile.begin(), outFile.end(), '\"'), outFile.end());
                    }

                    convertImage(setImage(inFile), outFile);
                    continue;

                //Normal view 'v'
                case 'v':
                case 'V':
                    if ((cnt + 1) >= argc || argv[cnt + 1][0] == '-') {
                        if (debugSet) cout << "Not enough options for this operation" << endl;
                        continue;
                    }
                    inFile = argv[++cnt];
                    inFile.erase(remove(inFile.begin(), inFile.end(), '\"'), inFile.end());

                    displayImage(setImage(inFile));
                    break;

                //Unknown option
                default:
                    if (debugSet) cout << "Unknown option" << endl;
                    continue;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    progName = argv[0];
    progName.erase(remove(progName.begin(), progName.end(), '\"'), progName.end());
    progName = progName.substr(progName.find_last_of("\\/") + 1, progName.find_last_of('.'));

    return getUsrOpt(argc, argv);
}
