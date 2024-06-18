
#pragma once

#include <vector>
#include <iostream>
#include <list>

typedef unsigned int color;

extern const color RED;
extern const color GREEN;
extern const color BLUE;
extern const color YELLOW;

/// @brief These are the operations that can be executed
enum Operation  { OP_NOOP
                , OP_TURN_LEFT, OP_TURN_RIGHT, OP_RIGHT_CORNER_UP, OP_RIGHT_CORNER_DOWN, OP_LEFT_CORNER_UP, OP_LEFT_CORNER_DOWN
                , OP_UPPER_RIGHT, OP_UPPER_LEFT, OP_RIGHT_UP, OP_RIGHT_DOWN, OP_LEFT_UP, OP_LEFT_DOWN, OP_BACK_CLOCKWISE, OP_BACK_COUNTER_CLOCKWISE
                , OP_RIGHTEST_UP, OP_RIGHTEST_DOWN, OP_TOP_RIGHT, OP_TOP_LEFT};

/// print a color in just one letter
void printColor(std::ostream &os, const color &c);

class pyramid;

/// print a pyramid in an understandable manner
std::ostream &operator<<(std::ostream &os, const pyramid &p);

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

    /// initialize the surface from a string encoding described in the constructor pyramid(std::string).
    surface(std::string &s);

    /// checks if all face colors are equal
    bool equal(const surface &s) const;

    /// computes a hash value for this surface, that is equal independent of the direction.
    size_t computeHash() const noexcept;

    /// checks if the surface has one color only
    bool isSolved() const;

    /// checks if the surface is solved apart from the corners
    bool isSolvedButCorners() const;

    /// do a clockwise rotation
    void rotateClockwise();

    /// do a counter-clockwise rotation
    void rotateCounterClockwise();

    /// do a rotation only for the upper triangle
    void rotateUpper(bool clockwise = true);

    /// do a rotation only for the upper triangle
    void rotateLeft(bool clockwise = true);

    /// do a rotation only for the upper triangle
    void rotateRight(bool clockwise = true);

    /// set only the upper four elements, in order
    void setUpper(unsigned int elements);

    /// set only the left four elements, in order
    void setLeft(unsigned int elements);

    /// set only the right four elements, in order
    void setRight(unsigned int elements);

    /// returns a reference to the the colors bitfield
    unsigned int getColors() const;

    /// returns the color of the according edge
    color getTop() const;
    color getRightest() const;
    color getLeftest() const;

    /// these return pointers to the corners of this surface
    void setTop(color c);
    void setLeftest(color c);
    void setRightest(color c);

    private:

    /// sets the masked section of the <elements> to the masked part of <selection>
    void setByMask(unsigned int mask, unsigned int selection);

    /**
     * The contents of this surface, ordered in the following way:
     *              0
     *            1 2 3
     *          4 5 6 7 8
     */
    unsigned int elements;

    static const size_t hashWeights[];
};

class hashPyramid;

/**
 * Represents a magic pyramid from the following perspective:
 * The pyramid stands on the bottom surface, with one surface in the front, and two towards the left and right back.
 */
class pyramid
{
    friend class hashPyramid;

    public:

    /// this is for graph-algorithms that find it useful
    mutable bool marked = false;

    /// explicit copy constructor, that duplicates all memory from pointers too
    pyramid(const pyramid &p);

    /// construct a pyramid with the following colors assigned to each surface
    pyramid(color front, color left, color right, color bott);

    /**
     * Construct a pyramid from a runlength-encoded string, formated in the following way:
     * First the front surface, then the right, then left and finally bottom are encoded.
     * The surfaces are separated by commas. A surface is encoded in the following scheme:
     * The tiles are color-coded one after another (top-down, left-right - same order as in the internal rep.),
     * where r=red, g=green, b=blue, y=yellow. A number after a letter means, that this letter appears that many times.
     * Example: All yellow surface: "y9".
     * Another example: The following surface:
     *                          r
     *                         ggg      == "rg3y3rr"
     *                        yyyrr
     * An entire pyramid encoding: "g3brrb3,r6gyy,y4g5,brbby3bb"
     */
    pyramid(std::string s);

    /// checkes whether p is exactly the same pyramid.
    bool equal(const pyramid &p) const;

    /// checks whether p is the same pyramid up to whole rotations
    bool equivalent(const pyramid &p) const;

    /// equality by equivalence as defined above
    bool operator==(const pyramid&p) const;

    /// checks if the cube is in some position where all faces have one color only
    bool isSolved() const;

    /// checks if the cube is solved apart from the corners
    bool isSolvedButCorners() const;

    /**
     * Rotate the whole pyramid from the right to the left.
     * From the top, this would be rotating clockwise.
     */
    void turnLeft();

    /**
     * Rotate the whole pyramid from the left to the right.
     * From the top, this would be rotating counter-clockwise.
     */
    void turnRight();

    /**
     * Rotate the right corner up to the top, while keeping the left corner "fixed".
     * I.e. rotate the pyramid along the axis that goes through the left corner and the center of the right surface
     * in counter-clockwise direction, when seen from the left corner.
     */
    void rotateRightCornerUp();

    /**
     * Opposite movement of rotateRightCornerUp();
     */
    void rotateRightCornerDown();
    
    /**
     * Rotate the left corner up to the top, while keeping the right corner "fixed".
     * I.e. rotate the pyramid along the axis that goes through the right corner and the center of the left surface
     * in clockwise direction, when seen from the right corner.
     */
    void rotateLeftCornerUp();

    /// Opposite movement of rotateLeftCornerUp();
    void rotateLeftCornerDown();
    

    // rotate the upper four elements towards the right (from above this is a counter-clockwise rotation)
    void rotateUpperRight();

    // rotate the upper four elements towards the left (from above this is a clockwise rotation)
    void rotateUpperLeft();

    /// rotate the four elements on the lower-right part upwards, i.e.
    /// rotate them clockwise seen from the right corner along the axis formed by the
    /// right corner and the center of the left surface.
    void rotateRightUp();

    /** rotate the four elements on the lower-right part downwards, i.e.
     * rotate them counter-clockwise seen from the right corner along the axis formed by the
     * right corner and the center of the left surface.
     */
    void rotateRightDown();

    /** rotate the four elements on the lower-left part upwards, i.e.
     * rotate them counter-clockwise seen from the left corner along the axis formed by the
     * left corner and the center of the right surface.
     */
    void rotateLeftUp();

    /** rotate the four elements on the lower-left part downwards, i.e.
     * rotate them clockwise seen from the left corner along the axis formed by the
     * left corner and the center of the right surface.
     */
    void rotateLeftDown();

    /// rotate the back clockwise from the front perspective
    void rotateBackClockwise();

    /// rotate the back counter-clockwise from the front perspective
    void rotateBackCounterClockwise();
    
    /// rotate the right tip of the pyramid upwards
    void rotateRightestUp();

    /// rotate the right tip of the pyramid downwards
    void rotateRightestDown();

    /// rotate the top tip of the pyramid towards the right
    void rotateTopRight();

    /// rotate the top tip of the pyramid towards the left
    void rotateTopLeft();

    surface getFront() const;

    surface getRight() const;

    surface getLeft() const;

    surface getBottom() const;

    /// saves this pyramid to a string that is of the format we read in the constructor from strings.
    std::string storageString() const;

    private:

    surface front;

    surface left;

    surface right;

    surface bottom;
};

bool solve(pyramid &p, std::list<Operation> &moves);

void executeOperation(pyramid &p, Operation op);

std::string operationToString(const Operation &op);

/// hash a pyramid into a set or map of a standard container:
struct hashPyramid
{
    static size_t computeHash(const pyramid &p) noexcept
    {
        /*
        const unsigned int pf = p.front.getColors();
        const unsigned int pr = p.right.getColors();
        const unsigned int pl = p.left.getColors();
        const unsigned int pb = p.bottom.getColors();

        int p1 = std::popcount(pf);
        int p2 = std::popcount(pr);
        int p3 = std::popcount(pl);
        int p4 = std::popcount(pb);

        int x1 = p1*p2*p3*p4;

        return x1;//*/
        return p.front.computeHash() ^ p.right.computeHash() ^ p.left.computeHash() ^ p.bottom.computeHash();
    }

    size_t operator()(const pyramid& p) const noexcept
    {
        return p.front.computeHash() ^ p.right.computeHash() ^ p.left.computeHash() ^ p.bottom.computeHash();
    }
};
