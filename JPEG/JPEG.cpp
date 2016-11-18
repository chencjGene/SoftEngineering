// JPEG.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<fstream>
#include<list>

#include "constant.h"
#include "RLEClass.h"
#include "DCT.h"
#include "universe.h"
using namespace std;


int** readImage( string filename, int &imgHigh, int &imgWidth )
{
    /*
    Open files that contain image information.
    
    Parameter:
        filename : it is "lady.bat" there.
        imgHigh : high of image, it will be changed by calling this function.
        imgWidth : width of image, it will be changed by calling this function.
    */

    if ( !isBatFile( filename ) )
    {
        throw exception( "we can not deal with file except .bat now." );
    }
    //.bat file do not include image's size information. it required additive information to be given as macro form.
    else
    {
        imgHigh = BATHIGH;
        imgWidth = BATWIDTH;
        //Should set the file's readform as binary.
        ifstream datafile( "lady.dat", ios_base::binary );
        unsigned char temp[ BATHIGH * BATWIDTH ];
        if ( !datafile )
        {
            throw exception( "file does not exist." );
        }
        for ( int i = 0; i < BATHIGH * BATWIDTH ; i++ )
        {
            // we use get() instead of >> because >> will ingore 32, namely 'space' .
            temp[i] = datafile.get();
        }
        int **imgData = NULL;
        imgData = newIntMatrix( imgHigh, imgWidth );
        for ( int i = 0 ; i < imgHigh ; ++i )
            for ( int j = 0 ; j < imgWidth ; ++j )
            {
                imgData[i][j] = temp[i * imgHigh + j];
            }
        return imgData;
    }
}

void cutDataIntoBlock( int **imgData, int block[][BLKSIZE], const int& imgHigh, const int& imgWidth, const int& ithBlock, const int& jthBlock )
{
    /*
    cut data into smaller block for latter transform.
    
    Parameter:
        imgData : head of image data.
        block : that store small block data cut from imgData.
        imgHigh :
        imgWidth :
        ithBlock : beginning index of block in 0-axis
        jthBlock : beginning index of block in 1-axis
    */
    //TODO ; this condition is just suitable for sizes of 8's times.
    if ( ( ithBlock >= imgHigh / BLKSIZE ) || ( jthBlock >= imgWidth / BLKSIZE ) )
    {
        throw exception( "index's out of range." );
    }
    for ( int i = 0 ; i < BLKSIZE ; i++ )
        for ( int j = 0 ; j < BLKSIZE ; j++ )
        {
            if ( i > ( imgHigh - ithBlock * 8 - 1 ) || j > ( imgWidth - jthBlock * 8 - 1 ) )
            {
                block[i][j] = 0;
            }
            else
            {
                block[i][j] = imgData[ithBlock * 8 + i][jthBlock * 8 + j] - 128;
            }
        }
}




void Quantization( double DCTCoe[][BLKSIZE] , int DCTQuanCoe[][BLKSIZE] )
{
    /*
    Quantizing the DCT coefficients according quantization matrix.

    Parameter:
        DCTCoe : a matrix containing DCT coefficients
        DCTQuanCoe : a matrix to store the quantization result.
    */
    for ( int i = 0 ; i < BLKSIZE ; i++ )
        for ( int j = 0 ; j < BLKSIZE ; j++ )
        {
            double temp =  DCTCoe[i][j] / QuanCoe[i][j];
            DCTQuanCoe[i][j] = (int)( temp + ( temp >= 0 ? roundRate : 0 - roundRate ) );
        }
}

list<int> zigzag( int DCTQuanCoe[][BLKSIZE] )
{
    /*
    Do the zigzag job in JPEG. It return a list as result.

    Parameter:
        DCTQuanCoe : a matrix containing the quantized DCT coefficients.
    */
    list<int> zigzagList;
    list<int>::iterator it;
    int lastNonZeroP = BLKSIZE * BLKSIZE - 1 ;
    //find last nonzero number's position.
    for( ; lastNonZeroP >= 0 ; lastNonZeroP-- )
    {
        if( DCTQuanCoe[ zigzagX[lastNonZeroP] ][ zigzagY[lastNonZeroP] ] != 0 )
        {
            break;
        }
    }
    for ( int i = 0; i <= lastNonZeroP ; i++ )
    {
        zigzagList.push_back( DCTQuanCoe[ zigzagX[i] ][ zigzagY[i] ] );
    }
    return zigzagList;
}

list<RLEClass> RLE( list<int> zigzagList, int prvsDC )
{
    /*
     Run length coding.

     Parameter:
        zigzagList : the result of zigzag as a list.
        prvsDC : prvious dirrect current.
    */
    list<RLEClass> RLEList;
    list<int>::iterator it = zigzagList.begin();
    //deal with DC
    int sizeDC = (int)( log( abs ( *it - prvsDC ) ) / log( 2 ) + 1 );
    //-1 inidcate DC
    RLEList.push_back( RLEClass( -1, sizeDC, *it - prvsDC ) );
    it++;
    int zeroCount = 0;
    for ( ; it != zigzagList.end() ; it++ )
    {
        if ( *it == 0 )
        {
            zeroCount ++;
            //we should cutoff zeros if there are more than zeroMaxLength(15) zeros.
            if ( zeroCount > zeroMaxLength )
            {
                RLEList.push_back( RLEClass( zeroMaxLength, 0, 0 ) );
                //we cutoff when there are 16 zeros continuously, so we start count at 1.
                zeroCount = 1;
            }
        }
        else
        {
            int sizeAC = (int)( log( abs( *it ) ) / log( 2 ) + 1 );
            RLEList.push_back( RLEClass( zeroCount, sizeAC, *it ) );
            zeroCount = 0;
        }
    }
    //0 indicate the end of list
    if( zigzagList.size() < 63)
    {
        RLEList.push_back( RLEClass(0, 0, 0 ) );
    }
    return RLEList;
}

string valueCode( const int& value )
{
    /*
    amplitude encoding.

    Parameter:
        value : value to be encoded.
    */

    //0 return nothing
    if ( value == 0 ) 
    {
        return "";
    }
    int bit = (int)( log( abs( value ) ) / log( 2 ) );
    string s;
    int absValue;
    //if value less than zero, the first bit of code is zero.
    if( value < 0 )
    {
        //scale the value to 0 ~ 2^bit
        absValue = value + pow( 2, bit + 1 ) - 1;
        s += '0';
    }
    else
    {
        //scale the value to 0 ~ 2^bit
        absValue = value - pow( 2, bit );
        s += '1';
    }
    unsigned int deltaValue = (int) ( absValue );
    //encode the rest bits.
    for ( int i = 0; i < bit ; i++ )
    {
        unsigned int temp = 1<< ( bit - i - 1 );
        s += ( ( (temp&deltaValue)>0 )?'1':'0' );
    }
    return s;
}

string enCode( list<RLEClass> RLEList )
{
    /*
    encoding the result of run length encoding to bit stream.

    Parameter:
        RLEList : the result of RLE as a list.
    */

    //deal with DC
    list<RLEClass>::iterator it = RLEList.begin();
    string s;
    s += DCCodeTable[ (*it).second ];
    s += valueCode( (*it).third );
    for ( ++it ; it != RLEList.end(); it++ )
    {
        s += ACCodeTable[ (*it).first ][ (*it).second ];
        s += valueCode( (*it).third );
    }
    return s; 
}


void test()
{
    /*
    This is a test function. It using the test data containing in " constant.h " and can show the intermediate result of the JEPG.  
    */
    double DCTCoe[BLKSIZE][BLKSIZE];
    int DCTQuanCoe[BLKSIZE][BLKSIZE];
    for( int i = 0; i < BLKSIZE; i++ )
        for ( int j = 0; j < BLKSIZE; j++ )
        {
            testBlock[i][j] -= 128;
        }
    dct( testBlock, DCTCoe );
    for ( int i = 0 ; i < BLKSIZE ; i++ )
    {
        for ( int j = 0 ; j < BLKSIZE ; j++ )
        {
            cout<<DCTCoe[i][j]<<"  ";
        }
        cout<<'\n'<<endl;
    }
    Quantization( DCTCoe , DCTQuanCoe );
    for ( int i = 0 ; i < BLKSIZE ; i++ )
    {
        for ( int j = 0 ; j < BLKSIZE ; j++ )
        {
            cout<<DCTQuanCoe[i][j]<<"  ";
        }
        cout<<endl;
    }
    list<int> zigzagList = zigzag( DCTQuanCoe );
    list<RLEClass> RLEList = RLE( zigzagList, 12 );
    for( list<RLEClass>::iterator it = RLEList.begin() ; it != RLEList.end() ; it++ )
    {
        ( *it ).Print();
    }
    cout<<endl;
    cout<<enCode( RLEList ) << endl;
    cout<<valueCode(-1023);
}

string paddingZeros( string tempString )
{
    string s;
    int count = 0;
    int finalCount = 0;
    for ( int i = 0; i < tempString.length(); i++ )
    {
        s += tempString[i];
        finalCount++;
        if( tempString[i] == '0' )
        {
            count = 0;
        }
        else
        {
            count++;
        }
        if( ( count >= 8 ) && ( finalCount % 8 ) == 0 )
        {
            s += "00000000";
            finalCount += 8;
            count = 0;
        }
    }
    return s;
}


void writeJPEG( string filename, string finalString)
{
    ofstream outfile( filename, ios_base::binary );
    if( !outfile )
    {
        throw( "outfile cannot be created." );
    }
    int target;
    char targetChar;
    unsigned char temp;
    for ( int i = 0; i < ( finalString.length() / 8 ); i++ )
    {
        target = 0;
        for ( int j = 0; j < 8; j++ )
        {
            temp = ( finalString[ i * 8 + j ] - '0' ) ;
            target += ( temp << ( 7 - j ) ) ;
        }
        targetChar = (char)target;
        outfile << targetChar;
    }

    if ( ( finalString.length() % 8 ) != 0 )
    {
        target = 0;
        for ( int j = 0; j < ( finalString.length() - ( finalString.length() / 8 * 8 ) ); j++ )
        {
            temp = finalString[ ( finalString.length() / 8 * 8 ) + j ] - '0';
            target += ( temp << ( 7 - j ) ) ;
        }
    }
    targetChar = (char)target;
    outfile << targetChar;
    outfile.close();
}

int _tmain(int argc, _TCHAR* argv[])
    {
    try{
        int imgHigh, imgWidth;
        int **imgData = readImage( "lady.dat", imgHigh, imgWidth );
        int block[BLKSIZE][BLKSIZE];
        double DCTCoe[BLKSIZE][BLKSIZE];
        int DCTQuanCoe[BLKSIZE][BLKSIZE];
        int prvsDC = 0;
        string tempString;
        string finalString;
        for( int ithBlock = 0; ithBlock < imgHigh / BLKSIZE; ithBlock++ )
        {
            for( int jthBlock = 0; jthBlock < imgWidth / BLKSIZE ; jthBlock++ )
            {
                cutDataIntoBlock( imgData, block, imgHigh, imgWidth, ithBlock, jthBlock );
                DCT( block, DCTCoe );
                Quantization( DCTCoe , DCTQuanCoe);
                list<int> zigzagList = zigzag( DCTQuanCoe );
                list<RLEClass> RLEList = RLE( zigzagList, prvsDC );
                string tempS = enCode( RLEList );
                tempString += tempS;
                prvsDC = DCTQuanCoe[0][0];
            }
        }
        cout<<"tempLength:"<<tempString.length()<<endl;
        finalString.clear();
        finalString = paddingZeros(tempString);
        cout<<"finalLength:"<<finalString.length()<<endl;
        writeJPEG("1.dat", finalString );
        ofstream txtFile("jpg.txt");
        txtFile<<finalString;
        txtFile.close();
    }
    catch(exception &e)
    {
        cerr<<e.what()<<endl;
    }
    system("pause");
	return 0;
}

