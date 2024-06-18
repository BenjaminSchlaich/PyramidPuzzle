

#include "pyramid.hpp"

#include <unordered_map>

const color RED = 0;
const color GREEN = 1;
const color BLUE = 2;
const color YELLOW = 3;

const unsigned int ALLRED = 0;
const unsigned int ALLGREEN = 0x15555;
const unsigned int ALLBLUE = 0x2aaaa;
const unsigned int ALLYELLOW = 0x3ffff;

// because we want to avoid the reversed operations when solving one after another
Operation reverseOp(const Operation &op);

// use these moves in the process of solving
static const std::list<Operation> solvingMoves = {
    //OP_TURN_LEFT,
    //OP_TURN_RIGHT,
    OP_RIGHT_CORNER_UP,
    //OP_RIGHT_CORNER_DOWN,
    OP_UPPER_RIGHT/*,
    OP_UPPER_LEFT,
    OP_RIGHT_UP,
    OP_RIGHT_DOWN//*/
};

void printColor(std::ostream &os, const color &c)
{
    switch(c)
    {
        case RED:   os << 'r'; break;
        case GREEN:   os << 'g'; break;
        case BLUE:   os << 'b'; break;
        case YELLOW:   os << 'y'; break;
    }
}

std::ostream &operator<<(std::ostream &os, const pyramid &p)
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

bool surface::isSolved() const
{
    return  elements == 0
            || elements == ALLGREEN
            || elements == ALLBLUE
            || elements == ALLYELLOW;
}

bool surface::isSolvedButCorners() const
{
    unsigned int mask = 0b001111110011111100;
    unsigned int mels = elements & mask;

    return  mels == 0
            || mels == (ALLGREEN & mask)
            || mels == (ALLBLUE & mask)
            || mels == (ALLYELLOW & mask);
}

void surface::rotateClockwise()
{
    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    // used the following mapping settings: MSB first, target bits: 1 0 11 10 3 2 5 4 17 16 13 12 15 14 7 6 9 8
    unsigned int x = elements;

    x = ((x & 0x0000000c) << 4)
    | ((x & 0x000000c0) << 6)
    | ((x & 0x00000303) << 8)
    | ((x & 0x00000030) << 10)
    | ((x & 0x00030000) >> 16)
    | ((x & 0x00003000) >> 10)
    | ((x & 0x00000c00) >> 6)
    | ((x & 0x0000c000) >> 4);

    elements = x;
}

void surface::rotateCounterClockwise()
{
    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    // used the following mapping settings: MSB first, source bits: 1 0 11 10 3 2 5 4 17 16 13 12 15 14 7 6 9 8
    // (this is the same mapping as for clockwise, but with source instead of target bits!)
    unsigned int x = elements;

    x = ((x & 0x00000c00) << 4)
    | ((x & 0x00000030) << 6)
    | ((x & 0x0000000c) << 10)
    | ((x & 0x00000003) << 16)
    | ((x & 0x0000c000) >> 10)
    | ((x & 0x00030300) >> 8)
    | ((x & 0x00003000) >> 6)
    | ((x & 0x000000c0) >> 4);

    elements = x;
}

void surface::rotateUpper(bool clockwise)
{
    unsigned int x = elements;

    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    if(clockwise)
    {
        // MSB first, target bits: 11 10 17 16 13 12 15 14 9 8 7 6 5 4 3 2 1 0
        x = (x & 0x000033ff)
        | ((x & 0x0000c000) << 2)
        | ((x & 0x00000c00) << 4)
        | ((x & 0x00030000) >> 6);
    }
    else
    {
        // MSB first, source bits: 11 10 17 16 13 12 15 14 9 8 7 6 5 4 3 2 1 0
        x = (x & 0x000033ff)
        | ((x & 0x00000c00) << 6)
        | ((x & 0x0000c000) >> 4)
        | ((x & 0x00030000) >> 2);
    }

    elements = x;
}

void surface::rotateLeft(bool clockwise)
{
    unsigned int x = elements;

    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    if(clockwise)
    {
        // MSB first, target bits: 17 16 5 4 13 12 11 10 15 14 7 6 9 8 3 2 1 0
        x = (x & 0x00033ccf)
        | ((x & 0x00000030) << 4)
        | ((x & 0x00000300) << 6)
        | ((x & 0x0000c000) >> 10);
    }
    else
    {
        // MSB first, source bits: 17 16 5 4 13 12 11 10 15 14 7 6 9 8 3 2 1 0
        x = (x & 0x00033ccf)
        | ((x & 0x00000030) << 10)
        | ((x & 0x0000c000) >> 6)
        | ((x & 0x00000300) >> 4);
    }

    elements = x;
}

void surface::rotateRight(bool clockwise)
{
    unsigned int x = elements;

    // code generated here for bit permutation: https://programming.sirrida.de/calcperm.php
    if(clockwise)
    {
        // MSB first, target bits: 17 16 15 14 13 12 1 0 9 8 7 6 11 10 3 2 5 4
        x = (x & 0x0003f3cc)
        | ((x & 0x00000003) << 4)
        | ((x & 0x00000030) << 6)
        | ((x & 0x00000c00) >> 10);
    }
    else
    {
        // MSB first, source bits: 17 16 15 14 13 12 1 0 9 8 7 6 11 10 3 2 5 4
        x = (x & 0x0003f3cc)
        | ((x & 0x00000003) << 10)
        | ((x & 0x00000c00) >> 6)
        | ((x & 0x00000030) >> 4);
    }

    elements = x;
}

void surface::setByMask(unsigned int mask, unsigned int selection)
{
    this->elements &= ~mask;                    // unset all masked elements
    this->elements |= (mask & selection);        // reset the masked and selected elements
}

void surface::setUpper(unsigned int elements)
{
    setByMask(0b111111110000000000, elements);
}

void surface::setLeft(unsigned int elements)
{
    setByMask(0b001100001111110000, elements);
}

void surface::setRight(unsigned int elements)
{
    setByMask(0b000000110000111111, elements);
}

unsigned int surface::getColors() const
{
    return elements;
}

color surface::getTop() const
{
    return elements >> 16;
}

color surface::getRightest() const
{
    return elements & 0b11;
}

color surface::getLeftest() const
{
    return (elements >> 8) & 0b11;
}

void surface::setTop(color c)
{
    setByMask(0b110000000000000000, c << 16);
}

void surface::setLeftest(color c)
{
    setByMask(0b000000001100000000, c << 8);
}

void surface::setRightest(color c)
{
    setByMask(0b000000000000000011, c);
}

pyramid::pyramid(const pyramid &p) : front(p.front), left(p.left), right(p.right), bottom(p.bottom)
{
    
}

pyramid::pyramid(color frontColor, color leftColor, color rightColor, color bottColor) :
front(surface(frontColor)), left(surface(leftColor)), right(surface(rightColor)), bottom(surface(bottColor))
{
    
}

pyramid::pyramid(std::string sin) : front(0), right(0), left(0), bottom(0)
{
    std::string s(sin);
    char delimiter = ',';

    size_t pos = 0;
    std::string tokens[4];
    int t = 0;

    while(s.size() > 0)
    {
        if(t == 4)
            throw std::runtime_error("pyramid::pyramid(std::string): illegal input string has more than 4 surfaces: " + sin);

        pos = s.find(delimiter);
        tokens[t++] = s.substr(0, pos);

        if(pos == std::string::npos)
            break;
        else
            s.erase(0, pos + 1);
    }

    if(t != 4)
        throw std::runtime_error("pyramid::pyramid(std::string): illegal input string has less than 4 surfaces: " + sin);

    front = surface(tokens[0]);
    right = surface(tokens[1]);
    left = surface(tokens[2]);
    bottom = surface(tokens[3]);
}

bool pyramid::equal(const pyramid &p) const
{
    const unsigned int pf = p.front.getColors();
    const unsigned int pr = p.right.getColors();
    const unsigned int pl = p.left.getColors();
    const unsigned int pb = p.bottom.getColors();

    const unsigned int qf = front.getColors();
    const unsigned int qr = right.getColors();
    const unsigned int ql = left.getColors();
    const unsigned int qb = bottom.getColors();

    int p1 = std::popcount(pf);
    int p2 = std::popcount(pr);
    int p3 = std::popcount(pl);
    int p4 = std::popcount(pb);

    int q1 = std::popcount(qf);
    int q2 = std::popcount(qr);
    int q3 = std::popcount(ql);
    int q4 = std::popcount(qb);

    int s1 = p1+p2+p3+p4;
    int s2 = q1+q2+q3+q4;

    if(s1 != s2)
        return false;
    
    int p1 = p1*p2*p3*p4;
    int p2 = q1*q2*q3*q4;

    if(p1 != p2)
        return false;
    
    std::list<unsigned int> l1 = {pf,pr,pl,pb};
    l1.sort();
    std::list<unsigned int> l2 = {qf,qr,ql,qb};
    l2.sort();

    return l1 == l2;
}

bool pyramid::isSolved() const
{
    return front.isSolved() && right.isSolved() && left.isSolved() && bottom.isSolved();
}

bool pyramid::isSolvedButCorners() const
{
    return front.isSolvedButCorners() && right.isSolvedButCorners()
            && left.isSolvedButCorners() && bottom.isSolvedButCorners();
}

surface pyramid::getFront() const
{
    return front;
}

surface pyramid::getRight() const
{
    return right;
}

surface pyramid::getLeft() const
{
    return left;
}

surface pyramid::getBottom() const
{
    return bottom;
}

std::string pyramid::storageString() const
{
    std::string s = "";

    constexpr auto convColor = [](const color c){
        switch (c)
        {
            case RED: return 'r';
            case GREEN: return 'g';
            case BLUE: return 'b';
            case YELLOW: return 'y';
            default:
                throw std::runtime_error("invalid color code: " + std::to_string(c));
        }
    };

    auto sides = {front, right, left, bottom};
    int i=0;

    for(auto &side: sides)
    {
        for(int i=8; i>=0; i--)
        {
            color c = (side.getColors() >> 2*i) & 0b11;

            s.push_back(convColor(c));
        }

        if(i++ < 3)
            s.push_back(',');
    }

    return s;
}

void pyramid::turnLeft()
{
    surface s = front;
    front = right;
    right = left;
    left = s;

    bottom.rotateCounterClockwise();
}

void pyramid::turnRight()
{
    surface s = front;
    front = left;
    left = right;
    right = s;

    bottom.rotateClockwise();
}

void pyramid::rotateRightCornerUp()
{
    front.rotateCounterClockwise();
    right.rotateClockwise();
    bottom.rotateClockwise();

    surface s = front;
    front = bottom;
    bottom = left;
    left = s;
}

void pyramid::rotateRightCornerDown()
{
    front.rotateCounterClockwise();
    right.rotateCounterClockwise();
    left.rotateClockwise();

    surface s = front;
    front = left;
    left = bottom;
    bottom = s;
}

void pyramid::rotateUpperRight()
{
    surface s(front);
    front.setUpper(left.getColors());
    left.setUpper(right.getColors());
    right.setUpper(s.getColors());
}

void pyramid::rotateUpperLeft()
{
    surface s(front);
    front.setUpper(right.getColors());
    right.setUpper(left.getColors());
    left.setUpper(s.getColors());
}

void pyramid::rotateRightUp()
{
    surface s(front);

    bottom.rotateCounterClockwise();
    front.setRight(bottom.getColors());
    bottom.rotateClockwise();
    
    bottom.setLeft(right.getColors());

    s.rotateClockwise();
    right.setLeft(s.getColors());
}

void pyramid::rotateRightDown()
{
    surface s(front);

    right.rotateCounterClockwise();
    front.setRight(right.getColors());
    right.rotateClockwise();

    right.setLeft(bottom.getColors());

    s.rotateClockwise();
    bottom.setLeft(s.getColors());
}

void pyramid::rotateRightestUp()
{
    color c = front.getRightest();
    front.setRightest(bottom.getLeftest());
    bottom.setLeftest(right.getLeftest());
    right.setLeftest(c);
}

void pyramid::rotateRightestDown()
{
    color c = front.getRightest();
    front.setRightest(right.getLeftest());
    right.setLeftest(bottom.getLeftest());
    bottom.setLeftest(c);
}

void pyramid::rotateTopRight()
{
    surface s(front);
    front.setTop(left.getTop());
    left.setTop(right.getTop());
    right.setTop(s.getTop());
}

void pyramid::rotateTopLeft()
{
    surface s(front);
    front.setTop(right.getTop());
    right.setTop(left.getTop());
    left.setTop(s.getTop());
}

bool solve(pyramid &start, std::list<Operation> &moves)
{
    if(start.isSolved())
        return true;

    std::list<pyramid> data = {start};

    std::unordered_map<const pyramid*, std::pair<pyramid*, Operation>> pred = {{&data.front(), {nullptr, OP_NOOP}}};
    
    std::list<pyramid*> q = {&data.front()};

    pyramid *end = nullptr;

    while(!q.empty() && end == nullptr)
    {
        pyramid *p = q.front();
        q.pop_front();

        Operation lastOpReversed = reverseOp(pred.at(p).second);
        
        // generate all neighbors
        for(auto &op: solvingMoves)
        {
            if(op == lastOpReversed)            // this would be very counter-productive.
                continue;

            data.push_back(pyramid(*p));
            pyramid *pp = &data.back();

            executeOperation(data.back(), op);

            if(!pred.contains(pp))
            {
                pred.insert({pp, {p, op}});
                q.push_back(pp);
            }

            if(data.back().isSolvedButCorners())
            {
                end = pp;
                break;
            }
        }
    }

    if(end != nullptr)
    {
        pyramid *p = end;

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

void executeOperation(pyramid &p, Operation op)
{
    switch(op)
    {
        case OP_TURN_LEFT:
            p.turnLeft();
            break;
        case OP_TURN_RIGHT:
            p.turnRight();
            break;
        case OP_RIGHT_CORNER_UP:
            p.rotateRightCornerUp();
            break;
        case OP_RIGHT_CORNER_DOWN:
            p.rotateRightCornerDown();
            break;
        case OP_UPPER_RIGHT:
            p.rotateUpperRight();
            break;
        case OP_UPPER_LEFT:
            p.rotateUpperLeft();
            break;
        case OP_RIGHT_UP:
            p.rotateRightUp();
            break;
        case OP_RIGHT_DOWN:
            p.rotateRightDown();
            break;
        case OP_RIGHTEST_UP:
            p.rotateRightestUp();
            break;
        case OP_RIGHTEST_DOWN:
            p.rotateRightestDown();
            break;
        case OP_TOP_LEFT:
            p.rotateTopLeft();
            break;
        case OP_TOP_RIGHT:
            p.rotateTopRight();
            break;
        default:
            throw std::runtime_error("executeOperation(): unknown operation " + std::to_string(op));
    }   
    
    #if DEBUG
    std::cout << "Executed operation: " << op << "()." << std::endl;
    #endif
}

std::string operationToString(const Operation &op)
{
    switch(op)
    {
        case OP_TURN_LEFT:
            return "Turn the whole pyramid left.";
            break;
        case OP_TURN_RIGHT:
            return "Turn the whole pyramid right.";
            break;
        case OP_RIGHT_CORNER_UP:
            return "Turn the right corner upwards.";
            break;
        case OP_RIGHT_CORNER_DOWN:
            return "Turn the right corner downwards.";
            break;
        case OP_UPPER_RIGHT:
            return "Rotate the upper section towards the right.";
            break;
        case OP_UPPER_LEFT:
            return "Rotate the upper section towards the left.";
            break;
        case OP_RIGHT_UP:
            return "Rotate the right section upwards.";
            break;
        case OP_RIGHT_DOWN:
            return "Rotate the right section downwards.";
            break;
        case OP_RIGHTEST_UP:
            return "Rotate the right corner upwards.";
            break;
        case OP_RIGHTEST_DOWN:
            return "Rotate the right corner downwards.";
            break;
        case OP_TOP_LEFT:
            return "Rotate the top corner left.";
            break;
        case OP_TOP_RIGHT:
            return "Rotate the top corner right.";
            break;
        case OP_NOOP:
            return "Don't do anything.";
        default:
            throw std::runtime_error("executeOperation(): unknown operation " + std::to_string(op));
    }
}

Operation reverseOp(const Operation &op)
{
    switch(op)
    {
        case OP_TURN_LEFT:
            return OP_TURN_RIGHT;
            break;
        case OP_TURN_RIGHT:
            return OP_TURN_LEFT;
            break;
        case OP_RIGHT_CORNER_UP:
            return OP_RIGHT_CORNER_DOWN;
            break;
        case OP_RIGHT_CORNER_DOWN:
            return OP_RIGHT_CORNER_UP;
            break;
        case OP_UPPER_RIGHT:
            return OP_UPPER_LEFT;
            break;
        case OP_UPPER_LEFT:
            return OP_UPPER_RIGHT;
            break;
        case OP_RIGHT_UP:
            return OP_RIGHT_DOWN;
            break;
        case OP_RIGHT_DOWN:
            return OP_RIGHT_UP;
            break;
        case OP_RIGHTEST_UP:
            return OP_RIGHTEST_DOWN;
            break;
        case OP_RIGHTEST_DOWN:
            return OP_RIGHTEST_UP;
            break;
        case OP_TOP_LEFT:
            return OP_TOP_RIGHT;
            break;
        case OP_TOP_RIGHT:
            return OP_TOP_LEFT;
            break;
        case OP_NOOP:
            return OP_NOOP;
        default:
            throw std::runtime_error("executeOperation(): unknown operation " + std::to_string(op));
    }
}
