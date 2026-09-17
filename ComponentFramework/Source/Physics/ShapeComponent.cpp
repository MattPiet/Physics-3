#include "Physics/ShapeComponent.h"

using namespace GEOMETRY;
using namespace MATH;
ShapeComponent::ShapeComponent(Component* parent_, Sphere sphere_) :Component(parent_)
{
	shapeType = ShapeType::sphere;
	// We are sure at this point that shape should be a sphere
	// So we can safely make a shared pointer of type sphere with the Sphere constructor
	shape = std::make_shared<Sphere>(sphere_);
}

ShapeComponent::ShapeComponent(Component* parent_, GEOMETRY::Cylinder cylinder_) :Component(parent_)
{
	shapeType = ShapeType::cylinder;
	shape = std::make_shared<Cylinder>(cylinder_);
}

ShapeComponent::ShapeComponent(Component* parent_, GEOMETRY::Capsule capsule_) :Component(parent_)
{
	shapeType = ShapeType::capsule;
	shape = std::make_shared<Capsule>(capsule_);
}

ShapeComponent::ShapeComponent(Component* parent_, GEOMETRY::Box box_) :Component(parent_)
{
	shapeType = ShapeType::box;
	shape = std::make_shared<Box>(box_);
}

// TODO for Assignment 1:
// The other constructors that take in Cylinder, Capsule, or Box 

ShapeComponent::~ShapeComponent(){
	OnDestroy();
}

bool ShapeComponent::OnCreate()
{
	isCreated = true;
	return true;
}

void ShapeComponent::OnDestroy(){
	isCreated = false;
	shape->OnDestroy();
}

void ShapeComponent::Render() const
{
	shape->debugDraw();
}
