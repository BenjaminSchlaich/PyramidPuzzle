
#pragma once

#include <vector>
#include <iostream>
#include <list>

enum color {RED, GREEN, BLUE, YELLOW};

/// print a color in just one letter
std::ostream &operator<<(std::ostream &os, const color &c);

class pyramid;

/// print a pyramid in an understandable manner
std::ostream &operator<<(std::ostream &os, const pyramid &p);

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

    /// these return pointers to the corners of this surface
    color getTop();
    color getLeftest();
    color getRightest();

    private:

    /**
     * The contents of this surface, ordered in the following way:
     *              0
     *            1 2 3
     *          4 5 6 7 8
     */
    unsigned int elements;
};

/**
 * Represents a magic pyramid from the following perspective:
 * The pyramid stands on the bottom surface, with one surface in the front, and two towards the left and right back.
 */
class pyramid
{
    public:

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

    /// checks whether all faces are equal
    bool equal(const pyramid &p) const;

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
     * I.e. rotate the pyramid along the axis that goes through the left corner and the center of the right surface.
     */
    void rotateRightCornerUp();

    /**
     * Opposite movement of rotateRightCornerUp();
     */
    void rotateRightCornerDown();

    /*
    void rotateLeftCornerUp();

    void rotateLeftCornerDown();
    */

    void rotateUpperRight();

    void rotateUpperLeft();

    void rotateRightUp();

    void rotateRightDown();

    /*
    void rotateLeftUp();

    void rotateLeftDown();
    */
    
    void rotateRightestUp();

    void rotateRightestDown();

    void rotateTopRight();

    void rotateTopLeft();

    const surface *getFront() const;

    const surface *getRight() const;

    const surface *getLeft() const;

    const surface *getBottom() const;

    private:

    surface *front;

    surface *left;

    surface *right;

    surface *bottom;
};

bool solve(pyramid &p, std::list<std::string> &moves);

/**
 *  executes the operation defined by <op> on <p>.
 *  Valid operation strings are:
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
 * */
void executeOperation(pyramid &p, const std::string &op);
