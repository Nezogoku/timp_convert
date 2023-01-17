# timp_convert
A small program that utilizes the libTIMP library and SFML to both view and convert LocoRoco tip and cip files.


    USAGE: convertTIMP [options] <file(s).tip/cip>
    Options:
        -h                                  Prints all commands, ignores all other commands
        -d                                  Activates debug mode
        -v <infile>                         Opens TIMP file for viewing, default option
        -b [XXX] [V] <infile(s)>            Batch converts all TIP files to specified image type XXX, default is PNG
                                                optional version type V for appropriate images, default is 0
        -c <infile> [<outfile>] [V]         Converts TIP file to new image, default is PNG
                                                optional version type V for appropriate images, default is 0

    Supports converting to:
        BMP, PBM, PGM, PPM, PAM, JPG, PNG
