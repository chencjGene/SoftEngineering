#include<iostream>
#include<fstream>
#include<time.h>
#include "Token.h"
using namespace std;
#define N 1

char g_Op[4] = {'+', '-', '*', '/'};
CFraction randFraction( int arange )
{
    //generating a random fraction.
    //all fractions presented in expression is in lowest terms. here we do not take reduction into acount.
    int rgNum[3];
    rgNum[0] = rand() % arange;
    // in case denominator is zero
    rgNum[2] = rand() % ( arange - 1 ) + 1;
    // in case number is large than denominator which might make integer larger than arange. for example 9'8|4 = 11 when arange is 10
    rgNum[1] = rand() % ( rgNum[2] );
    return CFraction( rgNum[0], rgNum[1], rgNum[2] );
}

CMode randEquation( int arange, int opNum )
{
    int randOp;
    CFraction randFrac;
    CMode expression;
    expression.clear();
    for ( int i = 0 ; i < opNum ; ++i )
    {
        randOp = rand() % 4;
        randFrac = randFraction( arange );
        expression.takeInFraction( randFrac );
        expression.takeInOperator( g_Op[randOp] );
    }
    randFrac = randFraction( arange );
    expression.takeInFraction( randFrac );
    return expression;
}

void productEquation( int amount, int arange, int max_length, string filename)
{
    //TODO : cannot generate equations with brackets.
    //producting an equation 
    //
    //Parameter:
    //  amount : number of equation wanted
    //  arange : max integer in equation (including result)
    //  filename : equation will be save as .txt file.
    ofstream outfile(filename);
    if( !outfile )
    {
        cout<< filename << "can't be opened!!!"<<endl;
        return ;
    }
    string s;
    CFraction t;
    CMode expression;
    for ( int i = 0; i < amount; ++i )
    {
        // rand()%max_length +1 in case generated expression is a fraction.
        expression = randEquation( arange , rand() % max_length + 1 ) ;
        s = expression.getString();
        outfile << s ;        
    }
    outfile.close();
    cout<< amount << " expression are saved in " << filename << endl ;
}

void evaluateEquationFile( string exerciseFilename, string answerFilename, string gradeFilename )
{
    // evaluating final grade given answer.
    // answer must be in lowest term
    //Parameter:
    //  exerciseFile : 
    //  answerFile :
    //  gradeFile : 
    ifstream exerciseFile( exerciseFilename );
    if ( !exerciseFile )
    {
        cout<< exerciseFilename << "does not exist!!!"<<endl;
        return ;
    }
    ifstream answerFile( answerFilename );
    if ( !answerFile )
    {
        cout<< answerFilename << "does not exist!!!"<< endl;
        return ;
    }
    ofstream gradeFile(gradeFilename);
    if ( !gradeFile )
    {
        cout<< gradeFilename << " cannot create!!! " << endl;
        return;
    }
    string exercise;
    string answer;
    CMode expression;
    while( getline( exerciseFile, exercise ) )
    {
        getline( answerFile , answer );
        expression.clear();
        //a expression must end with a '=' 
        //TODO : this part should be write into CMode::function.
        readExpression( exercise + '=', expression );
        expression.setTakeInFinished();
        if ( Expression( expression ).getString() == answer )
        {
            gradeFile << "1\n";
        }
        else
        {
            gradeFile << "0\n";
        }
    }

    exerciseFile.close();
    answerFile.close();
    gradeFile.close();
}
int main(int argc, char *argv[])
{
    
    srand( ( unsigned )time( NULL ) );
    int equationNum = 20;
    int equationRange = 10;
    string exerciseFilename;
    string answerFilename;
    bool isEvaluating = false;
    bool isGenerating = false;
    for ( int i = 1; i<argc; ++i )
    {
        string s = argv[i];
        if ( s[0] = '-' && s.length() == 2 )
        {
            i = i + 1;
            switch ( s[1] )
            {
            case 'n':
                equationNum = atoi( argv[i] );
                isGenerating = true;
                break;
            case 'r':
                equationRange = atoi ( argv[i] );
                isGenerating = true;
                break;
            case 'e':
                exerciseFilename = argv[i];
                isEvaluating = true;
                break;
            case 'a':
                answerFilename = argv[i];
                isEvaluating = true;
                break;
            default:
                cout<< "parameters were given in wrong form. Please read README.md carefully!"<<endl;
                return 1;
            };
        }
        else
        {
            cout<< "parameters were given in wrong form. Please read README.md carefully!"<<endl;
            break;
        }
    }
    if ( isEvaluating == isGenerating )
    {
        cout<< "You can not evaluate when generating equation" <<endl;
        //system("pause");
        return 1;
    }
    else if ( isGenerating )
    {
    cout<<"The number of expression you want to generate is "<<equationNum<<endl;
    cout<<"The range of expression is "<<equationRange<<endl;
    productEquation( equationNum, equationRange, 5, "E:\\SoftwareEngineering/Chapter_1/outEquation.txt" );
    }
    else
    {
    evaluateEquationFile(exerciseFilename, answerFilename, "gradeFile.txt");
    }
    //system("pause");
    return 0;
}
