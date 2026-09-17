#include "Assets/XMLAssetManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "GPU/ShaderComponent.h"
#include "GPU/MeshComponent.h"
#include "Physics/ShapeComponent.h"
#include "GPU/MaterialComponent.h"
#include "ECS/CameraActor.h"
#include "GPU/LightActor.h"
#include "Physics/Sphere.h"
#include "Physics/Cylinder.h"
#include <QMath.h>
#include "Core/Debug.h"
#include "Physics/TransformComponent.h"
#include <assert.h>

XMLAssetManager::XMLAssetManager()
{

	tinyxml2::XMLDocument doc;
	doc.LoadFile("XMLFiles/Scene0.xml");

	// Get the top of the node system
	tinyxml2::XMLElement* rootData = doc.RootElement();

	// Everything we load lives under <Scene0>.
	tinyxml2::XMLElement* sceneData = rootData->FirstChildElement("Scene0");

	for (const tinyxml2::XMLElement* child = rootData->FirstChildElement("Scene0")->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		AddMaterial(child);
		AddShader(child);
		AddCamera(child);
		AddLight(child);
		
		if (std::string(child->Name()) == "Mesh") {
			// Get mesh name and filename
			const char* meshName = child->Attribute("name");
			const char* meshFilename = child->Attribute("filename");
			AddComponent<MeshComponent>(meshName, nullptr, meshFilename);

			AddSphereShape(child);
			AddCylinderShape(child);
			AddCapsuleShape(child);
			AddBoxShape(child);
		}
	}

	// Now we have all the shared assets ready, time to build the actors
	for (const tinyxml2::XMLElement* child = rootData->FirstChildElement("Scene0")->FirstChildElement("Actor");
		child;
		child = child->NextSiblingElement())
	{
		if (std::string(child->Name()) == "Actor") {
			const char* actorName = child->Attribute("actorname");
			const char* parentName = child->Attribute("parent");
			Component* parent = nullptr;
			if (parentName != nullptr && std::string(parentName) != "none") {
				parent = GetComponent<Actor>(parentName).get();
			}

			Ref<Actor> actor = std::make_shared<Actor>(parent);
			// Add shared assets to the actor
			AddMeshToActor(child, actor);
			AddShaderToActor(child, actor);
			AddMaterialToActor(child, actor);
			AddShapeToActor(child, actor);
			// The transform is unique for the actor. Needs the parent too
			AddTransformToActor(child, actor, parent);
			// Add physics to the actor AFTER the transform. We need them to match in position and orientation
			AddPhysicsToActor(child, actor, parent);

			// Use OnCreate for the actor (which in turn fires OnCreate for each component).
			if (actor->OnCreate() == false) {
				Debug::Error(std::string("Actor ") + actorName + " could not load all of its assets", __FILE__, __LINE__);
			}
			AddComponent(child->Attribute("actorname"), actor);
		}
	}
}
XMLAssetManager::~XMLAssetManager()
{
	xmlAssets.clear();
}

Vec3 XMLAssetManager::ReadVec3(const tinyxml2::XMLElement* element, const char* xName, const char* yName, const char* zName) {
	// Make a vector from the three floats in the xml file
	return Vec3(
		element->FloatAttribute(xName),
		element->FloatAttribute(yName),
		element->FloatAttribute(zName)
	);
}

void XMLAssetManager::AddSphereShape(const tinyxml2::XMLElement* child)
{
	// tinyxml is liable to blow up if we make a tiny typo. Do some checks first before reading data
	const tinyxml2::XMLElement* shapeElement = child->FirstChildElement("Shape");
	if (shapeElement == nullptr) return; // get outta here if there is no Shape

	const tinyxml2::XMLElement* shapeTypeElement = shapeElement->FirstChildElement("Sphere");
	if (shapeTypeElement == nullptr) return; // get outta here if there is no Sphere

	// Get the name from the shapeElement. We store the shape under that name
	const char* shapeName = shapeElement->Attribute("name");

	// So far, so good. Time to load in the data
	// Use the helper function to read in a Vec3 from three floats
	Vec3 sphereCentre = ReadVec3(shapeTypeElement, "centreX", "centreY", "centreZ");
	float sphereRadius = shapeTypeElement->FloatAttribute("radius");

	GEOMETRY::Sphere sphere(sphereRadius, sphereCentre);
	AddComponent<ShapeComponent>(shapeName, nullptr, sphere);
}

void XMLAssetManager::AddCylinderShape(const tinyxml2::XMLElement* child)
{
	// tinyxml is liable to blow up if we make a tiny typo. Do some checks first before reading data
	const tinyxml2::XMLElement* shapeElement = child->FirstChildElement("Shape");
	if (shapeElement == nullptr) return; // get outta here if there is no Shape

	const tinyxml2::XMLElement* shapeTypeElement = shapeElement->FirstChildElement("Cylinder");
	if (shapeTypeElement == nullptr) return; // get outta here if there is no Sphere

	// Get the name from the shapeElement. We store the shape under that name
	const char* shapeName = shapeElement->Attribute("name");

	// So far, so good. Time to load in the data
	// Use the helper function to read in a Vec3 from three floats
	Vec3 cylinderPointA = ReadVec3(shapeTypeElement, "capCentrePosAX", "capCentrePosAY", "capCentrePosAZ");
	Vec3 cylinderPointB = ReadVec3(shapeTypeElement, "capCentrePosBX", "capCentrePosBY", "capCentrePosBZ");
	float cylinderRadius = shapeTypeElement->FloatAttribute("radius");

	GEOMETRY::Cylinder cylinder(cylinderRadius, cylinderPointA, cylinderPointB);
	AddComponent<ShapeComponent>(shapeName, nullptr, cylinder);
}

void XMLAssetManager::AddCapsuleShape(const tinyxml2::XMLElement* child)
{
	// tinyxml is liable to blow up if we make a tiny typo. Do some checks first before reading data
	const tinyxml2::XMLElement* shapeElement = child->FirstChildElement("Shape");
	if (shapeElement == nullptr) return; // get outta here if there is no Shape

	const tinyxml2::XMLElement* shapeTypeElement = shapeElement->FirstChildElement("Capsule");
	if (shapeTypeElement == nullptr) return; // get outta here if there is no Sphere

	// Get the name from the shapeElement. We store the shape under that name
	const char* shapeName = shapeElement->Attribute("name");

	// So far, so good. Time to load in the data
	// Use the helper function to read in a Vec3 from three floats
	Vec3 spherePointA = ReadVec3(shapeTypeElement, "sphereCentrePosAX", "sphereCentrePosAY", "sphereCentrePosAZ");
	Vec3 spherePointB = ReadVec3(shapeTypeElement, "sphereCentrePosBX", "sphereCentrePosBY", "sphereCentrePosBZ");
	float capsuleRadius = shapeTypeElement->FloatAttribute("radius");

	GEOMETRY::Capsule capsule(capsuleRadius, spherePointA, spherePointB);
	AddComponent<ShapeComponent>(shapeName, nullptr, capsule);
}

void XMLAssetManager::AddBoxShape(const tinyxml2::XMLElement* child)
{
	// TODO for assignment 1
	// tinyxml is liable to blow up if we make a tiny typo. Do some checks first before reading data
	const tinyxml2::XMLElement* shapeElement = child->FirstChildElement("Shape");
	if (shapeElement == nullptr) return; // get outta here if there is no Shape

	const tinyxml2::XMLElement* shapeTypeElement = shapeElement->FirstChildElement("Box");
	if (shapeTypeElement == nullptr) return; // get outta here if there is no Sphere

	// Get the name from the shapeElement. We store the shape under that name
	const char* shapeName = shapeElement->Attribute("name");

	// So far, so good. Time to load in the data
	// Use the helper function to read in a Vec3 from three floats
	Vec3 center = ReadVec3(shapeTypeElement, "centerX", "centerY", "centerZ");
	Vec3 halfExtents = ReadVec3(shapeTypeElement, "halfExtentX", "halfExtentY", "halfExtentZ");
	float angle = shapeTypeElement->FloatAttribute("angleDeg");
	Vec3 axis = ReadVec3(shapeTypeElement, "axisX", "axisY", "axisZ");

	GEOMETRY::Box box(center, halfExtents, QMath::angleAxisRotation(angle, axis));
	AddComponent<ShapeComponent>(shapeName, nullptr, box);
}

void XMLAssetManager::AddMaterial(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) == "Material") {
		const char* name = child->Attribute("name");
		const char* filename = child->Attribute("filename");
		AddComponent<MaterialComponent>(name, nullptr, filename);
	}
}

void XMLAssetManager::AddShader(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) == "Shader") {
		const char* name = child->Attribute("name");
		const char* vertFilename = child->Attribute("vertFilename");
		const char* fragFilename = child->Attribute("fragFilename");
		AddComponent<ShaderComponent>(name, nullptr, vertFilename, fragFilename);
	}
}

void XMLAssetManager::AddCamera(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) != "Camera") return;

	const char* cameraName = child->Attribute("cameraname");
	// Grab the <Transform> once, then read the numbers off it.
	const tinyxml2::XMLElement* transform = child->FirstChildElement("Transform");
	// If we got this far, time to load in the data
	Vec3 axis = ReadVec3(transform, "axisx", "axisy", "axisz");
	Vec3 cameraPos = ReadVec3(transform, "posx", "posy", "posz");
	float angleDeg = transform->FloatAttribute("angleDeg");
	Component* cameraParent = nullptr;

	Ref<CameraActor> camera = std::make_shared<CameraActor>(cameraParent);
	camera->AddComponent<TransformComponent>(nullptr, cameraPos, QMath::angleAxisRotation(angleDeg, axis));
	camera->OnCreate();
	AddComponent(cameraName, camera);
}

void XMLAssetManager::AddLight(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) != "Light") return;

	const char* lightName = child->Attribute("lightname");

	// Build Light
	Vec3 lightPos = ReadVec3(child, "posx", "posy", "posz");
	Vec4 colour = ReadVec3(child, "red", "green", "blue");
	colour.w = child->FloatAttribute("alpha");

	float intensity = child->FloatAttribute("intensity");

	Vec3 falloff = ReadVec3(child, "falloffx", "falloffy", "falloffz");

	// We've only coded a DirectionLight so far. Others will have to be next time!
	// const char* style = child->Attribute("lightstyle");
	LightStyle lightstyle = LightStyle::DirectionLight;

	Component* lightParent = nullptr;
	Ref<LightActor> light = std::make_shared<LightActor>(lightParent, lightstyle, lightPos, colour, intensity, falloff);
	light->OnCreate();
	AddComponent(lightName, light);
}

void XMLAssetManager::AddMeshToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Mesh")) {
		const char* name = child->FirstChildElement("Mesh")->Attribute("name");
		Ref<MeshComponent> mesh = GetComponent<MeshComponent>(name);
		actor->AddComponent<MeshComponent>(mesh);
	}

}

void XMLAssetManager::AddShaderToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Shader")) {
		const char* name = child->FirstChildElement("Shader")->Attribute("name");
		Ref<ShaderComponent> shader = GetComponent<ShaderComponent>(name);
		actor->AddComponent<ShaderComponent>(shader);
	}
}

void XMLAssetManager::AddMaterialToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Material")) {
		const char* name = child->FirstChildElement("Material")->Attribute("name");
		Ref<MaterialComponent> material = GetComponent<MaterialComponent>(name);
		actor->AddComponent<MaterialComponent>(material);
	}
}

void XMLAssetManager::AddShapeToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (std::string(child->FirstChildElement("Shape")->Name()) == "Shape") {
		const char* name = child->FirstChildElement("Shape")->Attribute("name");
		Ref<ShapeComponent> shape = GetComponent<ShapeComponent>(name);
		actor->AddComponent<ShapeComponent>(shape);
	}
}

void XMLAssetManager::AddTransformToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Component* parent)
{
	const tinyxml2::XMLElement* element = child->FirstChildElement("Transform");
	if (element == nullptr) return;

	// If we got this far, time to load in data
	Vec3 axis  = ReadVec3(element, "axisx", "axisy", "axisz");
	Vec3 pos   = ReadVec3(element, "posx", "posy", "posz");
	Vec3 scale = ReadVec3(element, "scalex", "scaley", "scalez");
	float angleDeg = child->FirstChildElement("Transform")->FloatAttribute("angleDeg");;
		
	actor->AddComponent<TransformComponent>(parent, pos, QMath::angleAxisRotation(angleDeg, axis), scale);
}

void XMLAssetManager::AddPhysicsToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Component* parent)
{
	// TODO for assignment 3
	// Add a default physics component to the actor, but don't forget to match transform component's position & orientation
	// You're gonna have to assume the transform component has already been built (fingers crossed!)
}

