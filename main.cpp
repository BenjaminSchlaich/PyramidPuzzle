
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <fstream>

using namespace std;

void build_graph(vector<pyramid> &nodes, vector<list<size_t>> &G);

void testSolve()
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

    return;
}

void createAndStoreNodes()
{
    vector<pyramid> nodes;
    vector<list<size_t>> edges;

    build_graph(nodes, edges);

    cout << "In total there are " << nodes.size() << " pyramides." << endl;
    
    ofstream savefile("nodes.txt");

    if(!savefile.good())
        throw runtime_error("Could not open file to store pyramid nodes.");
    
    cout << "Saving the pyramids to file." << endl;

    for(pyramid &p: nodes)
        savefile << p.storageString() << endl;
    
    savefile.close();

    ofstream edgesfile("edges.txt");

    if(!edgesfile.good())
        throw runtime_error("Could not open file to store finished pyramid nodes.");
    
    cout << "Saving the edges to file." << endl;

    for(auto &l: edges)
    {
        size_t i=0;

        for(size_t id: l)
        {
            edgesfile << id;
            i++;

            if(i < l.size())
                edgesfile << ',';
        }

        edgesfile << endl;
    }
    
    edgesfile.close();

    cout << "Everything saved and closed." << endl;

    return;
}

void loadNodes(vector<pyramid> &ps)
{
    ifstream ifs("nodes.txt");

    if(!ifs.good())
        throw runtime_error("Could not read nodes from file.");
    
    ps.clear();

    while(ifs.good())
    {
        string s;

        getline(ifs, s);

        if(ifs.eof())       // there is one last newline which leads to an empty string that cannot be parsed.
            break;

        ps.push_back(pyramid(s));
    }

    ifs.close();
}

void loadEdges(vector<list<size_t>> &g)
{
    ifstream ifs("edges.txt");

    if(!ifs.good())
        throw runtime_error("Could not read nodes from file.");
    
    g.clear();

    while(ifs.good())
    {
        string s;

        getline(ifs, s);

        if(ifs.eof())                   // there is one last newline which leads to an empty string that cannot be parsed.
            break;

        size_t comma = s.find(',');     // this should never return npos, as there always are two rotational neighbors
        
        g.push_back({stoul(s.substr(0, comma)), stoul(s.substr(comma+1, s.length() - comma + 1))});
    }

    ifs.close();
}

void generate(unordered_set<pyramid> &S)
{
    Operation ops[] =   {OP_UPPER_RIGHT,OP_UPPER_LEFT,OP_RIGHT_UP,OP_RIGHT_DOWN
                        ,OP_LEFT_UP,OP_LEFT_DOWN,OP_BACK_CLOCKWISE,OP_BACK_COUNTER_CLOCKWISE};

    S.clear();
    S.insert(pyramid("b9,g9,y9,r9"));

    std::cout << "starting the generation of pyramids..." << std::endl;

    do
    {
        for(const pyramid &p: S)
        {
            if(p.marked)
                continue;

            bool marked = true;

            for(Operation &op: ops)
            {
                pyramid pp(p);

                executeOperation(pp, op);

                if(!S.contains(pp))
                {
                    S.insert(pp);
                    marked = false;
                }
            }

            p.marked = marked;
        }

    } while(S.size() < 933120);
    
    cout << S.size() << " different pyramids were generated." << endl;

    return;
}

int main()
{
    // runAllTests();

    unordered_set<pyramid> ps;

    generate(ps);

    return 0;
}

void build_graph(const vector<pyramid> &nodes, vector<list<size_t>> &G)
{
    list<Operation> searchOps = 
                { OP_NOOP
                , OP_TURN_LEFT, OP_TURN_RIGHT, OP_RIGHT_CORNER_UP, OP_RIGHT_CORNER_DOWN, OP_LEFT_CORNER_UP, OP_LEFT_CORNER_DOWN
                , OP_UPPER_RIGHT, OP_UPPER_LEFT, OP_RIGHT_UP, OP_RIGHT_DOWN, OP_LEFT_UP, OP_LEFT_DOWN, OP_BACK_CLOCKWISE, OP_BACK_COUNTER_CLOCKWISE
                , OP_RIGHTEST_UP, OP_RIGHTEST_DOWN, OP_TOP_RIGHT, OP_TOP_LEFT};

    pyramid start("b9,g9,y9,r9");
    list<pyramid> q = {start};

    size_t notifyStepSize = 100000;
    size_t lastNotification = 0;
    size_t ID = 0;

    unordered_map<pyramid, size_t, hashPyramid> trans;  // translate pyramids into ids.
    trans.insert({q.front(), ID++});                    // insert the 0-th node

    nodes.clear();
    nodes.push_back(start);

    G.clear();
    G.push_back(list<size_t>());                        // empty edge list for 0-th node

    while(!q.empty())
    {
        pyramid p = q.front();
        q.pop_front();
        size_t pID = trans.at(p);               // we might need this multiple times for edge insertion

        for(Operation &op: searchOps)
        {
            pyramid pp(p);

            executeOperation(pp, op);           // make pp a neighbor pyramid

            if(!trans.contains(pp))
            {
                q.push_back(pp);                // look at the edges from pp later
                nodes.push_back(pp);            // add pp to the discovered nodes
                G.at(pID).push_back(ID);        // insert the edge from p to pp
                trans.insert({pp, ID++});       // assign pp it's new id
                G.push_back(list<size_t>());    // and add an empty edge list for it

                if(trans.size() > lastNotification + notifyStepSize)
                {
                    cout << "So far " << trans.size() << " pyramids were found." << endl;
                    lastNotification = trans.size();
                }
            }
            else
            {
                size_t ppID = trans.at(pp);     // find the id of pp so we can
                G.at(pID).push_back(ppID);      // insert the edge
            }
        }
    }
}
