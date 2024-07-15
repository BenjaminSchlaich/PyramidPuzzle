
#pragma once

#define UNDEFINED 0 // using this macro for refactoring step by step from the pyramid code.

#include <vector>
#include <iostream>
#include <list>

typedef unsigned int color;

extern const color RED;
extern const color GREEN;
extern const color BLUE;
extern const color YELLOW;
extern const color ORANGE;
extern const color WHITE;

/// @brief These are the operations that can be executed
enum Operation  { OP_NOOP
                , OP_TURN_TOP1, OP_TURN_TOP2, OP_TURN_BOTTOM1, OP_TURN_BOTTOM2
                , OP_TURN_RIGHT1, OP_TURN_RIGHT2, OP_TURN_LEFT1, OP_TURN_LEFT2
                , OP_TURN_FRONT1, OP_TURN_FRONT2, OP_TURN_BACK1, OP_TURN_BACK2
                , OP_ROT_UP, OP_ROT_DOWN, OP_ROT_LEFT, OP_ROT_RIGHT};

extern const std::list<Operation> allOperations;

/// print a color in just one letter
void printColor(std::ostream &os, const color &c);

class cube;

/// print a cube in an understandable manner
std::ostream &operator<<(std::ostream &os, const cube &p);

class surface;

/// equivalent to surface::equal(surface)
bool operator==(const surface &s1, const surface &s2);

class surface   
{
    public:

    /// copy constructor that works by duplicating everything!
    surface(const surface &s);

    /// initialize the surface with one color only, c.
    surface(color c);

    /// initialize the surface from a string encoding described in the constructor cube(std::string).
    surface(std::string &s);

    /// checks if all face colors are equal
    bool equal(const surface &s) const;

    /// computes a hash value for this surface, that is equal independent of the direction.
    size_t computeHash() const noexcept;

    /// checks if the surface has one color only
    bool isSolved() const;

    /// do a clockwise rotation
    void rotateClockwise();

    /// do a counter-clockwise rotation
    void rotateCounterClockwise();

    /// set only the top three elements, in order
    void setTop(unsigned int elements);

    /// set only the left three elements, in order
    void setLeft(unsigned int elements);

    /// set only the right three elements, in order
    void setRight(unsigned int elements);

    /// set only the bottom three elements, in order
    void setBottom(unsigned int elements);

    /// returns the color bitfield
    unsigned int getColors() const;

    private:

    /// sets the masked section of the <elements> to the masked part of <selection>
    void setByMask(unsigned int mask, unsigned int selection);

    /**
     * The contents of this surface, ordered in the following way:
     *          0 1 2
     *          3 4 5
     *          6 7 8
     * An integer is suficcient, as we have 6 colors -> 3 bits per color
     * -> 3 * 9 = 27 < 32, which is the size of one int.
     */
    unsigned int elements;

    static const size_t hashWeights[];
};

class hashCube;

/**
 * Represents a magic cube from the following perspective:
 * The cube stands on the bottom surface, with one surface in the front, and two towards the left and right back.
 */
class cube
{
    friend class hashCube;

    public:

    /// this is for graph-algorithms that find it useful
    mutable bool marked = false;

    /// explicit copy constructor, that duplicates all memory from pointers too
    cube(const cube &p);

    /// construct a cube with the following colors assigned to each surface
    cube(color front, color left, color right, color bott);

    /**
     * Construct a cube from a runlength-encoded string, formated in the following way:
     * First the front surface, then the right, then left and finally bottom are encoded.
     * The surfaces are separated by commas. A surface is encoded in the following scheme:
     * The tiles are color-coded one after another (top-down, left-right - same order as in the internal rep.),
     * where r=red, g=green, b=blue, y=yellow. A number after a letter means, that this letter appears that many times.
     * Example: All yellow surface: "y9".
     * Another example: The following surface:
     *                          r
     *                         ggg      == "rg3y3rr"
     *                        yyyrr
     * An entire cube encoding: "g3brrb3,r6gyy,y4g5,brbby3bb"
     */
    cube(std::string s);

    /// checkes whether p is exactly the same cube.
    bool equal(const cube &p) const;

    /// checks whether p is the same cube up to whole rotations
    bool equivalent(const cube &p) const;

    /// equality by equivalence as defined above
    bool operator==(const cube&p) const;

    /// checks if the cube is in some position where all faces have one color only
    bool isSolved() const;

    // Rotations of parts of the cube
    void turnFront1();

    void turnFront2();

    void turnBack1();

    void turnBack2();

    void turnTop1();

    void turnTop2();

    void turnLeft1();

    void turnLeft2();

    void turnRight1();

    void turnRight2();

    void turnBottom1();

    void turnBottom2();

    // Rotations of the whole cube
    void rotLeft();
    void rotRight();
    void rotUp();
    void rotDown();

    surface getFront() const;

    surface getBack() const;

    surface getRight() const;

    surface getLeft() const;

    surface getTop() const;

    surface getBottom() const;

    /// saves this cube to a string that is of the format we read in the constructor from strings.
    std::string storageString() const;

    private:

    surface front, back, left, right, top, bottom;
};

bool solve(cube &p, std::list<Operation> &moves);

void executeOperation(cube &p, Operation op);

std::string operationToString(const Operation &op);

/// hash a cube into a set or map of a standard container:
struct hashCube
{
    static size_t computeHash(const cube &p) noexcept
    {
        // this function must be commutative, for whole-rotations to be invariant.
        return p.front.computeHash() ^ p.back.computeHash() 
                ^ p.left.computeHash() ^ p.right.computeHash()
                ^ p.top.computeHash() ^ p.bottom.computeHash();
    }

    size_t operator()(const cube& p) const noexcept
    {
        return computeHash(p);
    }
};
