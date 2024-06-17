
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>
#include <unordered_set>

using namespace std;

void build_all(unordered_set<pyramid, hashPyramid> &S);

int main()
{
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
        cout << "The puzzle could not be solved." << endl;

    return 0;
}

int qmain()
{
    unordered_set<pyramid, hashPyramid> allPyramides;

    build_all(allPyramides);

    cout << "In total there are " << allPyramides.size() << " pyramides:" << endl << endl;
    
    if(allPyramides.size() < 20)
        for(pyramid p: allPyramides)
            cout << p << endl << endl << endl;

    return 0;
}

void build_all(unordered_set<pyramid, hashPyramid> &S)
{
    list<Operation> searchOps = {OP_RIGHT_CORNER_UP, OP_UPPER_RIGHT};
    list<pyramid> q = {pyramid("b9,g9,y9,r9")};

    size_t notifyStepSize = 100000;
    size_t lastNotification = 0;

    while(!q.empty())
    {
        pyramid p = q.front();
        q.pop_front();
        S.insert(p);

        for(Operation &op: searchOps)
        {
            pyramid pp(p);

            executeOperation(pp, op);

            if(!S.contains(pp))
            {
                q.push_back(pp);
                S.insert(pp);

                if(S.size() > lastNotification + notifyStepSize)
                {
                    cout << "So far " << S.size() << " pyramids were found." << endl;
                    lastNotification = S.size();
                }
            }
        }
    }
}
