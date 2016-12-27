#include "CFraction.h"
#include<assert.h>
#include<list>
using namespace std;

const char number = '8';
const char expressionBegin = '1';
const char expressionEnd = '9';
class CToken
{
private:
    char m_op;
    CFraction m_frac;

public:
    CToken( const char& ch )
    {
        m_op = ch;
    }

    CToken( const CFraction& frac )
    {
        m_frac = frac;
        m_op = number;
     }

    bool isFraction()
    {
        if ( m_op == number )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    char getOp()
    {
        //constructor guaratee that m_op exist;
        return m_op;
    }

    CFraction getFrac()
    {
        assert ( m_op == number );
        return m_frac; 
    }
};

class CMode
{
private:
    list<CToken> expression;
    // takkInFinished indicate that taking in fraction and operator process is finished
    //we seperate reading process and taking in process to avoid unnecessary error.
    bool takeInFinished;
    list<CToken>::iterator next, prvs;
public:
    CMode()
    {
        clear();
    }

    void takeInFraction( const CFraction& frac )
    {
        assert (  takeInFinished == false );
        expression.push_back( CToken (frac) );
    }

    void takeInOperator( const char& ch )
    {
        assert (  takeInFinished == false );
        expression.push_back( CToken( ch ) );
    }

    void clear()
    {
        expression.clear();
        //set a begin indication for later convenience.
        expression.push_back( CToken( expressionBegin ) );
        //clear mean this class can take in new expression agian
        //so here we set takeInFinished false.
        takeInFinished = false;
    }

    void setTakeInFinished()
    {
        takeInFinished = true;
        expression.push_back ( CToken( expressionEnd ) );
        readInit();
    }

    void readInit()
    {
        prvs = expression.begin();
        next = expression.begin();
        next ++;
    }

    CToken getToken()
    {
        assert ( takeInFinished );
        if ( next == expression.end() )
        {
            return CToken(expressionEnd);
        }
        else
        {
            prvs = next;
            next ++;
            return *prvs;
        }
    }

    void pushToken()
    {
        assert ( takeInFinished );
        assert ( prvs != expression.begin() );
        //we do not push anything if it is the end of the expression.
        if ( next ==  expression.end() )
        {
            return;
        }
        //push a Token means iterator go back a site.
        next --;
        prvs --;
        //expresstion.begin can not use -- operate.
		// this situation will not accur when calculating a expression
        assert ( prvs != expression.begin() );
    }

    void print()
    {
        for ( list<CToken>::iterator it = expression.begin();it != expression.end();++it )
        {
            CToken t = *it;
            if ( t.isFraction() )
            {
                cout<<t.getFrac().getString();
            }
            else
            {
                char ch = t.getOp();
                if ( ch != expressionBegin )
                {
                    cout<<ch;
                }

            }
        }
        cout<<endl;
    }

    string getString()
    {
        string s;
        s.clear();
        for ( list<CToken>::iterator it = expression.begin();it != expression.end();++it )
        {
            CToken t = *it;
            if ( t.isFraction() )
            {
                s = s + t.getFrac().getString() ;
            }
            else
            {
                char ch = t.getOp();
                if ( ch != expressionBegin )
                {
                    s = s + ch ;
                }

            }
        }
        s = s + '\n' ;
        return s;
    }
};

bool isInteger( const char& ch )
{
    if ( ch >= '0' && ch <= ( '0' + 9 ) ) 
    {
        return true;
    }
    else
    {
        return false;
    }
}
// if ch is one of { 0~9, ' , | , ( }
bool isFraction( const char& ch )
{
    if ( isInteger( ch ) || ch == '\'' || ch == '|' || ch == '(' )
    {
        return true;
    }
    else
    {
        return false;
    }


}
bool isEnd( const char& ch )
{
    if ( ch == '=' )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isOperator( const char& ch )
{
    if( ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ')' )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool readExpression( string s, CMode& expression)
{
    if ( s.empty() )
    {
        return false;
    }
    //clear expression to take in new expression
    expression.clear();
    bool prvsIsFraction = false;
    bool prvsIsInteger;
    // store three integer of a fraction
    int rgNum[3];
    //handle chars of s in order
    for ( string::iterator ich = s.begin(); ich != s.end(); ++ich )
    {
        char ch = *ich;
        // if ch is one of { 0~9, ' , | , ( }
        if ( isFraction(ch) )
        {
            if ( prvsIsFraction )
            {
                return false;
            }
            if ( ch == '(' )
            {
                expression.takeInOperator( ch );
                //do not cahnge prvs state because a fraction is wanted after '('
                continue;
            }
            int i = 0;
            prvsIsInteger = false;
            while ( isFraction( ch ) )
            {
                // a fraction contain no more than 3 int
                if ( i >= 3 )
                {
                    return false;
                }
                if  ( isInteger( ch ) )
                {
                    if ( prvsIsInteger )
                    {
                        //in case number is lager than 10
                        rgNum[i] = rgNum[i] * 10 + ch - '0';
                    }
                    else
                    {
                        //ch - '0' means turn a char number into a int number
                        rgNum[i] = ch - '0';
                        prvsIsInteger = true;
                    }
                }
                else
                {
                    // ' should be after the first int and | should be after the second integer
                    if ( ( ch == '\'' && i == 0 ) || ( ch == '|' && i == 1 ) )
                    {
                        i++;
                        //set prvsIsInteger to indicate that a new int is comming
                        prvsIsInteger = false;
                    }
                    // what if expression is given in 1|2 form
                    else if ( ch == '|' && i == 0 )
                    {
                        rgNum[1] = rgNum[0];
                        rgNum[0] = 0;
                        i = 2;
                        prvsIsInteger = false;
                    }
                    else if ( isOperator( ch ) )
                    {
                        continue;
                    }
                    else
                    {
                        return false;
                    }
                }
                ++ich;
                ch = *ich;
            }
            if ( i == 0 )
            {
                //set rest of array();
                rgNum[1] = 0;
                rgNum[2] = 1;
                // serve as a flag show that accept a correct fraction
                i = 2;
            }
            if ( i == 2 )
            {
                expression.takeInFraction( CFraction( rgNum[0], rgNum[1], rgNum[2] ) );               
                prvsIsFraction = true;
            }
            else
            {
                return false;
            }
        }
        //if ch is an operator 
        //here else if is not wanted because it will skip operator after fraction.
        //
        if ( isOperator( ch ) )
        {
            assert ( prvsIsFraction == true );
            if ( ch == ')' )
            {
                expression.takeInOperator( ch );
                //do not cahnge prvs state because a operator is wanted after ')'
                continue;
            }
            else
            {
                expression.takeInOperator( ch );
                //reset prvsIsFraction()
                prvsIsFraction = false;

            }
        }
        else if ( isEnd( ch ) )
        {

        }
        else
        {
            return false;
        }
    }
}

CFraction Expression(CMode& expression);

CFraction Primary( CMode& expression )
{
    CToken t = expression.getToken();
    if ( t.getOp() == '(' )
    {
        CFraction temp = Expression( expression );
        t = expression.getToken();
        if ( t.getOp() == ')' )
        {
            return temp;
        }
        else
        {
            throw (0);
        }
    }
    else if ( t.getOp() == number )
    {
        return t.getFrac();
    }
    else
    {
        throw(0);
    }
}

CFraction Term( CMode& expression )
{
    CFraction left = Primary( expression );
    CToken t('0');
    while(1)
    {
        t = expression.getToken();
        if ( t.getOp() == '*' )
        {
            left = left * Primary( expression );
        }
        else if ( t.getOp() == '/' )
        {
            left = left / Primary( expression );
        }
        else
        {
            expression.pushToken();
            return left;
        }
    }
}


CFraction Expression( CMode& expression )
{
    CFraction left = Term( expression );
    CToken t('0');
    while(1)
    {
        t = expression.getToken();
        if ( t.getOp() == '+' )
        {
            left = left + Term( expression );
        }
        else if ( t.getOp() == '-' )
        {
            left = left - Term( expression );
        }
        else
        {
            expression.pushToken();
            return left;
        }
    }
}