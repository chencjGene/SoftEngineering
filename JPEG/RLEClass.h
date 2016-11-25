#include<iostream>
using namespace std;
class RLEClass
{
public:
    int first;
    int second;
    int third;
    RLEClass( int a, int b, int c )
    {
        first = a;
        second  = b;
        third = c;
    }
    void Print()
    {
        cout<< '(';
        if ( first >-1 )
        {
            cout<< first << ',';
        }
        cout<<second;
        cout<< ')' << '(' << third << ')' << ',';
    }
};