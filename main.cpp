
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>
#include <unordered_map>
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

int main()
{
    vector<pyramid> pyramids;
    vector<list<size_t>> graph;

    loadNodes(pyramids);

    loadEdges(graph);

    pyramid test("ryrgyyr3,b3yggygg,ybygrrgbb,brbryybgg");
    test.turnRight();

    vector<pyramid>::iterator spot;

    if((spot = find(pyramids.begin(), pyramids.end(), test)) == pyramids.end())
        cout << "Oh no, we're missing some." << endl;
    else
    {
        size_t id = distance(pyramids.begin(), spot);
        cout << "id: " << id << endl;

        cout << "neighbors: " << endl;

        cout << pyramids.at(graph.at(id).front()) << endl;
        cout << pyramids.at(graph.at(id).back()) << endl;
    }
    
    return 0;
}

void build_graph(vector<pyramid> &nodes, vector<list<size_t>> &G)
{
    list<Operation> searchOps = {OP_RIGHT_CORNER_UP, OP_UPPER_RIGHT};
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
