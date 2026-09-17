// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder. Lucky for me I was in that class (Game Engine 4)

#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "glew.h" // so we can make a GLuint
#include "ECS/Component.h"

class MaterialComponent: public Component {
	GLuint textureID;
	const char* filename;

	public:
		MaterialComponent(Component* parent_, const char* filename_);
		~MaterialComponent();
		bool OnCreate() override;
		void OnDestroy() override;
		inline GLuint getTextureID() const { return textureID; }
};
#endif;