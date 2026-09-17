#include "Physics/TransformComponent.h"
#include <MMath.h>

TransformComponent::TransformComponent(Component* parent_):Component(parent_) 
{
}
TransformComponent::TransformComponent(Component* parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_) 
	: Component(parent_), pos(pos_), orientation(orientation_), scale(scale_)
{
}
TransformComponent::~TransformComponent() {
// let's set the scale to 1 so we don't foul ourselves up
	scale = Vec3(1.0f, 1.0f, 1.0f);
	pos = Vec3(0.0f, 0.0f, 0.0f);
	orientation = Quaternion(1.0, Vec3(0.0f, 0.0f, 0.0f));
}

bool TransformComponent::OnCreate() {
	return true;
}
void TransformComponent::OnDestroy() {}
void TransformComponent::Update(const float deltaTime) {}
void TransformComponent::Render() const {}

Matrix4 TransformComponent::GetTransformMatrix() const
{
	Matrix4 m;
	// we're gonna get translation, rotation, and scale
	// so let's do rotation first, then scale, then translation
	// remember that means Translate * Scale * Rotation
	m = MMath::translate(pos) * MMath::scale(scale) * MMath::toMatrix4(orientation);
	return m;
}
