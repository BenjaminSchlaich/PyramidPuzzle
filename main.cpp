
#include "pyramid.hpp"
#include "testpyramid.hpp"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <fstream>

using namespace std;

void findEdges(vector<pyramid> &nodes, vector<list<size_t>> &G);

void testSolve()
{
    pyramid p("yggrbrgry,byybrgyyr,grbbybygr,bgrbgyrbg");
    // pyramid p("rgbrbgbyy,bgyyrrbbg,yrryrgyyg,brrbg3by");
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

// generate all configurations and save them to file
void generateNodes();

// must run generateNodes() fist.
void generateEdges();

void loadNodes(vector<pyramid> &ps);

void loadEdges(vector<list<size_t>> &g);

void solverLoop()
{
    while(true)
    {
        cout << "Enter pyramid puzzle instance (or type 'exit' to exit): ";
        string s;
        getline(cin, s);

        if(s == "exit")
            return;
        
        try
        {
            pyramid p(s);

            list<Operation> solution;

            if(solve(p, solution))
            {
                cout << "The puzzle was solved like so:" << endl << endl;

                for(auto &move: solution)
                    std::cout << operationToString(move) << endl << endl;
            }
            else
                cout << "The puzzle could not be solved." << endl;
        }
        catch(const std::exception& e)
        {
            cout << "An error occurred when trying to solve your problem:" << endl;
            std::cerr << e.what() << endl << endl;
        }
    }

    return;
}

int main()
{
    
}

void findEdges(vector<pyramid> &ps, vector<list<size_t>> &G)
{
    cout << "build_graph()..." << endl;

    list<Operation> ops =   { OP_UPPER_RIGHT, OP_UPPER_LEFT, OP_RIGHT_UP, OP_RIGHT_DOWN
                            , OP_LEFT_UP, OP_LEFT_DOWN, OP_BACK_CLOCKWISE, OP_BACK_COUNTER_CLOCKWISE };

    cout << "loading the nodes from file..." << endl;

    loadNodes(ps);

    cout << "loaded " << ps.size() << " nodes." << endl;

    unordered_map<pyramid, size_t, hashPyramid> trans;  // translate pyramids into ids.
    size_t id = 0;

    cout << "building a translation table..." << endl;

    for(const pyramid &p: ps)
        trans.insert({p, id++});

    cout << "done." << endl << "finding all edges now..." << endl;

    G.clear();
    G = vector<list<size_t>>(ps.size(), list<size_t>());

    size_t percent = ps.size() / 100;
    size_t lastNotification = 0;

    for(size_t id=0; id<ps.size(); id++)
    {
        pyramid &u = ps.at(id);

        for(Operation op: ops)
        {
            pyramid p(u);

            executeOperation(p, op);

            size_t pID = trans.at(p);
            G.at(id).push_back(pID);
        }

        if(id > lastNotification + percent)
        {
            lastNotification = id;
            cout << (id / percent) << " %." << endl;
        }
    }

    cout << "generated the whole graph." << endl;
}

void generateNodes()
{
    cout << "generate()..." << endl;

    Operation ops[] =   {OP_UPPER_RIGHT,OP_UPPER_LEFT,OP_RIGHT_UP,OP_RIGHT_DOWN
                        ,OP_LEFT_UP,OP_LEFT_DOWN,OP_BACK_CLOCKWISE,OP_BACK_COUNTER_CLOCKWISE};

    unordered_set<pyramid, hashPyramid> S = {pyramid("b9,g9,y9,r9")};

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

    ofstream savefile("nodes.txt");

    if(!savefile.good())
        throw runtime_error("Could not open file to store pyramid nodes.");
    
    cout << "saving the pyramids to file..." << endl;

    for(const pyramid &p: S)
        savefile << p.storageString() << endl;
    
    savefile.close();

    cout << "saved all pyramids." << endl;

    return;
}

void generateEdges()
{
    cout << "createAndStoreNodes()..." << endl;

    vector<pyramid> nodes;
    vector<list<size_t>> edges;

    findEdges(nodes, edges);

    ofstream edgesfile("edges.txt");

    if(!edgesfile.good())
        throw runtime_error("Could not open file to store finished pyramid nodes.");
    
    cout << "saving the edges to file..." << endl;

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

    cout << "everything saved and closed." << endl;

    return;
}

void loadNodes(vector<pyramid> &ps)
{
    cout << "loadNodes()..." << endl;

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
    cout << "loadEdges()..." << endl;
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
