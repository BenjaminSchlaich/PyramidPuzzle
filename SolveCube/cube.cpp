
#include "cube.hpp"

#include <unordered_map>

const color RED = 0;
const color GREEN = 1;
const color BLUE = 2;
const color YELLOW = 3;
const color ORANGE = 4;
const color WHITE = 5;

const unsigned int ALLRED = 0;
const unsigned int ALLGREEN = 0x15555;
const unsigned int ALLBLUE = 0x2aaaa;
const unsigned int ALLYELLOW = 0x3ffff;

const std::list<Operation> allOperations =
                { OP_NOOP
                , OP_TURN_TOP1, OP_TURN_TOP2, OP_TURN_BOTTOM1, OP_TURN_BOTTOM2
                , OP_TURN_RIGHT1, OP_TURN_RIGHT2, OP_TURN_LEFT1, OP_TURN_LEFT2
                , OP_TURN_FRONT1, OP_TURN_FRONT2, OP_TURN_BACK1, OP_TURN_BACK2
                , OP_ROT_UP, OP_ROT_DOWN, OP_ROT_LEFT, OP_ROT_RIGHT};

// because we want to avoid the reversed operations when solving one after another
Operation reverseOp(const Operation &op);

// use these moves in the process of solving
static const std::list<Operation> solvingMoves = 
                { OP_NOOP
                , OP_TURN_TOP1, OP_TURN_TOP2, OP_TURN_BOTTOM1, OP_TURN_BOTTOM2
                , OP_TURN_RIGHT1, OP_TURN_RIGHT2, OP_TURN_LEFT1, OP_TURN_LEFT2
                , OP_TURN_FRONT1, OP_TURN_FRONT2, OP_TURN_BACK1, OP_TURN_BACK2};

const size_t surface::hashWeights[] = {3, 5, 7, 5, 3, 7, 5, 7, 3};
static const size_t colorWeights[] = {11, 13, 17, 19};

void printColor(std::ostream &os, const color &c)
{
    switch(c)
    {
        case RED:       os << 'r'; break;
        case GREEN:     os << 'g'; break;
        case BLUE:      os << 'b'; break;
        case YELLOW:    os << 'y'; break;
        case ORANGE:    os << 'o'; break;
        case WHITE:     os << 'w'; break;
    }
}

std::ostream &operator<<(std::ostream &os, const cube &p)
{
    os << std::endl << " LEFT" << std::endl;

    unsigned int vl = p.getLeft().getColors();

    unsigned int mask = 0b11;
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        int c = vl;
        for(int j=i; j<8; j++)
            c >>= 2;
        c &= mask;

        printColor(os, c);
    }

    os << std::endl << std::endl << " FRONT" << std::endl;
    
    unsigned int vf = p.getFront().getColors();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        int c = vf;
        for(int j=i; j<8; j++)
            c >>= 2;
        c &= mask;

        printColor(os, c);
    }

    os << std::endl << std::endl << " RIGHT" << std::endl;
    
    unsigned int vr = p.getRight().getColors();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        int c = vr;
        for(int j=i; j<8; j++)
            c >>= 2;
        c &= mask;

        printColor(os, c);
    }

    os << std::endl << std::endl << " BOTTOM" << std::endl;
    
    unsigned int vb = p.getBottom().getColors();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        int c = vb;
        for(int j=i; j<8; j++)
            c >>= 2;
        c &= mask;

        printColor(os, c);
    }

    return os;
}

bool operator==(const surface &s1, const surface &s2)
{
    return s1.equal(s2);
}

surface::surface(const surface &s)
{
    elements = s.elements;
}

surface::surface(color c)
{
    switch(c)
    {
        case RED:
            elements = ALLRED;
            break;
        case GREEN:
            elements = ALLGREEN;
            break;
        case BLUE:
            elements = ALLBLUE;
            break;
        case YELLOW:
            elements = ALLYELLOW;
            break;
    }
}

surface::surface(std::string &s)
{
    elements = 0;

    int i = 0;
    int is = 0;

    while(is < s.length())
    {
        if(i == 9)
            throw std::runtime_error("surface::surface(string): more than 9 tiles in the input string: " + s);

        char c = s.at(is++);

        if(isnumber(c))
        {
            if(i == 0)
                throw std::runtime_error("surface::surface(string): ill formatted string: begins with a number: " + s);

            c -= '0';

            while(--c > 0)  // first decrement, because one color has been assigned for is-1 already
            {
                if(i == 9)
                    throw std::runtime_error("surface::surface(string): more than 9 tiles in the input string: " + s);

                // elements.at(i) = elements.at(i-1);
                elements |= (0b11 & (elements >> 2));

                i++;

                if(i < 9)
                    elements <<= 2;

            }
        }
        else
        {
            switch(c)
            {
                case 'r':
                    elements |= RED;
                    break;
                case 'g':
                    elements |= GREEN;
                    break;
                case 'b':
                    elements |= BLUE;
                    break;
                case 'y':
                    elements |= YELLOW;
                    break;
                default:
                {
                    std::string errmsg = "surface::surface(string): illegal color character: ";
                    errmsg += c;
                    throw std::runtime_error(errmsg);
                }
            }

            i++;

            if(i < 9)
                elements <<= 2;
            
        }
    }

    if(i < 9)
        throw std::runtime_error("surface::surface(string): input string encodes less than 9 tiles: " + s);
}

bool surface::equal(const surface &s) const
{
    return elements == s.elements;
}

size_t surface::computeHash() const noexcept
{
    size_t hash = 1;
    size_t s = elements;

    for(int i=0; i<9; i++)
    {
        color c = s & 0b11;
        s >>= 2;

        size_t t = colorWeights[c] * hashWeights[i];

        hash *= t;
    }

    return hash;
}

bool surface::isSolved() const
{
    return  elements == 0
            || elements == ALLGREEN
            || elements == ALLBLUE
            || elements == ALLYELLOW;
}

void surface::rotateClockwise()
{
    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    // used the following mapping settings: MSB first, target bits: 1 0 11 10 3 2 5 4 17 16 13 12 15 14 7 6 9 8
    unsigned int x = elements;

    x = UNDEFINED;

    elements = x;
}

void surface::rotateCounterClockwise()
{
    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    // used the following mapping settings: MSB first, source bits: 1 0 11 10 3 2 5 4 17 16 13 12 15 14 7 6 9 8
    // (this is the same mapping as for clockwise, but with source instead of target bits!)
    unsigned int x = elements;

    x = UNDEFINED;

    elements = x;
}

unsigned int surface::getColors() const
{
    return elements;
}

cube::cube(const cube &p)   : front(p.front), back(p.left)
                            , left(p.right), right(p.bottom)
                            , top(p.top), bottom(p.bottom)
{
    
}

cube::cube(color frontColor, color leftColor, color rightColor, color bottColor) :
front(surface(frontColor)), back(surface(leftColor)),
left(surface(rightColor)), right(surface(bottColor)),
top(surface(rightColor)), bottom(surface(bottColor))
{
    
}

cube::cube(std::string sin) : front(0), back(0), left(0), right(0), top(0), bottom(0)
{
    std::string s(sin);
    char delimiter = ',';

    size_t pos = 0;
    std::string tokens[4];
    int t = 0;

    while(s.size() > 0)
    {
        if(t == 6)
            throw std::runtime_error("cube::cube(std::string): illegal input string has more than 6 surfaces: " + sin);

        pos = s.find(delimiter);
        tokens[t++] = s.substr(0, pos);

        if(pos == std::string::npos)
            break;
        else
            s.erase(0, pos + 1);
    }

    if(t != 6)
        throw std::runtime_error("cube::cube(std::string): illegal input string has less than 6 surfaces: " + sin);

    front = surface(tokens[0]);
    back = surface(tokens[1]);
    left = surface(tokens[2]);
    right = surface(tokens[3]);
    top = surface(tokens[3]);
    bottom = surface(tokens[3]);
}

bool cube::equal(const cube &p) const
{
    return p.front == front && p.back == back
        && p.left == left && p.right == right
        && p.top == top && p.bottom == bottom;
}

bool cube::operator==(const cube &p) const
{
    return equivalent(p);
}

bool cube::isSolved() const
{
    return front.isSolved() && right.isSolved() && left.isSolved() && bottom.isSolved();
}

surface cube::getFront() const
{
    return front;
}

surface cube::getRight() const
{
    return right;
}

surface cube::getLeft() const
{
    return left;
}

surface cube::getBottom() const
{
    return bottom;
}

std::string cube::storageString() const
{
    std::string s = "";

    constexpr auto convColor = [](const color c){
        switch (c)
        {
            case RED: return 'r';
            case GREEN: return 'g';
            case BLUE: return 'b';
            case YELLOW: return 'y';
            case ORANGE: return 'o';
            case WHITE: return 'w';
            default:
                throw std::runtime_error("invalid color code: " + std::to_string(c));
        }
    };

    auto sides = {front, right, back, top, left, bottom};
    int i=0;

    for(auto &side: sides)
    {
        for(int i=8; i>=0; i--)
        {
            color c = (side.getColors() >> 3*i) & 0b111;

            s.push_back(convColor(c));
        }

        if(++i < 6)
            s.push_back(',');
    }

    return s;
}

bool solve(cube &start, std::list<Operation> &moves)
{
    if(start.isSolved())
        return true;

    std::list<cube> data = {start};

    std::unordered_map<const cube*, std::pair<cube*, Operation>> pred = {{&data.front(), {nullptr, OP_NOOP}}};
    
    std::list<cube*> q = {&data.front()};

    cube *end = nullptr;

    while(!q.empty() && end == nullptr)
    {
        cube *p = q.front();
        q.pop_front();

        Operation lastOpReversed = reverseOp(pred.at(p).second);
        
        // generate all neighbors
        for(auto &op: solvingMoves)
        {
            if(op == lastOpReversed)            // this would be very counter-productive.
                continue;

            data.push_back(cube(*p));
            cube *pp = &data.back();

            executeOperation(data.back(), op);

            if(!pred.contains(pp))
            {
                pred.insert({pp, {p, op}});
                q.push_back(pp);
            }

            if(data.back().isSolved())
            {
                end = pp;
                break;
            }
        }
    }

    if(end != nullptr)
    {
        cube *p = end;

        while(true)
        {
            auto &pnext = pred.at(p);
            p = pnext.first;

            if(p != nullptr)
                moves.push_front(pnext.second);
            else
                break;
        }

        return true;
    }
    else
        return false;
}

/**
  OP_NOOP
, OP_TURN_TOP1, OP_TURN_TOP2, OP_TURN_BOTTOM1, OP_TURN_BOTTOM2
, OP_TURN_RIGHT1, OP_TURN_RIGHT2, OP_TURN_LEFT1, OP_TURN_LEFT2
, OP_TURN_FRONT1, OP_TURN_FRONT2, OP_TURN_BACK1, OP_TURN_BACK2
 */
void executeOperation(cube &p, Operation op)
{
    switch(op)
    {
        case OP_NOOP:
            break;
        case OP_TURN_TOP1:
            p.turnTop1();
            break;
        case OP_TURN_TOP2:
            p.turnTop2();
            break;
        case OP_TURN_BOTTOM1:
            p.turnBottom1();
            break;
        case OP_TURN_BOTTOM2:
            p.turnBottom2();
            break;
        case OP_TURN_RIGHT1:
            p.turnRight1();
            break;
        case OP_TURN_RIGHT2:
            p.turnRight2();
            break;
        case OP_TURN_LEFT1:
            p.turnLeft1();
            break;
        case OP_TURN_LEFT2:
            p.turnLeft2();
            break;
        case OP_TURN_FRONT1:
            p.turnFront1();
            break;
        case OP_TURN_FRONT2:
            p.turnFront2();
            break;
        case OP_TURN_BACK1:
            p.turnBack1();
            break;
        case OP_TURN_BACK2:
            p.turnBack2();
            break;
        case OP_ROT_LEFT:
            p.rotLeft();
            break;
        case OP_ROT_RIGHT:
            p.rotRight();
            break;
        case OP_ROT_UP:
            p.rotUp();
            break;
        case OP_ROT_DOWN:
            p.rotDown();
            break;
        default:
            throw std::runtime_error("executeOperation(): unknown operation: " + operationToString(op));
    }
    
    #if DEBUG
    std::cout << "Executed operation: " << op << "()." << std::endl;
    #endif
}

std::string operationToString(const Operation &op)
{
    switch(op)
    {
        case OP_TURN_TOP1:
            return "Turn the upper part to the left.";
        case OP_TURN_TOP2:
            return "Turn the upper part to the right.";
        case OP_TURN_BOTTOM1:
            return "Turn the bottom part to the left.";
        case OP_TURN_BOTTOM2:
            return "Turn the bottom part to the right.";
        case OP_TURN_RIGHT1:
            return "Turn the right part upwards.";
        case OP_TURN_RIGHT2:
            return "Turn the right part downwards.";
        case OP_TURN_LEFT1:
            return "Turn the left part upwards.";
        case OP_TURN_LEFT2:
            return "Turn the left part downwards.";
        case OP_TURN_FRONT1:
            return "Turn the front part clockwise.";
        case OP_TURN_FRONT2:
            return "Turn the front part counter-clockwise.";
        case OP_TURN_BACK1:
            return "Turn the back part clockwise from the front perspective.";
        case OP_TURN_BACK2:
            return "Turn the back part counter-clockwise from the front perspective.";
        case OP_ROT_LEFT:
            return "Rotate the whole cube to the left.";
        case OP_ROT_RIGHT:
            return "Rotate the whole cube to the right.";
        case OP_ROT_UP:
            return "Rotate the whole cube upwards.";
        case OP_ROT_DOWN:
            return "Rotate the whole cube downwards.";
        case OP_NOOP:
            return "Don't do anything.";
        default:
            throw std::runtime_error("operationToString(): unknown operation " + std::to_string(op));
    }
}

Operation reverseOp(const Operation &op)
{
    switch(op)
    {
        case OP_NOOP:
            return OP_NOOP;
        case OP_TURN_TOP1:
            return OP_TURN_TOP2;
        case OP_TURN_TOP2:
            return OP_TURN_TOP1;
        case OP_TURN_BOTTOM1:
            return OP_TURN_BOTTOM2;
        case OP_TURN_BOTTOM2:
            return OP_TURN_BOTTOM1;
        case OP_TURN_RIGHT1:
            return OP_TURN_RIGHT2;
        case OP_TURN_RIGHT2:
            return OP_TURN_RIGHT1;
        case OP_TURN_LEFT1:
            return OP_TURN_LEFT2;
        case OP_TURN_LEFT2:
            return OP_TURN_LEFT1;
        case OP_TURN_FRONT1:
            return OP_TURN_FRONT2;
        case OP_TURN_FRONT2:
            return OP_TURN_FRONT1;
        case OP_TURN_BACK1:
            return OP_TURN_BACK2;
        case OP_TURN_BACK2:
            return OP_TURN_BACK1;
        case OP_ROT_LEFT:
            return OP_ROT_RIGHT;
        case OP_ROT_RIGHT:
            return OP_ROT_LEFT;
        case OP_ROT_UP:
            return OP_ROT_DOWN;
        case OP_ROT_DOWN:
            return OP_ROT_UP;
        default:
            throw std::runtime_error("reverseOp(): unknown operation " + std::to_string(op));
    }
}
