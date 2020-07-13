#ifndef _cMessObject_HG_
#define _cMessObject_HG_

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class cMeshObject
{
public:
	cMeshObject();
	~cMeshObject() {};
	std::string meshName;
	glm::vec3 position;				//0,0,0 (origin)
	glm::vec4 colourRGBA;			//0,0,0,1 (black)
	glm::vec3 orientation;			//roation: XYZ
	float scale;					//1.0f
	bool isWireFrame;				//false
};

#endif	// _cMessObject_HG_