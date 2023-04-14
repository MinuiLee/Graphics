#include "BSPTree.h"
#include <unordered_map>
#include <algorithm>
#include <algorithm>
int loop = 4;

BSPTree::BSPTree(std::vector<Mesh*> &meshes, std::vector<Plane>* ptr)
{
	int triangleSum = 0;
	std::vector<std::vector<GLuint>> indices;

	for (Mesh* m : meshes)
	{
		indices.push_back(m->indexArray);
		triangleSum += static_cast<int>(m->indexArray.size()) / 3;
	}

	plane_ptr = ptr;

	node = BuildBSTtree(meshes, indices, triangleSum);

	if (node)
	{
		InitVAO(node);
	}
}

void BSPTree::Draw(Shader* shader)
{
	if (node)
	{
		DrawTree(node, shader);
	}
}

void BSPTree::DrawPlanes(Shader* shader)
{
	if (node)
	{
		DrawPlanes_(node, shader);
	}
}

BSPNode* BSPTree::BuildBSTtree(std::vector<Mesh*>& meshes, std::vector<std::vector<GLuint>> &indices, int triangleSum)
{
	if (meshes.empty()) return nullptr;

	if (triangleSum < 500)
	{
		BSPNode* newNode = new BSPNode;
		newNode->indices = indices;
		newNode->meshes = meshes;
		newNode->front = nullptr;
		newNode->back = nullptr;
		return newNode;
	}

	int polyNum = static_cast<int>(meshes.size());

	Plane splitPlane = PickSplittingPlane(meshes, indices);

	plane_ptr->push_back(splitPlane);

	std::vector<std::vector<GLuint>> frontIndices, backIndices;
	std::vector<Mesh*> frontMeshes, backMeshes;

	int front = 0, behind = 0, straddle = 0;
	for (int i = 0; i < polyNum; ++i)
	{
		int f = 0, b = 0, co = 0;
		for (GLuint idx : indices[i])
		{
			float result = splitPlane.PlaneVsPoint(meshes[i]->vertexArray[idx].position);
			if (result > 0.f) f++;
			else if (result < 0.f) b++;
		}

		if (f != 0 && b != 0)
		{
			//straddle
			std::vector<GLuint> oFrontIdx;
			std::vector<GLuint> oBackIdx;
			SplitPolygon(splitPlane, meshes[i], indices[i], oFrontIdx, oBackIdx);

			if (!oFrontIdx.empty())
			{
				frontIndices.push_back(oFrontIdx);
				frontMeshes.push_back(meshes[i]);
			}
			if (!oBackIdx.empty())
			{
				backIndices.push_back(oBackIdx);
				backMeshes.push_back(meshes[i]);
			}
		}
		else if (f != 0)
		{
			frontIndices.push_back(indices[i]);
			frontMeshes.push_back(meshes[i]);
		}
		else
		{
			backIndices.push_back(indices[i]);
			backMeshes.push_back(meshes[i]);
		}
	}

	size_t fSize = 0;
	size_t bSize = 0;

	for (auto& ff : frontIndices)
		fSize += ff.size();
	for (auto& bb : backIndices)
		bSize += bb.size();

	BSPNode* frontNode = BuildBSTtree(frontMeshes, frontIndices, static_cast<int>(fSize) / 3);
	BSPNode* backNode = BuildBSTtree(backMeshes, backIndices, static_cast<int>(bSize) / 3);

	BSPNode* newNode = new BSPNode;
	newNode->front = frontNode;
	newNode->back = backNode;
	newNode->plane = splitPlane;

	if (frontNode == nullptr && backNode == nullptr)
	{
		newNode->meshes = meshes;
		newNode->indices = indices;
	}
	
	return newNode;
}

Plane BSPTree::PickSplittingPlane(std::vector<Mesh*> &meshes, std::vector<std::vector<GLuint>>& indices)
{
	int polyNum = static_cast<int>(meshes.size());

	// make planes x y z
	std::vector<Plane> planes;
	std::vector<float> xMedian;
	std::vector<float> yMedian;
	std::vector<float> zMedian;

	for (int i = 0; i < polyNum; ++i)
	{
		int indexSize = static_cast<int>(indices[i].size());
		std::vector<Vertex>& vertices = meshes[i]->vertexArray;
		std::vector<GLuint>& indices_ = indices[i];

		for (int j = 0; j < indexSize; j++)
		{
			xMedian.push_back(vertices[indices_[j]].position.x);
			yMedian.push_back(vertices[indices_[j]].position.y);
			zMedian.push_back(vertices[indices_[j]].position.z);
		}
	}

	std::sort(xMedian.begin(), xMedian.end());
	std::sort(yMedian.begin(), yMedian.end());
	std::sort(zMedian.begin(), zMedian.end());

	Plane p1;
	p1.Initialize(glm::vec3(1, 0, 0), xMedian[xMedian.size() / 2]);
	planes.push_back(p1);

	Plane p2;
	p2.Initialize(glm::vec3(0, 1, 0), yMedian[yMedian.size() / 2]);
	planes.push_back(p2);

	Plane p3;
	p3.Initialize(glm::vec3(0, 0, 1), zMedian[zMedian.size() / 2]);
	planes.push_back(p3);

	const float k = 0.0f;
	Plane bestPlane;
	float bestScore = FLT_MAX;

	for (Plane& p : planes)
	{
		int front = 0, behind = 0, straddle = 0;

		for (int i = 0; i < polyNum; ++i)
		{
			int f = 0, b = 0;
			for (GLuint & idx : indices[i])
			{
				float result = p.PlaneVsPoint(meshes[i]->vertexArray[idx].position);
				if (result < 0) behind++;
				else if (result > 0) front++;
			}

			/*if (f != 0 && b != 0) straddle++;
			else if (f) front++;
			else if (b) behind++;
			else
			{
				front++;
				behind++;
			}*/
		}

		float score = k * straddle + (1.0f - k) * abs(front - behind);
		if (score < bestScore)
		{
			bestScore = score;
			bestPlane = p;
		}
	}

	return bestPlane;
}

void BSPTree::SplitPolygon(Plane& plane, Mesh*& meshes,std::vector<GLuint>& indices, 
	std::vector<GLuint>& oFrontIdx, std::vector<GLuint>& oBackIdx)
{
	std::vector<Vertex> & vertices = meshes->vertexArray;
	int size = static_cast<int>(indices.size());

	for (int j = 0; j < size; j += 3)
	{
		bool f = false;
		bool b = false;

		for (int idx = j; idx < j + 3; idx++)
		{
			float result = plane.PlaneVsPoint(vertices[indices[idx]].position);
			if (result > 0.f) f  = true;
			else if(result < 0.f) b = true;
		}
		for (int idx = j; idx < j + 3; idx++)
		{
			if (b && !f)
			{
				oBackIdx.push_back(indices[idx]);
			}
			else if(f && !b)
			{
				oFrontIdx.push_back(indices[idx]);
			}
		}
	}
}

void BSPTree::DrawTree(BSPNode* node_, Shader* shader)
{
	if (node_->front)
		DrawTree(node_->front, shader);

	if (node_->back)
		DrawTree(node_->back, shader);

	if (node_->front == nullptr && node_->back == nullptr)
	{
		int size = static_cast<int>(node_->meshes.size());

		for (int i = 0; i < size; ++i)
		{
			shader->SetMat4fv(modelmatrix, "ModelMatrix");
			shader->SetVec3f(node_->color, "line_color");

			shader->Use();
			
			//Bind VAO
			glBindVertexArray(node_->meshes[i]->VAO);

			//Draw
			glDrawElements(GL_LINES, static_cast<GLsizei>(node_->indices[i].size()), GL_UNSIGNED_INT, nullptr);

			//CleanUp
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}
}

void BSPTree::DrawPlanes_(BSPNode* node_, Shader* shader)
{
	if (node_->front)
		DrawTree(node_->front, shader);

	if (node_->back)
		DrawTree(node_->back, shader);

	if (node_->front == nullptr && node_->back == nullptr)
	{
		node_->plane.Draw(shader, glm::vec3(0.f, 0.4f, 0.f));
	}
}

void BSPTree::InitVAO(BSPNode* node_)
{
	if (node_->front)
		InitVAO(node_->front);

	if (node_->back)
		InitVAO(node_->back);

	if (node_->front == nullptr && node_->back == nullptr)
	{
		node_->color.x = ((rand() % 100) / 200.0f) + 0.4f;
		node_->color.y = ((rand() % 100) / 200.0f) + 0.4f;
		node_->color.z = ((rand() % 100) / 200.0f) + 0.4f;

		int size = static_cast<int>(node_->meshes.size());
		node_->EBO = new GLuint[size];

		for (int i = 0; i < size; ++i)
		{
			int nrOfIndices = static_cast<int>(node_->indices[i].size());
			int nrOfPoints = static_cast<int>(node_->meshes[i]->vertexArray.size());
			
			//Gen EBO and bind and send data
			if (nrOfIndices > 0)
			{
				glGenBuffers(1, &node_->EBO[i]);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node_->EBO[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrOfIndices * sizeof(GLuint), node_->indices[i].data(), GL_STATIC_DRAW);
			}
		}
	}
}

void BSPTree::SaveTree(BSPNode* node_, std::string& str)
{
	if (node_->front)
		SaveTree(node_->front, str);

	if (node_->back)
		SaveTree(node_->back, str);

	if (node_->front == nullptr && node_->back == nullptr)
	{

	}
}
