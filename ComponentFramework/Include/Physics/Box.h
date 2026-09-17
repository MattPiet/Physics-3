/// Umer Noor 2022
/// Originally written by Dr. Scott Fielder
/// Part of his MATH library. I moved it to GEOMETRY and inherited from Shape

#ifndef BOX_H
#define BOX_H

#include "Shape.h"
#include <Vector.h>
#include <Quaternion.h>
#include <vector>

namespace GEOMETRY {
    class Box : public Shape {
    private:
        // Represent an Orientated Bounding Box by centre position, half extents, and
        //orientation. REFERENCE: Real Time Collision Detection by Ericson
        // Ericson recommends keeping the orientation as a matrix for fast collision
        //detection, but I love quaternions too much
        MATH::Vec3 center;
        MATH::Vec3 halfExtents;
        MATH::Quaternion orientation;
    public:
        Box(MATH::Vec3 center_, MATH::Vec3 halfExtents_, MATH::Quaternion orientation_);
        // The Set method is handy, as it ensures I generate vertices and normals
        // whenever the box is changed
        void set(MATH::Vec3 center_, MATH::Vec3 halfExtents_, MATH::Quaternion
        orientation_);
        // Plus a bunch of annoying getters

        // Fill the vertices and normals list with Vec3's to represent a box
        void generateVerticesAndNormals() override; 
    };
}
#endif