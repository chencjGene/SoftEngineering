#include<string>
#include<assert.h>

using namespace std;

// Store integer and fraction using one data class.
class CFraction
{
private:
    int m_int;
    int m_num;
    int m_deno;
    //replace m_type with function getType()

    int getGCD( int a , int b )
    {
        //assume b!=0
        //this function is private.
        int minus = a < 0 ? -1:1;
        a = a * minus;
        int r = a % b;
        while( r != 0 )
        {
            a = b;
            b = r;
            r = a % b;
        }
        return b;
    }
    
    int getLCM( int a , int b )
    {
        //this function is private.
        //only called between two denominators. so a and b is no-zero.
        int iGCD = getGCD( a , b );
        return ( a / iGCD * b ) ;
    }

    void getImproperFraction( const int& Iint, const int& Inum, const int& Ideno, int& num, int& deno)
    {
        //this function is private
        num = Iint * Ideno + Inum ;
        deno = Ideno;
    }

public:
    CFraction()
    {
        setValue(0, 0, 1);
    }

    CFraction(const int& Iint, const int& Inum, const int& Ideno )
    {
        setValue( Iint, Inum, Ideno); 
    }

    void setValue( const int& Iint, const int& Inum, const int& Ideno )
    {
        if ( Ideno == 0 )
        {
            //TODO : Here we can throw a error
            throw Ideno;
        }
        m_int = Iint;
        m_num = Inum;
        m_deno = Ideno;
        reduction();
    }

    bool isInt()
    {
        //reduction that guarating it is in lowest term.
        //TODO : maybe it can be omit because other restrictions.
        reduction();
        if ( m_num == 0 )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    
    void reduction()
    {
        //case when deno is minus
        if ( m_deno < 0 )
        {
            m_deno = -m_deno;
            m_num = -m_num;
        }
        //reduction 
        int GCD = getGCD( m_num, m_deno);
        m_num /= GCD;
        m_deno /= GCD;
        m_int += m_num / m_deno;
        m_num -= m_num/m_deno * m_deno;
        //what if m_num is minus
        if ( m_num < 0 )
        {
            //m_num must be great than minus m_deno because operations above.
            m_num += m_deno;
            m_int -= 1;
        }
        //set denominator to 1 if num is zero to reduce computation.
        if ( m_num == 0 )
        {
            m_deno = 1;
        }
    }

    bool equalToFraction( const CFraction& temp )
    {
        //TODO ; assume temp is in lowest terms.
        if ( ( m_int == temp.m_int ) && ( m_num == temp.m_num ) && ( m_deno = temp.m_deno ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    //TODO : there two possible solutions. We can deal with
    //according two fractions are integer or not.
    //alse we can transform it into improper form.
    //here we implement the latter one.
    CFraction operator + ( const CFraction& temp )
    {
        int num1, num2, deno1, deno2;
        getImproperFraction( m_int, m_num, m_deno, num1, deno1 );
        getImproperFraction( temp.m_int, temp.m_num, temp.m_deno, num2, deno2 );
        //outDeno is two denos' LCM
        int outDeno = getLCM( deno1, deno2 );
        int outNum = num1 * outDeno / deno1 + num2 * outDeno / deno2 ;
        //reduction of the result can be executed in reduction function.
        return CFraction( 0, outNum, outDeno );
    }

    CFraction operator - ( const CFraction& temp )
    {
        int num1, num2, deno1, deno2;
        getImproperFraction( m_int, m_num, m_deno, num1, deno1 );
        getImproperFraction( temp.m_int, temp.m_num, temp.m_deno, num2, deno2 );
        //outDeno is two denos' LCM
        int outDeno = getLCM( deno1, deno2 );
        int outNum = num1 * outDeno / deno1 - num2 * outDeno / deno2 ;
        //reduction of the result can be executed in reduction function.
        return CFraction( 0, outNum, outDeno );
    }

    CFraction operator * ( const CFraction& temp )
    {
        int num1, num2, deno1, deno2;
        getImproperFraction( m_int, m_num, m_deno, num1, deno1 );
        getImproperFraction( temp.m_int, temp.m_num, temp.m_deno, num2, deno2 );
        //outDeno is two denos' LCM
        int outDeno = deno1 * deno2;
        int outNum = num1 * num2;
        //reduction of the result can be executed in reduction function.
        return CFraction( 0, outNum, outDeno );
    }

    CFraction operator / ( const CFraction& temp )
    {
        int num1, num2, deno1, deno2;
        getImproperFraction( m_int, m_num, m_deno, num1, deno1 );
        getImproperFraction( temp.m_int, temp.m_num, temp.m_deno, num2, deno2 );
        //outDeno is two denos' LCM
        int outDeno = num2 * deno1;
        int outNum = num1 * deno2;
        //reduction of the result can be executed in reduction function.
        return CFraction( 0, outNum, outDeno );
    }
/*
    void operator = ( const CFraction& temp )
    {
        m_int = temp.m_int;
        m_num = temp.m_num;
        m_deno = temp.m_deno;
    }
*/
    bool operator == ( const CFraction& temp )
    {
        return equalToFraction( temp );
    }

    string getString()
    {
        string s;
        if ( m_int != 0 )
        {
            s += to_string( m_int );
        }
        if ( ! isInt() )
        {
           if ( m_int != 0 )
           {
                s += '\'';
           }
           s += to_string( m_num );
           s += '|';
           s += to_string( m_deno);
        }
        if ( s.empty() )
        {
            s = '0';
        }

        return s;
    }
};
