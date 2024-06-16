

#include "pyramid.hpp"

#include <unordered_map>

static const std::list<std::string> solvingMoves = {
    "turnLeft",
    "turnRight",
    "rotateRightCornerUp",
    "rotateRightCornerDown",
    "rotateUpperRight",
    "rotateUpperLeft",
    "rotateRightUp",
    "rotateRightDown"
};

std::ostream &operator<<(std::ostream &os, const color &c)
{
    switch(c)
    {
        case RED:   os << 'r'; break;
        case GREEN:   os << 'g'; break;
        case BLUE:   os << 'b'; break;
        case YELLOW:   os << 'y'; break;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const pyramid &p)
{
    os << std::endl << " LEFT" << std::endl;

    auto &vl = p.getLeft()->getAll();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        os << vl.at(i);
    }

    os << std::endl << std::endl << " FRONT" << std::endl;
    
    auto &vf = p.getFront()->getAll();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        os << vf.at(i);
    }

    os << std::endl << std::endl << " RIGHT" << std::endl;
    
    auto &vr = p.getRight()->getAll();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        os << vr.at(i);
    }

    os << std::endl << std::endl << " BOTTOM" << std::endl;
    
    auto &vb = p.getBottom()->getAll();
    
    for(int i=0; i<9; i++)
    {
        if(i == 0)
            std::cout << "  ";
        else if(i == 1)
            std::cout << std::endl << " ";
        else if(i == 4)
            std::cout << std::endl;
        
        os << vb.at(i);
    }

    return os;
}

surface::surface(const surface &s)
{
    elements = std::vector(s.elements);

    upper = std::vector<color*>();
    upper.push_back(&elements.at(0));
    upper.push_back(&elements.at(1));
    upper.push_back(&elements.at(2));
    upper.push_back(&elements.at(3));

    left = std::vector<color*>();
    left.push_back(&elements.at(1));
    left.push_back(&elements.at(4));
    left.push_back(&elements.at(5));
    left.push_back(&elements.at(6));

    right = std::vector<color*>();
    right.push_back(&elements.at(3));
    right.push_back(&elements.at(6));
    right.push_back(&elements.at(7));
    right.push_back(&elements.at(8));
}

surface::surface(color c)
{
    elements = std::vector(9, c);

    upper = std::vector<color*>();
    upper.push_back(&elements.at(0));
    upper.push_back(&elements.at(1));
    upper.push_back(&elements.at(2));
    upper.push_back(&elements.at(3));

    left = std::vector<color*>();
    left.push_back(&elements.at(1));
    left.push_back(&elements.at(4));
    left.push_back(&elements.at(5));
    left.push_back(&elements.at(6));

    right = std::vector<color*>();
    right.push_back(&elements.at(3));
    right.push_back(&elements.at(6));
    right.push_back(&elements.at(7));
    right.push_back(&elements.at(8));
}

surface::surface(std::string &s)
{
    elements = std::vector<color>(9);

    int i = 0;
    int is = 0;

    while(is < s.length())
    {
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

                elements.at(i) = elements.at(i-1);
                i++;
            }
        }
        else
        {
            switch(c)
            {
                case 'r':
                    elements.at(i++) = RED;
                    break;
                case 'g':
                    elements.at(i++) = GREEN;
                    break;
                case 'b':
                    elements.at(i++) = BLUE;
                    break;
                case 'y':
                    elements.at(i++) = YELLOW;
                    break;
                default:
                {
                    std::string errmsg = "surface::surface(string): illegal color character: ";
                    errmsg += c;
                    throw std::runtime_error(errmsg);
                }
            }
        }
    }

    if(i < 9)
        throw std::runtime_error("surface::surface(string): input string encodes less than 9 tiles: " + s);
    
    upper = std::vector<color*>();
    upper.push_back(&elements.at(0));
    upper.push_back(&elements.at(1));
    upper.push_back(&elements.at(2));
    upper.push_back(&elements.at(3));

    left = std::vector<color*>();
    left.push_back(&elements.at(1));
    left.push_back(&elements.at(4));
    left.push_back(&elements.at(5));
    left.push_back(&elements.at(6));

    right = std::vector<color*>();
    right.push_back(&elements.at(3));
    right.push_back(&elements.at(6));
    right.push_back(&elements.at(7));
    right.push_back(&elements.at(8));
}

bool surface::equal(const surface &s) const
{
    return elements == s.elements;
}

bool surface::isSolved() const
{
    for(const color &e: elements)
        if(e != elements.front())
            return false;
    
    return true;
}

bool surface::isSolvedButCorners() const
{
    for(int i=2; i<8; i++)
    {
        if(i == 4)
            continue;
        else if(elements.at(i) != elements.at(1))
            return false;
    }

    return true;
}

void surface::rotateClockwise()
{
    // the corners
    color c = elements.at(0);
    elements.at(0) = elements.at(4);
    elements.at(4) = elements.at(8);
    elements.at(8) = c;

    // now the edegs
    c = elements.at(1);
    elements.at(1) = elements.at(6);
    elements.at(6) = elements.at(3);
    elements.at(3) = c;

    // and now the inner tiles
    c = elements.at(2);
    elements.at(2) = elements.at(5);
    elements.at(5) = elements.at(7);
    elements.at(7) = c;
}

void surface::rotateCounterClockwise()
{
    rotateClockwise();
    rotateClockwise();
}

void surface::rotateUpper(bool clockwise)
{
    color c = *upper.at(0);

    if(clockwise)
    {
        *upper.at(0) = *upper.at(1);
        *upper.at(1) = *upper.at(3);
        *upper.at(3) = c;
    }
    else
    {
        *upper.at(0) = *upper.at(3);
        *upper.at(3) = *upper.at(1);
        *upper.at(1) = c;
    }
}

void surface::rotateLeft(bool clockwise)
{
    color c = *left.at(0);

    if(clockwise)
    {
        *left.at(0) = *left.at(1);
        *left.at(1) = *left.at(3);
        *left.at(3) = c;
    }
    else
    {
        *left.at(0) = *left.at(3);
        *left.at(3) = *left.at(1);
        *left.at(1) = c;
    }
}

void surface::rotateRight(bool clockwise)
{
    color c = *right.at(0);

    if(clockwise)
    {
        *right.at(0) = *right.at(1);
        *right.at(1) = *right.at(3);
        *right.at(3) = c;
    }
    else
    {
        *right.at(0) = *right.at(3);
        *right.at(3) = *right.at(1);
        *right.at(1) = c;
    }
}

std::vector<color*> &surface::getUpper()
{
    return upper;
}

std::vector<color*> &surface::getLeft()
{
    return left;
}

std::vector<color*> &surface::getRight()
{
    return right;
}

const std::vector<color> &surface::getAll() const
{
    return elements;
}

color *surface::getTop()
{
    return &elements.at(0);
}

color *surface::getLeftest()
{
    return &elements.at(4);
}

color *surface::getRightest()
{
    return &elements.at(8);
}

pyramid::pyramid(const pyramid &p)
{
    front = new surface(*p.front);
    left = new surface(*p.left);
    right = new surface(*p.right);
    bottom = new surface(*p.bottom);
}

pyramid::pyramid(color frontColor, color leftColor, color rightColor, color bottColor)
{
    front = new surface(frontColor);
    left = new surface(leftColor);
    right = new surface(rightColor);
    bottom = new surface(bottColor);
}

pyramid::pyramid(std::string s)
{
    char delimiter = ',';

    size_t pos = 0;
    std::string tokens[4];
    int t = 0;

    while(s.size() > 0)
    {
        if(t == 4)
            throw std::runtime_error("pyramid::pyramid(std::string): illegal input string has more than 4 surfaces.");

        pos = s.find(delimiter);
        tokens[t++] = s.substr(0, pos);

        if(pos == std::string::npos)
            break;
        else
            s.erase(0, pos + 1);
    }

    if(t != 4)
        throw std::runtime_error("pyramid::pyramid(std::string): illegal input string has less than 4 surfaces.");

    front = new surface(tokens[0]);
    right = new surface(tokens[1]);
    left = new surface(tokens[2]);
    bottom = new surface(tokens[3]);
}

bool pyramid::equal(const pyramid &p) const
{
    if(!p.bottom->equal(*bottom)
    || !p.front->equal(*front)
    || !p.left->equal(*left)
    || !p.right->equal(*right))
        return false;
    else
        return true;
}

bool pyramid::isSolved() const
{
    return front->isSolved() && right->isSolved() && left->isSolved() && bottom->isSolved();
}

bool pyramid::isSolvedButCorners() const
{
    return front->isSolvedButCorners() && right->isSolvedButCorners()
            && left->isSolvedButCorners() && bottom->isSolvedButCorners();
}

const surface *pyramid::getFront() const
{
    return front;
}

const surface *pyramid::getRight() const
{
    return right;
}

const surface *pyramid::getLeft() const
{
    return left;
}

const surface *pyramid::getBottom() const
{
    return bottom;
}

void pyramid::turnLeft()
{
    surface *s = front;
    front = right;
    right = left;
    left = s;

    bottom->rotateCounterClockwise();
}

void pyramid::turnRight()
{
    turnLeft();
    turnLeft();
}

void pyramid::rotateRightCornerUp()
{
    front->rotateCounterClockwise();
    right->rotateClockwise();
    bottom->rotateClockwise();

    surface *s = front;
    front = bottom;
    bottom = left;
    left = s;
}

void pyramid::rotateRightCornerDown()
{
    rotateRightCornerUp();
    rotateRightCornerUp();
}

void pyramid::rotateUpperRight()
{
    auto itf = front->getUpper().begin();
    auto itl = left->getUpper().begin();
    auto itr = right->getUpper().begin();

    while(itf != front->getUpper().end())
    {
        color c = **itf;
        **itf++ = **itl;
        **itl++ = **itr;
        **itr++ = c;
    }
}

void pyramid::rotateUpperLeft()
{
    rotateUpperRight();
    rotateUpperRight();
}

void pyramid::rotateRightUp()
{
    auto itf = front->getRight().begin();
    auto itb = bottom->getLeft().begin();
    auto itr = right->getLeft().begin();

    while(itf != front->getRight().end())
    {
        color c = **itf;
        **itf++ = **itb;
        **itb++ = **itr;
        **itr++ = c;
    }

    // internal rotations
    front->rotateRight(false);
    right->rotateLeft();
}

void pyramid::rotateRightDown()
{
    rotateRightUp();
    rotateRightUp();
}

void pyramid::rotateRightestUp()
{
    color c = *front->getRightest();
    *front->getRightest() = *bottom->getLeftest();
    *bottom->getLeftest() = *right->getLeftest();
    *right->getLeftest() = c;
}

void pyramid::rotateRightestDown()
{
    rotateRightestUp();
    rotateRightestUp();
}

void pyramid::rotateTopRight()
{
    color c = *front->getTop();
    *front->getTop() = *left->getTop();
    *left->getTop() = *right->getTop();
    *right->getTop() = c;
}

void pyramid::rotateTopLeft()
{
    rotateTopRight();
    rotateTopRight();
}

bool solve(pyramid &start, std::list<std::string> &moves)
{
    if(start.isSolved())
        return true;

    std::list<pyramid> data = {start};

    std::unordered_map<const pyramid*, std::pair<pyramid*, const std::string*>> pred = {{&data.front(), {nullptr, nullptr}}};
    
    std::list<pyramid*> q = {&data.front()};

    pyramid *end = nullptr;

    while(!q.empty() && end == nullptr)
    {
        pyramid *p = q.front();
        q.pop_front();

        // generate all neighbors
        for(auto &op: solvingMoves)
        {
            data.push_back(pyramid(*p));
            pyramid *pp = &data.back();

            executeOperation(data.back(), op);

            if(!pred.contains(pp))
            {
                pred.insert({pp, {p, &op}});
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
        // now we just might to solve the corners yet:
        if(!end->isSolved())
        {
            if(end->getFront()->getAll().at(0) != end->getFront()->getAll().at(1))
            {

            }
        }

        pyramid *p = end;

        while(true)
        {
            auto &pnext = pred.at(p);
            p = pnext.first;

            if(p != nullptr)
                moves.push_front(*pnext.second);
            else
                break;
        }

        return true;
    }
    else
        return false;
}

void executeOperation(pyramid &p, const std::string &op)
{
    if(op == "turnLeft")
        p.turnLeft();
    else if(op == "turnRight")
        p.turnRight();
    else if(op == "rotateRightCornerUp")
        p.rotateRightCornerUp();
    else if(op == "rotateRightCornerDown")
        p.rotateRightCornerDown();
    else if(op == "rotateUpperRight")
        p.rotateUpperRight();
    else if(op == "rotateUpperLeft")
        p.rotateUpperLeft();
    else if(op == "rotateRightUp")
        p.rotateRightUp();
    else if(op == "rotateRightDown")
        p.rotateRightDown();
    else if(op == "rotateRightestUp")
        p.rotateRightestUp();
    else if(op == "rotateRightestDown")
        p.rotateRightestDown();
    else if(op == "rotateTopRight")
        p.rotateTopRight();
    else if(op == "rotateTopLeft")
        p.rotateTopLeft();
    else
        throw std::runtime_error("executeOperation(): unknown operation " + op);
    
    #if DEBUG
    std::cout << "Executed operation: " << op << "()." << std::endl;
    #endif
}
