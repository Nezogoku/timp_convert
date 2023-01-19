# timp_convert
A small program that utilizes the libTIMP library and SFML to both view and convert LocoRoco tip and cip files.

    
    USAGE: timp_convert [options] <file(s).tip/cip>

    Options:
        -h                                  Prints all commands, ignores all other commands
        -d                                  Activates debug mode
        -v <infile>                         Opens TIMP file for viewing, default option
        -b [XXX] [V] <infile(s)>            Batch converts all TIP files to specified image type XXX, default is PNG
                                                optional version type V for appropriate images, default is 0
        -c <infile> [<outfile>] [V]         Converts TIP file to new image, default is PNG
                                                optional version type V for appropriate images, default is 0

    Supported formats include:
        BMP DIB
            Windows (Device-Independent) Bitmap
        PNM PBM PGM PPM
            Netpbm Portable Anymap Format, versions 1 through 6 correspond with:
                Portable Bitmap Format, versions 1 (ASCII) and 4 (Binary)
                Portable Greymap Format, versions 2 (ASCII) and 5 (Binary)
                Portable Pixmap Format, versions 3 (ASCII) and 6 (Binary)
        PAM
            Netpbm Portable Arbitrary Map
        JPG JPEG JPE JIF JFIF JFI
            Joint Photographic Experts Group format
        ICO
            Windows Computer Icon
        CUR
            Windows Mouse Cursor, versions 1 through 6 correspond with:
                Auto-generated hotspot, version 1
                Centred hotspot, version 2
                Top-left hotspot, version 3
                Top-right hotspot, version 4
                Bottom-left hotspot, version 5
                Bottom-right hotspot, version 6
        PNG
            Portable Network Graphics format
