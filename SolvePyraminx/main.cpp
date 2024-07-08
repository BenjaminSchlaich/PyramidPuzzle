
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

// find the rotations so that p0 becomes p1
void findRotation(const pyramid &p0, const pyramid &p1, list<Operation> &solution);

// find the Operation that turns p0 into p1
Operation findOperation(const pyramid &p0, const pyramid &p1);

// solve the problem using the precomputed graph.
void bfsSolve(const vector<pyramid> &ps, const vector<list<size_t>> g, list<Operation> &solution, const pyramid &inst);

int main()
{
    ///*
    solverLoop();
    /*/
    vector<pyramid> ps;

    loadNodes(ps);

    vector<list<size_t>> g;

    loadEdges(g);

    list<Operation> solution;

    pyramid problem("b9,g9,y9,r9");

    // problem.turnLeft();
    problem.rotateRightUp();
    problem.rotateLeftDown();
    problem.rotateTopLeft();

    bfsSolve(ps, g, solution, problem);

    for(Operation op: solution)
        cout << operationToString(op) << endl;//*/
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

    cout << ps.size() << " nodes were loaded successfuly." << endl;
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
        else
            g.push_back({});

        const char delimiter = ',';

        size_t pos = 0;
        vector<string> tokens;

        while(s.size() > 0)
        {
            pos = s.find(delimiter);
            tokens.push_back(s.substr(0, pos));

            if(pos == std::string::npos)
                break;
            else
                s.erase(0, pos + 1);
        }

        for(string &t: tokens)
            g.back().push_back(stoul(t));
    }

    ifs.close();

    cout << "loaded a graph with " << g.size() << " nodes." << endl;
}

void bfsSolve(const vector<pyramid> &ps, const vector<list<size_t>> g, list<Operation> &solution, const pyramid &inst)
{
    solution.clear();

    // trivial check:
    if(inst.isSolved())
    {
        solution.push_back(OP_NOOP);    // because an empty solution means that no solution was found, which isn't the case.
        return;
    }

    // first, let's find the right starting point, and also reset the marked flags
    size_t startID = -1;

    for(size_t id=0; id<ps.size(); id++)
    {
        if(ps.at(id) == inst)
            startID = id;
        
        ps.at(id).marked = false;
    }
    
    if(startID == -1)
    {
        cout << "The right entry point was not found!" << endl;
        return;
    }

    ps.at(startID).marked = true;

    // so we have everything in order later for backtracking
    list<Operation> rotationSolution;
    
    // turn the pyramid, so that it becomes exactly the one in the graph.
    if(!inst.equal(ps.at(startID)))
        findRotation(inst, ps.at(startID), rotationSolution);

    // bfs queue
    list<size_t> q;
    q.push_back(startID);

    // bfs predecessor array
    vector<size_t> pred(ps.size());
    pred.at(startID) = startID;

    // solution target:
    const pyramid target("b9,g9,y9,r9");
    bool solved = false;

    while(!(solved || q.empty()))
    {
        size_t uID = q.front();
        q.pop_front();

        for(size_t vID: g.at(uID))      // iterate over all neighbors
        {
            const pyramid &v = ps.at(vID);

            if(v.marked)                // was visited before, skip.
                continue;
            
            v.marked = true;            // mark the previously unknown node
            pred.at(vID) = uID;         // set u to be the predecessor of v
            q.push_back(vID);           // and add v to the queue
            
            if(v == target)             // we've found the solution!
            {
                solved = true;
                break;
            }

            q.push_back(vID);           // add unknown node to the queue
        }
    }

    if(q.empty())
    {
        cout << "No solution was found!" << endl;
        return;
    }
    
    // do all of the backtracking
    size_t pID = q.back();

    while(pID != startID)
    {
        const pyramid &p1 = ps.at(pID);
        pID = pred.at(pID);
        const pyramid &p0 = ps.at(pID);
        solution.push_back(findOperation(p0, p1));
    }

    for(Operation rot: rotationSolution)
        solution.push_front(rot);
}

void findRotation(const pyramid &p0, const pyramid &p1, list<Operation> &solution)
{
    cout << "Rotate " << p0 << " so that it becomes " << p1 << endl;
}

Operation findOperation(const pyramid &p0, const pyramid &p1)
{
    for(Operation op: allOperations)
    {
        pyramid p(p0);

        executeOperation(p, op);

        if(p.equal(p1))
            return op;
    }

    throw runtime_error("findOperation(): no operation turns '" + p0.storageString() + "' into '" + p1.storageString() + "'.");
}
