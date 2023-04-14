#pragma once
#include <glm.hpp>
#include <vector>
#include "Plane.h"
#include "Object.h"
#include <map>

struct BSPNode
{
	std::vector<Mesh*> meshes;
	std::vector<std::vector<GLuint>> indices;

	BSPNode* front = nullptr;
	BSPNode* back = nullptr;

	GLuint* EBO = nullptr;
	glm::vec3 color = glm::vec3();

	Plane plane;
};

class BSPTree
{
public:
	BSPTree() : node(nullptr), plane_ptr(nullptr) {}
	BSPTree(std::vector<Mesh*> &meshes, std::vector<Plane>* ptr);
	void Draw(Shader* shader);
	void DrawPlanes(Shader* shader);
	std::vector<Plane>* plane_ptr;
private:
	BSPNode* BuildBSTtree(std::vector<Mesh*> &meshes, std::vector<std::vector<GLuint>> &indicies, int triangleSum);
	Plane PickSplittingPlane(std::vector<Mesh*>& meshes, std::vector<std::vector<GLuint>>& indices);

	void SplitPolygon(Plane& plane, Mesh*& meshes, std::vector<GLuint>& indices, std::vector<GLuint>& oFrontIdx, std::vector<GLuint>& oBackIdx);
	BSPNode* node;

	void DrawTree(BSPNode* node_, Shader* shader);
	void DrawPlanes_(BSPNode* node_, Shader* shader);

	void InitVAO(BSPNode* node);

	void SaveTree(BSPNode* node, std::string& str);
private:
	glm::mat4 modelmatrix = glm::mat4(1.f);
};