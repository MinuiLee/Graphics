/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Point3D.h
Purpose: Point3D primitive class cpp file
Language: C++
Platform: Visual Studio 2019
Project: minui.lee_CS300_1
Author: Minui Lee, minui.lee, 180002020
Creation date: 10 Feb 2021
End Header --------------------------------------------------------*/

#include "Point3D.h"
#include "BoundingSphere.h"
#include "AABB.h"
#include "Plane.h"
#include "Triangle.h"

void Point3D::Draw(Shader* shader, glm::vec3 color)
{
    shader->SetMat4fv(modelmatrix, "ModelMatrix");
    shader->SetVec3f(color, "line_color");

    shader->Use();

    //Bind VAO
    glBindVertexArray(VAO);

    //Draw
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));

    //CleanUp
    glBindVertexArray(0);
    glUseProgram(0);
}

bool Point3D::PointVsSphere(const BoundingSphere& bc) const
{
    return glm::distance(coord, bc.center) <= bc.radius;
}

bool Point3D::PointVsAABB(const AABB& aabb) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (aabb.min[i] > coord[i] || coord[i] > aabb.max[i])
        {
            return false;
        }
    }

    return true;
}

bool Point3D::PointVsTriangle(const Triangle& triangle) const
{
    //on plane?
    glm::vec3 nor = glm::cross(triangle.v2 - triangle.v1, triangle.v3 - triangle.v1);
    float dis = glm::dot(nor, triangle.v1);

    if (glm::dot(nor, coord) != dis) return false;

    // inside a triangle?
    glm::vec3 u = triangle.v2 - triangle.v1;
    glm::vec3 v = triangle.v3 - triangle.v1;
    glm::vec3 n = glm::cross(u, v);

    glm::vec3 I = coord;

    float uu = glm::dot(u, u);
    float uv = glm::dot(u, v);
    float vv = glm::dot(v, v);
    glm::vec3 w = I - triangle.v1;
    float wu = glm::dot(w, u);
    float wv = glm::dot(w, v);
    float det = uv * uv - uu * vv;

    float s, t;
    s = (uv * wv - vv * wu) / det;
    if (s < 0.0 || s > 1.0)
        return false;

    t = (uv * wu - uu * wv) / det;
    if (t < 0.0 || (s + t > 1.f))
        return false;

    return true;
}

bool Point3D::PointVsPlane(const Plane& plane) const
{
    return glm::dot(plane.normal, coord) == plane.distance;
}

void Point3D::InitVAO()
{
    //Create VAO
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Gen VBO and bind and send data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, nrOfPoints * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    //Set vertex attribute pointers and enable
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    //Bind VAO 0
    glBindVertexArray(0);
}

void Point3D::Initialize(glm::vec3 coord_)
{
    coord = coord_;

    points.clear();
    points.push_back(coord);

    nrOfPoints = static_cast<unsigned int>(points.size());
    InitVAO();
}
