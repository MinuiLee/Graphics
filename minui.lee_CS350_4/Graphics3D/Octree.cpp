#include "Octree.h"
#include "BoundingSphere.h"

Octree::Octree(std::vector<Mesh*>& meshes)
{
	if (meshes.empty())
		return;

	std::vector<Vertex> vertices;

	glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());

	node = new Node();
	
	int triangleSum = 0;

	for (Mesh* m : meshes)
	{
		max = glm::max(max, m->GetMax());
		min = glm::min(min, m->GetMin());

		node->triangles.insert({ m, m->indexArray });
		triangleSum += static_cast<int>(m->indexArray.size()) / 3;
	}

	node->center = (max + min) / 2.f;

	node->halfWidth = abs(max.x - node->center.x);
	node->halfWidth = glm::max(node->halfWidth, abs(max.y - node->center.y));
	node->halfWidth = glm::max(node->halfWidth, abs(max.z - node->center.z));

	node->aabb = new AABB(glm::vec3(node->center - node->halfWidth), glm::vec3(node->center + node->halfWidth));

	node->currDepth = 0;

	InsertTriangles(node, triangleSum);
}

void Octree::DrawAABB_(Node* node_, Shader* shader)
{
	if (node_ == nullptr) return;
	
	if(intersectedNode != node_)
		node_->aabb->Draw(shader, node_->color);

	if (node_->pChildren == nullptr) return;

	for (int i = 0; i < 8; ++i)
	{
		DrawAABB_(&node_->pChildren[i], shader);
	}
}

void Octree::DrawAABB(Shader* shader)
{
	if (intersectedNode)
	{
		intersectedNode->aabb->Draw(shader, glm::vec3(1.f, 0.f, 0.f));
	}
	
	if (node)
	{
		DrawAABB_(node, shader);
	}
}

void Octree::DrawTriangles(Shader* shader)
{
	if (node) DrawTriangle_(node, shader);
}

void Octree::InsertTriangles(Node* node_, int triangleNum)
{
	if (triangleNum < 5000) return;

	BuildChildNodes(node_);

	int triangleSum[8] = { 0 };

	for (auto& p : node_->triangles)
	{
		int size = static_cast<int>(p.second.size());

		for (int i = 0; i < size; i += 3)
		{
			std::vector<int> indicies;
			for (int j = i; j < i + 3; ++j)
			{
				int index;
				glm::vec3 delta = p.first->vertexArray[p.second[j]].position - node_->center;

				if (delta.x < 0)
				{
					if (delta.y < 0)
					{
						if (delta.z < 0) index = 0;
						else index = 4;
					}
					else
					{
						if (delta.z < 0) index = 2;
						else index = 6;
					}
				}
				else
				{
					if (delta.y < 0)
					{
						if (delta.z < 0) index = 1;
						else index = 5;
					}
					else
					{
						if (delta.z < 0) index = 3;
						else index = 7;
					}
				}

				if (std::find(indicies.begin(), indicies.end(), index) == indicies.end())
				{
					indicies.push_back(index);
					triangleSum[index] += 1;
				}
			}

			for (int idx : indicies)
			{
				auto& temp = node_->pChildren[idx].triangles;
				if (temp.find(p.first) == temp.end())
				{
					temp.insert({ p.first, std::vector<GLuint>() });
				}

				for (int j = i; j < i + 3; ++j)
				{
					temp[p.first].push_back(p.second[j]);
				}
			}
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		InsertTriangles(&node_->pChildren[i], triangleSum[i]);
	}
}

void Octree::BuildChildNodes(Node* node_)
{
	float step = node_->halfWidth * 0.5f;
	glm::vec3 offset;
	node_->pChildren = new Node[8];

	for (int i = 0; i < 8; ++i)
	{
		offset.x = ((i & 1) ? step : -step);
		offset.y = ((i & 2) ? step : -step);
		offset.z = ((i & 4) ? step : -step);

		glm::vec3 childCenter = node_->center + offset;
		float childHalfWidth = step;

		node_->color = glm::vec3(0.f, 0.4f, 0.f);
		node_->pChildren[i].center = childCenter;
		node_->pChildren[i].halfWidth = childHalfWidth;
		node_->pChildren[i].aabb = new AABB(childCenter - childHalfWidth, childCenter + childHalfWidth);
		node_->pChildren[i].currDepth = node_->currDepth + 1;
	}
}

void Octree::IntersectTest(BoundingSphere* bs)
{
	if(node)
		DetectCollision_BroadPhase(bs, node);

	/*if (intersectedNode)
		bs->dir = glm::vec3(0.f);*/
}

void Octree::DrawTriangle_(Node* node_, Shader* shader)
{
	if (node_ == nullptr) return;

	if (node_->pChildren != nullptr)
	{
		for (int i = 0; i < 8; ++i)
		{
			DrawTriangle_(&node_->pChildren[i], shader);
		}
	}
	else
	{
		for (auto& m : node_->triangles)
		{
			shader->SetMat4fv(modelmatrix, "ModelMatrix");
			shader->SetVec3f(node_->color, "line_color");

			shader->Use();

			//Bind VAO
			glBindVertexArray(m.first->VAO);

			//Draw
			glDrawElements(GL_LINES, static_cast<GLsizei>(m.second.size()), GL_UNSIGNED_INT, nullptr);

			//CleanUp
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}
}

void Octree::InitVAO(Node* node_)
{
	if (node_ == nullptr) return;

	if (node_->pChildren != nullptr)
	{
		for (int i = 0; i < 8; ++i)
		{
			InitVAO(&node_->pChildren[i]);
		}
	}
	else
	{
		node_->color.x = ((rand() % 100) / 200.0f) + 0.4f;
		node_->color.y = ((rand() % 100) / 200.0f) + 0.4f;
		node_->color.z = ((rand() % 100) / 200.0f) + 0.4f;

		int size = static_cast<int>(node_->triangles.size());
		node_->EBO = new GLuint[size];

		int i = 0;

		for (auto& m : node_->triangles)
		{
			int nrOfIndices = static_cast<int>(m.second.size());
			int nrOfPoints = static_cast<int>(m.first->vertexArray.size());

			//Gen EBO and bind and send data
			if (nrOfIndices > 0)
			{
				glGenBuffers(1, &node_->EBO[i]);
				if(i<size) //idk but it gives a warning
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, node_->EBO[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, nrOfIndices * sizeof(GLuint), m.second.data(), GL_STATIC_DRAW);
			}

			++i;
		}
	}
}

void Octree::DetectCollision_MidPhase(BoundingSphere* bs, Node* node_)
{
	if (!bs->TestSphereVsAABB(*node_->aabb))
	{
		return;
	}

	if (node_->pChildren)
	{
		for (int i = 0; i < 8; ++i)
		{
			DetectCollision_MidPhase(bs, &node_->pChildren[i]);
		}
	}
	else
	{
		intersectedNode = node_;
	}
}

void Octree::DetectCollision_BroadPhase(BoundingSphere* bs, Node* node_)
{
	intersectedNode = nullptr;

	if (!bs->TestSphereVsAABB(*node_->aabb))
	{
		return;
	}

	for (int i = 0; i < 8; ++i)
	{
		DetectCollision_MidPhase(bs, &node_->pChildren[i]);
	}
}
