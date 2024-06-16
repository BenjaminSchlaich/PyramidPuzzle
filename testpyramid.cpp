
#include "basic.hpp"
#include "testpyramid.hpp"


static const std::list<std::pair<std::list<std::string>,std::list<std::string>>> testCases = {
    {
        {"b9,g9,y9,r9","g9,y9,b9,r9","r9,y9,g9,b9","g4r5,r4y5,y4g5,b9","g3brrb3,r6gyy,y4g5,brbby3bb","g3yggy3,yrygr5,brbby3bb,b4g3rr","yryg3y3,brbbr5,g3y4bb,b4g3rr"},
        {"turnLeft","rotateRightCornerUp","rotateUpperRight","rotateRightUp","rotateRightCornerDown","rotateUpperLeft"}
    }
};

void runAllTests()
{
    int t = 0;
    int success = 0;
    int skipped = 0;
    int failed = 0;

    for(auto &testpair: testCases)
    {
        auto &configs = testpair.first;
        auto &ops = testpair.second;

        std::list<pyramid> configurations;

        for(auto &s: configs)
            configurations.push_back(pyramid(s));

        int status = runTestCase(configurations, ops);

        if(status > 0)
            success++;
        else if(status == 0)
            skipped++;
        else
            failed++;
        
        t++;
    }

    std::cout << std::endl << std::endl << "    TEST STATUS" << std::endl;
    std::cout << "Total test: " << t << ". Successful tests: " << success << ". Failures: " << failed << ". Skipped: " << skipped << "." << std::endl;
}

int runTestCase(const std::list<pyramid> &configs, const std::list<std::string> &ops)
{
    if(ops.size() + 1 != configs.size())
        return 0;

    int step = 1;
    auto itC = configs.begin();
    auto itO = ops.begin();
    pyramid p(*itC++);

    while(itO != ops.end())
    {
        executeOperation(p, *itO++);

        if(!p.equal(*itC))
        {
            std::cout << "Test failure in step " << step << ":" << std::endl;
            std::cout << "Expected pyramid: " << std::endl << *itC << std::endl;
            std::cout << "Got pyramid:" << std::endl << p << std::endl;
            return -1;
        }

        itC++;
        step++;
    }

    return 1;
}
