#include "cMeshObject.h"

cMeshObject::cMeshObject()
{
	//clear the glm number
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->coluorRGBA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->orientation = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = 1.0f;
	this->isWireFrame = false;

	return;
}