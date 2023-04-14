#pragma once

#include <glm.hpp>
#include <vector>
#include "Object.h"
#include "AABB.h"
#include <map>

struct Node
{
	glm::vec3 center = glm::vec3();
	float halfWidth = 0.f;
	int currDepth = 0;

	GLuint* EBO = nullptr;

	glm::vec3 color = glm::vec3();

	AABB* aabb = nullptr;
	Node* pChildren = nullptr;
	std::map<Mesh*, std::vector<GLuint>> triangles;
	//std::vector<Object*> pObjects = std::vector<Object*>();
};

class Octree
{
public:
	Octree() : node(nullptr) {}
	Octree(std::vector<Mesh*>& meshes);
	//~Octree() {}

	void DrawAABB(Shader* shader);
	void DrawTriangles(Shader* shader);

	void InsertTriangles(Node* noed_, int triangleNum);
	void BuildChildNodes(Node* node_);

private:
	void DrawAABB_(Node* node_, Shader* shader);
	void DrawTriangle_(Node* node_, Shader* shader);
	void InitVAO(Node* node_);
	glm::mat4 modelmatrix = glm::mat4(1.f);
	Node* node = nullptr;
};
