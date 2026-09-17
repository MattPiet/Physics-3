#include <Physics\Box.h>
#include <MMath.h>
#include <assert.h>

using namespace MATH;
using namespace GEOMETRY;

Box::Box(MATH::Vec3 center_, MATH::Vec3 halfExtents_, MATH::Quaternion orientation_)
{
    set(center_, halfExtents_, orientation_);
}

void Box::set(MATH::Vec3 center_, MATH::Vec3 halfExtents_, MATH::Quaternion orientation_)
{
    center = center_;
    halfExtents = halfExtents_;
    orientation = orientation_;
    generateVerticesAndNormals();
}
// ![Box Collision](../../ExamplePhotos/Boxcollexample.jpg)
void Box::generateVerticesAndNormals()
{
    vertices.clear();
    normals.clear();
    
    Vec3 corners[8];

    corners[0] = center + Vec3(-halfExtents.x, -halfExtents.y,  halfExtents.z); // Bottom-Left
    corners[1] = center + Vec3( halfExtents.x, -halfExtents.y,  halfExtents.z); // Bottom-Right
    corners[2] = center + Vec3( halfExtents.x,  halfExtents.y,  halfExtents.z); // Top-Right
    corners[3] = center + Vec3(-halfExtents.x,  halfExtents.y,  halfExtents.z); // Top-Left

    corners[4] = center + Vec3(-halfExtents.x, -halfExtents.y, -halfExtents.z); // Bottom-Left
    corners[5] = center + Vec3( halfExtents.x, -halfExtents.y, -halfExtents.z); // Bottom-Right
    corners[6] = center + Vec3( halfExtents.x,  halfExtents.y, -halfExtents.z); // Top-Right
    corners[7] = center + Vec3(-halfExtents.x,  halfExtents.y, -halfExtents.z); // Top-Left

    int lineIndices[24] = {
        0, 1,  1, 2,  2, 3,  3, 0, // Front face edges
        4, 5,  5, 6,  6, 7,  7, 4, // Back face edges
        0, 4,  1, 5,  2, 6,  3, 7  // Connecting edges (
    };
    
    for (int i = 0; i < 24; ++i) {
        vertices.push_back(corners[lineIndices[i]]);
        Vec3 rotatedNormal = QMath::rotate(corners[lineIndices[i]],orientation);
        normals.push_back(VMath::normalize(rotatedNormal));
    }
    
    // Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
    storeMeshData(GL_LINES);

}
