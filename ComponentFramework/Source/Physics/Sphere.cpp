#include "Physics/Sphere.h"
#include <MMath.h>
#include <assert.h>

using namespace MATH;
using namespace GEOMETRY;

Sphere::Sphere(float r_, MATH::Vec3 centre_) {
	set(r_, centre_);
}

void Sphere::set(float r_, Vec3 centre_) {
	assert(r_ > VERY_SMALL); // Blow up if the radius is zero or negative
	r = r_;
	centre = centre_;
	// Generate vertices and normals whenever the radius or centre position is changed
	generateVerticesAndNormals();
}

float Sphere::getRadius() const { return r; }
Vec3 Sphere::getCentre() const { return centre; }

void Sphere::generateVerticesAndNormals()
{
	vertices.clear();
	normals.clear();
	// We need to fill the vertices and normals arrays with the correct data for a sphere
	// deltaTheta governs how many points per ring. Try messing with it
	const float deltaTheta = 2.0f;
	// deltaPhi governs how many rings there are in total. Try messing with it
	const float deltaPhi = 2.0f;

	for (float thetaDeg = 0.0f; thetaDeg <= 360.0f; thetaDeg += deltaTheta)
	{
		// Build a ring
		Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		for (float phiDeg = 0.0f; phiDeg <= 180.0f; phiDeg += deltaPhi) {
			// Rotate a point in the ring around the y-axis to build a sphere!
			Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
			circle = rotationMatrix * circle;
			// Push the circle point to our vertices array, but offset by the circle centre
			vertices.push_back(centre + circle);
			// The normal of a sphere points outwards to the ring
			normals.push_back(VMath::normalize(circle));
		}
	}
	// Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
	storeMeshData(GL_POINTS);
}