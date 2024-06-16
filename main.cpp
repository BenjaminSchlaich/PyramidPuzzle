
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>

using namespace std;

int main()
{
    runAllTests();

    cout << endl << endl;

    // pyramid p("yggrbrgry,byybrgyyr,grbbybygr,bgrbgyrbg");    
    pyramid p("yryg3y3,brbbr5,g3y4bb,b4g3rr");

    std::list<std::string> solution;

    if(solve(p, solution))
    {
        cout << "The puzzle was solved like so:" << endl;

        for(auto &move: solution)
            std::cout << move << std::endl;
    }
    else
        cout << "The puzzle could not be solved." << endl;

    return 0;
}
