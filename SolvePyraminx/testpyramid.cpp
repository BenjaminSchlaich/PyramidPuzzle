
#include "basic.hpp"
#include "testpyramid.hpp"


static const std::list<std::pair<std::list<std::string>,std::list<Operation>>> testCases = {
    {
        {"b9,g9,y9,r9","g9,y9,b9,r9","r9,y9,g9,b9","g4r5,r4y5,y4g5,b9","g3brrb3,r6gyy,y4g5,brbby3bb","g3yggy3,yrygr5,brbby3bb,b4g3rr","yryg3y3,brbbr5,g3y4bb,b4g3rr"},
        {OP_TURN_LEFT,OP_RIGHT_CORNER_UP,OP_UPPER_RIGHT,OP_RIGHT_UP,OP_RIGHT_CORNER_DOWN,OP_UPPER_LEFT}
    },
    /**
     *  Execute each operation once:
     *  turnLeft
     *  turnRight
     *  rotateRightCornerUp
     *  rotateRightCornerDown
     *  rotateUpperRight
     *  rotateUpperLeft
     *  rotateRightUp
     *  rotateRightDown
     *  rotateRightestUp
     *  rotateRightestDown
     *  rotateTopRight
     *  rotateTopLeft
     * 
     *  added:
     *  rotateLeftCornerUp
     *  rotateLeftCornerDown
     * */
    {
        {"b9,g9,y9,r9","gb8,yg8,by8,r9","gb7g,yg3rg4,by8,r4br4","gbbgbbggr,yrggbrrgg,by8,rbrrgbbrr"
        ,"yrggbbggr,by3brrgg,gbbgy5,rbrrgbbrr","y3byygbg,gygrbyyrb,rbrrgbbrr,rg3ygrbb","rbrrgbbrr,y3byygbg,gygrbyyrb,yrggbbggr"
        ,"bgbrrg3y,ygyygbbyy,r3brrbbg,gygrbyyrb","r3brg3y,bgbrgbbyy,ygyyrrbbg,gygrbyyrb","r3yrgyyb,bgbrygbyy,ygyyrrbbg,g3rgbbrb"
        ,"r3yrgyyg,bgbrbgbyy,ygyyrrbbg,g3rybbrb","yrryrgyyg,rgbrbgbyy,bgyyrrbbg,g3rybbrb","rgbrbgbyy,bgyyrrbbg,yrryrgyyg,brrbg3by"
        ,"ybbgbrrgg,bbggyyrbr,gy4rrgr,bgyyrrbbg","rgbrbgbyy,bgyyrrbbg,yrryrgyyg,brrbg3by"},
        {OP_TOP_LEFT,OP_RIGHTEST_DOWN,OP_RIGHT_DOWN
        ,OP_UPPER_LEFT,OP_RIGHT_CORNER_DOWN,OP_TURN_RIGHT
        ,OP_RIGHT_CORNER_UP,OP_UPPER_RIGHT,OP_RIGHT_UP
        ,OP_RIGHTEST_UP,OP_TOP_RIGHT,OP_TURN_LEFT
        ,OP_LEFT_CORNER_UP,OP_LEFT_CORNER_DOWN}
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
        {
            failed++;
            std::cout << "Test case " << (t+1) << " failed." << std::endl;
        }
        
        t++;
    }

    std::cout << std::endl << std::endl << "    TEST STATUS" << std::endl;
    std::cout << "Total test: " << t << ". Successful tests: " << success << ". Failures: " << failed << ". Skipped: " << skipped << "." << std::endl;
}

int runTestCase(const std::list<pyramid> &configs, const std::list<Operation> &ops)
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
