//TODO : finishing this funcion.
bool isBatFile(char* s)
{
    /*
    Judging a filename( s ) is a Bat file or not 

    Parameter:
        s : filename of a file.
    */
    return 1;
}

int** newIntMatrix( int length, int width )
{
    /*
    malloc a matrix.

    Parameter:
        length : length of the matrix.
        width : width of the matrix.
    */
    int **p;
    p = new int*[length];
    for ( int i = 0; i < length; i++ )
        p[i] = new int[width];
    return p;
}
