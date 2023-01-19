#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "imgSupported.hpp"
#include "libtimp/timp.hpp"
#include "convert/convert.hpp"
#include "display/display.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;


int useImage(string infile, string outfile, string type, int version, bool debugSet) {
    for (auto &s: type) s = (char)toupper(s);

    if (debugSet) cout << "Entered USEIMAGE"
                       << "\n\tIN  FILE: " << infile
                       << "\n\tOUT FILE: " << outfile
                       << "\n\tOUT TYPE: " << type
                       << "\n\tVERSION : " << version
                       << '\n' << endl;

    bool found = false,
         r = false;

    int i = 0;
    for (auto &sub : formats) {
        for (auto &mode : sub) {
            if (mode == type) { found = true; break; }
        }
        if (found) break;

        i += 1;
    }
    if (!found) {
        if (debugSet) cout << "Current mode not supported" << endl;
        return r;
    }

    timp userImages(debugSet);
    userImages.loadFile(infile);
    if (!userImages.setData()) return r;

    string imageName = userImages.getImgName();
    int x = userImages.getImgWidth(),
        y = userImages.getImgHeight(),
        z = userImages.getImgDepth();

    if (x * y == 0x00) return r;

    unsigned char img8[x * y * 4] = {};
    userImages.getImgRGBA8(img8);

    if (debugSet) cout << "Current Mode: ";
    switch(i) {
        case 0:
            if (debugSet) cout << "DISPLAY" << endl;
            r = display(imageName, img8, x, y);
            break;

        case 1:
            if (debugSet) cout << "CONVERT TO BMP" << endl;
            r = outBMP(outfile, img8, x, y, z);
            break;

        case 2:
            if (debugSet) cout << "CONVERT TO PNM" << endl;

            if (type == formats[2][1] && (version != 1 && version != 4)) version = 1;
            else if (type == formats[2][2] && (version != 2 && version != 5)) version = 2;
            else if (type == formats[2][3] && (version != 3 && version != 6)) version = 3;
            else if (version > 6) version = 6;

            if (debugSet) cout << "PNM version set to " << version << endl;

            r = outPNM(outfile, img8, x, y, z, 255, version);
            break;

        case 3:
            if (debugSet) cout << "CONVERT TO PAM" << endl;
            r = outPAM(outfile, img8, x, y, z, 255);
            break;

        case 4:
            if (debugSet) cout << "CONVERT TO JPG" << endl;
            r = outJPG(outfile, img8, x, y);
            break;

        case 5:
            if (debugSet) cout << "CONVERT TO ICO" << endl;
            r = outICO(outfile, img8, x, y, z, 0);
            break;

        case 6:
            if (debugSet) cout << "CONVERT TO CUR" << endl;

            if (version < 1 || version > 6) version = 1;

            if (debugSet) cout << "CUR version set to " << version << endl;

            r = outICO(outfile, img8, x, y, z, version);
            break;

        case 7:
            if (debugSet) cout << "CONVERT TO PNG" << endl;
            r = outPNG(outfile, img8, x, y);
            break;
    }

    if (debugSet) cout << '\n';

    return r;
}

///Program information
void progInfo(string progName) {
    cout << "USAGE: " << progName << " [options] <file(s).tip/cip>\n\n"
         << "Options:\n"
         << "    -h                                  Prints all commands, ignores all other commands\n"
         << "    -d                                  Activates debug mode\n"
         << "    -v <infile>                         Opens TIMP file for viewing, default option\n"
         << "    -b [XXX] [V] <infile(s)>            Batch converts all TIP files to specified image type XXX, default is PNG\n"
         << "                                            optional version type V for appropriate images, default is 0\n"
         << "    -c <infile> [<outfile>] [V]         Converts TIP file to new image, default is PNG\n"
         << "                                            optional version type V for appropriate images, default is 0\n\n"
         << "Supported formats include:";

    for (int f = 0; f < FORMATS; ++f) {
        cout << "\n    ";
        for (auto &f0 : formats[f + 1]) if (!f0.empty()) cout << f0 << ' ';
        for (auto &f1 : format_descriptions[f + 1]) if (!f1.empty()) cout << "\n        " << f1;
    }
    cout << endl;
}


int main(int argc, char *argv[]) {
    bool debugSet = false;
    string progName = argv[0];
    progName.erase(remove(progName.begin(), progName.end(), '\"'), progName.end());
    progName = progName.substr(progName.find_last_of("\\/") + 1);
    progName = progName.substr(0, progName.find_last_of('.'));

    if (argc < 2) progInfo(progName);
    else {
        for (int a = 1; a < argc;) {
            int version = 0;
            string inFile = "";
            string outFile = "";
            string outOpt = "";
            string option = argv[a];
            option.erase(remove(option.begin(), option.end(), '\"'), option.end());


            if (option[0] == '-') {
                vector<string> outOpts;
                int e = 1;

                option[1] = (char)toupper(option[1]);

                switch(option[1]) {
                    //Get help 'H'
                    case 'H':
                        if (debugSet) cout << "HELP set" << endl;

                        progInfo(progName);
                        return 0;

                    //Set debug mode 'H'
                    case 'D':
                        cout << "Toggle DEBUG MODE" << endl;
                        debugSet = !debugSet;
                        a += 1;
                        continue;

                    //Batch convert 'B'
                    case 'B':
                        if (debugSet) cout << "BATCH CONVERT set" << endl;

                        a += 1;
                        while (a < argc) {
                            outOpt = argv[a];
                            outOpt.erase(remove(outOpt.begin(), outOpt.end(), '\"'), outOpt.end());

                            if (outOpt[0] == '-') break;
                            a += 1;

                            outOpts.push_back(outOpt);
                        }

                        if (outOpts.empty()) {
                            if (debugSet) cout << "Not enough options for this operation" << endl;
                            continue;
                        }

                        outOpt = outOpts[0];
                        if (outOpt.size() > 4 || outOpt.find_first_of('.') != string::npos) {
                            if (debugSet) cout << "Output not specified, defaulting to PNG" << endl;
                            outOpt = "PNG";
                            e = 0;
                        }

                        if (outOpts[e].find_first_not_of("0123456789") == string::npos) {
                            version = std::stoi(outOpts[e++]);
                        }
                        else {
                            if (debugSet) cout << "Version not specified, defaulting to 0" << endl;
                        }

                        for (; e < outOpts.size(); ++e) {
                            inFile = outOpts[e];
                            outFile = inFile.substr(0, inFile.find_last_of('.'));

                            useImage(inFile, outFile, outOpt, version, debugSet);
                        }
                        continue;

                    //Normal convert 'C'
                    case 'C':
                        if (debugSet) cout << "CONVERT set" << endl;

                        a += 1;
                        while (a < argc) {
                            outOpt = argv[a];
                            outOpt.erase(remove(outOpt.begin(), outOpt.end(), '\"'), outOpt.end());

                            if (outOpt[0] == '-') break;
                            a += 1;

                            if (inFile.empty()) {
                                inFile = outOpt;
                            }
                            else if (outFile.empty()) {
                                if (outOpt.find_first_not_of("0123456789") == string::npos) {
                                    version = std::stoi(outOpt);
                                    break;
                                }
                                else outFile = outOpt;
                            }
                            else if (outOpt.find_first_not_of("0123456789") == string::npos) {
                                version = std::stoi(outOpt);
                                break;
                            }
                            else {
                                if (debugSet) cout << "Version not specified, defaulting to 0" << endl;
                                break;
                            }
                        }

                        outOpt = "PNG";
                        if (inFile.empty()) {
                            if (debugSet) cout << "Not enough options for this operation" << endl;
                            continue;
                        }
                        else if (outFile.empty()) {
                            outFile = inFile.substr(0, inFile.find_last_of('.')) + ".png";
                            if (debugSet) cout << "No output set, defaulting to " << outFile << endl;
                        }

                        outOpt = outFile.substr(outFile.find_last_of('.') + 1);
                        outFile = outFile.substr(0, outFile.find_last_of('.'));

                        useImage(inFile, outFile, outOpt, version, debugSet);
                        continue;

                    //Normal view 'V'
                    case 'V':
                        if (debugSet) cout << "DISPLAY set" << endl;

                        outOpt = argv[++a];
                        outOpt.erase(remove(outOpt.begin(), outOpt.end(), '\"'), outOpt.end());

                        if (outOpt[0] == '-') {
                            if (debugSet) cout << "Not enough options for this operation" << endl;
                            continue;
                        }
                        inFile = outOpt;

                        return useImage(inFile, "", "DISPLAY", version, debugSet);

                    //Unknown option
                    default:
                        if (debugSet) cout << "Unknown option" << endl;
                        a += 1;
                        continue;
                }
            }
            else {
                inFile = option;
                return useImage(inFile, "", "DISPLAY", version, debugSet);
            }
        }
    }

    return 0;
}
