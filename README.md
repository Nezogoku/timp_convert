# timp_convert
A small program that utilizes the libTIMP library and SFML to both view and convert LocoRoco tip and cip files.


    USAGE: timp_convert [options] <file(s).tip>
    Options:
        -h                            prints all commands, ignores all other commands
        -d                            activates debug mode
        -v <infile.tip>               opens TIP file for viewing, debug loads it in chunks
        -b [XXX] <infile(s).tip>      batch converts all TIP files to specified image type XXX, default is PNG
        -c <infile.tip> [<outfile>]   converts TIP file to new image, default is PNG
    
    If debug and display are activated,
    ENTER prints a chunk of the image being viewed

    Theoretically supports converting to:
        BMP
        PNG
        TGA
        JPG
