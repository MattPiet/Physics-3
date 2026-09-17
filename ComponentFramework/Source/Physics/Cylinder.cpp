#include <Physics\Cylinder.h>
#include <MMath.h>
#include <assert.h>

using namespace MATH;
using namespace GEOMETRY;

Cylinder::Cylinder(float r_, MATH::Vec3 capCentrePosA_, MATH::Vec3 capCentrePosB_)
{
    set(r_, capCentrePosA_, capCentrePosB_);
}

void Cylinder::set(float r_, MATH::Vec3 capCentrePosA_, MATH::Vec3 capCentrePosB_)
{
    assert(r_ > VERY_SMALL); // Blow up if the radius is zero or negative
    r = r_;
    capCentrePosA = capCentrePosA_;
    capCentrePosB = capCentrePosB_;
    // Generate vertices and normals whenever the radius or centre position is changed
    generateVerticesAndNormals();
}

void Cylinder::generateVerticesAndNormals()
{
    /* 
     * I first clear the Verts and Normals incase set gets called during runtime 
     * Then I basically replicated the page displayed on the assignment doc in Ericsons book
     */
    vertices.clear();
    normals.clear();
    Vec3 up = VMath::normalize(Vec3(0.0f, 1.0f, 0.0f));
    Vec3 q = capCentrePosB;
    Vec3 BottomCenter = capCentrePosA;
    Vec3 distanceVectorA_B = capCentrePosB - capCentrePosA;
   
    if (abs(VMath::dot(distanceVectorA_B, up)) > 0.999f) {
        up = Vec3(1.0f, 0.0f, 0.0f); // If the direction is almost parallel to up, use a different up vector
    }
    Vec3 n = VMath::normalize(VMath::cross(distanceVectorA_B, up));
    Vec3 x =  BottomCenter + (n * r);
    Vec3 v = x - BottomCenter;
    Vec3 w = distanceVectorA_B * (VMath::dot(v, distanceVectorA_B) / VMath::dot(distanceVectorA_B, distanceVectorA_B));
    float isCylinder = VMath::dot((v - w), (v - w)) - (r * r);
    std::cout << "Cylinder Value is: " << isCylinder << std::endl;

    
    // We need to fill the vertices and normals arrays with the correct data for a sphere
    // deltaTheta governs how many points per ring. Try messing with it
    const float deltaTheta = 1.0f;
    const int rings = 20; // number of rings in the cylinder
    const int capRingDensity = r * 5; // this is the amount of rings within the cap, I found 5 as a good scalar number through trial and error
    
    /* all of this part is to set up for the calculations within the for loops
     * My idea when coming up with this whole mess was to apple what Umer had for the sphere but use it for the Cylinder
     * Basically you make a ring of verticies then place them up a vector that travels from the bottom cap to the top cap
    */
    Vec3 A_B_Direction = VMath::normalize(distanceVectorA_B);
    if (abs(VMath::dot(A_B_Direction, up) > 0.999f)) {
        up = Vec3(1.0f, 0.0f, 0.0f); // If the direction is almost parallel to up, use a different up vector
    }
    Vec3 perpindicularLine = VMath::normalize(VMath::cross(A_B_Direction, up));
    Vec3 perpinduclarLineUP = VMath::normalize(VMath::cross(A_B_Direction, perpindicularLine ));
  
    for (int currentRing = 0; currentRing < rings; currentRing++) {
            float t = (float)currentRing / (float)rings;
            Vec3 ringCenter = BottomCenter + (distanceVectorA_B * t); // Moves step-by-step from A to B
        for (float thetaDeg = 0.0f; thetaDeg <= 360.0f; thetaDeg += deltaTheta) {
            
            float cosThetaDeg = cos(thetaDeg);
            float sinThetaDeg = sin(thetaDeg);
            
            Vec3 circle = (perpindicularLine * cosThetaDeg) + (perpinduclarLineUP * sinThetaDeg);
      
            Vec3 vertexPos = ringCenter + (circle * r);
            
            if (currentRing == 0 || currentRing == rings - 1)
            {
                for (int i = 0; i < capRingDensity; i++)
                {
                    if (i == currentRing)
                    {
                        vertices.push_back(vertexPos);
                        normals.push_back(circle);
                    }
                    else
                    {
                        /* This is to make the actual cap, it basically just creates a number of rings = to capRingDensity
                         * So you make the first circle then make another thats smaller then another then another and so on
                         * each ring will have = distance because of the calculation ( r - i * r / capRingDensity) 
                         * That calculation will always work because the capRingDensity also scales off of the radius making the cap always look nice
                         */
                        float cosThetaDeg = cos(thetaDeg);
                        float sinThetaDeg = sin(thetaDeg);
                        Vec3 circle = (perpindicularLine * cosThetaDeg) + (perpinduclarLineUP * sinThetaDeg);
                        Vec3 vertexPos = ringCenter + (circle * ( r - i * r / capRingDensity));
                        vertices.push_back(vertexPos);
                        normals.push_back(circle);
                    }
                }
            }
            else
            {
                vertices.push_back(vertexPos);
                normals.push_back(circle);
            }
        }
    }
    // Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
    storeMeshData(GL_POINTS);

}
