/// Umer Noor 2022
/// Originally written by Dr. Scott Fielder
/// Part of his MATH library. I moved it to GEOMETRY and inherited from Shape

#ifndef CAPSULE_H
#define CAPSULE_H

#include "Shape.h"
#include <Vector.h>
#include <vector>

namespace GEOMETRY {
    class Capsule : public Shape {
    private:
        /// A capsule is represented by two spheres swept along connecting axis.
        //REFERENCE: Robust Contact Creation for Physics Simulations, D. Gregorius
        // http://media.steampowered.com/apps/valve/2015/DirkGregorius_Contacts.pdf
        // Make these private like the Cylinder member variables
        float r;
        MATH::Vec3 sphereCentrePosA;
        MATH::Vec3 sphereCentrePosB;
    public:
        // Use the Set method in the constructor as that ensure you generate vertices
       // and normals
        Capsule(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_);

        // Just like before, remember to blow up if you get a dodgy radius and fire off
        //generateVerticesAndNormals at the end of the Set method
        void set(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_);
        // Don’t forget getters too
        MATH::Vec3 getSphereCentrePosA() const { return sphereCentrePosA; };
        MATH::Vec3 getSphereCentrePosB() const { return sphereCentrePosB; };
        float getRadius() const { return r; };
        // Fill the vertices and normals list with Vec3's to represent a capsule
        // Try spheres for the top and bottom, with a cylinder for the main body
        void generateVerticesAndNormals() override; 
    };
}
#endif