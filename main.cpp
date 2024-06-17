
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>

using namespace std;

int main()
{
    /*
    runAllTests();
    /*/
    //pyramid p("yggrbrgry,byybrgyyr,grbbybygr,bgrbgyrbg");
    pyramid p("rgbrbgbyy,bgyyrrbbg,yrryrgyyg,brrbg3by");
    // pyramid p("yryg3y3,brbbr5,g3y4bb,b4g3rr");
    // pyramid p("b9,g9,y9,r9");

    std::list<Operation> solution;

    if(solve(p, solution))
    {
        cout << "The puzzle was solved like so:" << endl << endl;

        for(auto &move: solution)
            std::cout << operationToString(move) << endl << endl;
    }
    else
        cout << "The puzzle could not be solved." << endl;//*/

    return 0;
}
