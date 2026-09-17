/// Umer Noor 2022
/// Originally written by Dr. Scott Fielder
/// Part of his MATH library. I moved it to GEOMETRY and inherited from Shape

#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <Vector.h>
#include <vector>

namespace GEOMETRY {
	class Sphere : public Shape {
	private:
		// Keep radius and centre position private (I do listen to Scott sometimes)
		// We need to build vertices and normals whenever they change	
		float r;
		MATH::Vec3 centre;

	public:
		Sphere(float r_, MATH::Vec3 centre_);
		void set(float r_, MATH::Vec3 centre_);
		float getRadius() const;
		MATH::Vec3 getCentre() const;

		/// print the values of the sphere and add a comment if you wish
		void print(const char* comment = nullptr) {
			if (comment) printf("%s\n", comment);
			printf("%f %f %f %f\n", centre.x, centre.y, centre.z, r);
		}

		void generateVerticesAndNormals() override;

		// TODO for assignment 2:
		// RayIntersectionInfo rayIntersectionInfo(const Ray& ray) const override;
	};
}
#endif