#include <Physics\Capsule.h>
#include <MMath.h>
#include <assert.h>

using namespace MATH;
using namespace GEOMETRY;

Capsule::Capsule(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_)
{
    set(r_, sphereCentrePosA_, sphereCentrePosB_);
}

void Capsule::set(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_)
{
    assert(r_ > VERY_SMALL); // Blow up if the radius is zero or negative
    r = r_;
    sphereCentrePosA = sphereCentrePosA_;
    sphereCentrePosB = sphereCentrePosB_;
    // Generate vertices and normals whenever the radius or centre position is changed
    generateVerticesAndNormals();
}

void Capsule::generateVerticesAndNormals()
{
    vertices.clear();
    normals.clear();
    Vec3 up = VMath::normalize(Vec3(0.0f, 1.0f, 0.0f));
    Vec3 BottomCenter = sphereCentrePosA;
    Vec3 distanceVectorA_B = sphereCentrePosB - sphereCentrePosA;
   
    if (abs(VMath::dot(distanceVectorA_B, up)) > 0.999f) {
        up = Vec3(1.0f, 0.0f, 0.0f); // If the direction is almost parallel to up, use a different up vector
    }

    const float deltaTheta = 1.0f;
    const int rings = 50; // number of rings in the cylinder

    Vec3 A_B_Direction = VMath::normalize(distanceVectorA_B);
    if (abs(VMath::dot(A_B_Direction, up) > 0.999f)) {
        up = Vec3(1.0f, 0.0f, 0.0f); // If the direction is almost parallel to up, use a different up vector
    }
    Vec3 perpindicularLine = VMath::normalize(VMath::cross(A_B_Direction, up));
    Vec3 perpinduclarLineUP = VMath::normalize(VMath::cross(A_B_Direction, perpindicularLine ));

    const float deltaPhi = 2.0f;
    // First half circle
    for (float thetaDeg = 0.0f; thetaDeg <= 90.0f; thetaDeg += deltaTheta)
    {
        // Build a ring
        Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
        for (float phiDeg = 0.0f; phiDeg <= 360.0f; phiDeg += deltaPhi) {
            // Rotate a point in the ring around the y-axis to build a sphere!
            Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
            circle = rotationMatrix * circle;
            // Push the circle point to our vertices array, but offset by the circle centre
            vertices.push_back(sphereCentrePosA + circle);
            // The normal of a sphere points outwards to the ring
            normals.push_back(VMath::normalize(circle));
        }
        
    }
    // Second half circle
    for (float thetaDeg = 0.0f; thetaDeg <= 90.0f; thetaDeg += deltaTheta)
    {
        // Build a ring
        Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
        for (float phiDeg = 0.0f; phiDeg <= 360.0f; phiDeg += deltaPhi) {
            // Rotate a point in the ring around the y-axis to build a sphere!
            Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
            circle = rotationMatrix * circle;
            // Push the circle point to our vertices array, but offset by the circle centre
            vertices.push_back(sphereCentrePosB + -circle);
            // The normal of a sphere points outwards to the ring
            normals.push_back(VMath::normalize(circle));
        }
        
    }
    // build rings for capsule
    for (int currentRing = 0; currentRing < rings; currentRing++)
    {
        float t = (float)currentRing / (float)rings;
        Vec3 ringCenter = BottomCenter + (distanceVectorA_B * t); // Moves step-by-step from A to B
        for (float thetaDeg = 0.0f; thetaDeg <= 360.0f; thetaDeg += deltaTheta) {
            
            float cosThetaDeg = cos(thetaDeg);
            float sinThetaDeg = sin(thetaDeg);
            
            Vec3 circle = (perpindicularLine * cosThetaDeg) + (perpinduclarLineUP * sinThetaDeg);
      
            Vec3 vertexPos = ringCenter + (circle * r);
                vertices.push_back(vertexPos);
                normals.push_back(circle);
            
        }
    }
    // Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
    storeMeshData(GL_POINTS);
}
