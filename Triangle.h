//
// Created by Göksu Güvendiren on 27/02/2017.
//

#pragma once

#include <glm/glm.hpp>
#include "Vertex.h"
#include <random>

class Material;
class Vertex;
class Mesh;

class Triangle
{
    Vertex pointA;
    Vertex pointB;
    Vertex pointC;

    glm::vec3 surfNormal;
    int id;

public:
    Triangle() = default;
    Triangle(Vertex a, Vertex b, Vertex c, int tid = 1);
    Triangle(unsigned int a, unsigned int b, unsigned int c, const Mesh& mesh, int tid = 1);

    ~Triangle();

    int ID() const;

    auto Normal() const { return surfNormal; }
    float Area() const;

    Vertex& PointA() { return pointA; }
    Vertex& PointB() { return pointB; }
    Vertex& PointC() { return pointC; }

    Vertex PointA() const { return pointA; }
    Vertex PointB() const { return pointB; }
    Vertex PointC() const { return pointC; }

    glm::vec3 GetRandomPoint() const;
};