//
// Created by Göksu Güvendiren on 27/02/2017.
//

#include "Triangle.h"
#include "Mesh.h"

Triangle::Triangle(Vertex a, Vertex b, Vertex c, int tid) : pointA(a),
                                                            pointB(b),
                                                            pointC(c),
                                                            id(tid)
{
    surfNormal = glm::normalize(glm::cross(pointB.Data() - pointA.Data(),
                                           pointC.Data() - pointA.Data()));
}

int Triangle::ID() const
{
    return id;
}


Triangle::Triangle(unsigned int a, unsigned int b, unsigned int c, const Mesh& mesh, int tid) : pointA(mesh.GetVertex(a)),
                                                                                                pointB(mesh.GetVertex(b)),
                                                                                                pointC(mesh.GetVertex(c)),
                                                                                                id(tid) {}
Triangle::~Triangle() {}

float Triangle::Area() const
{
    return glm::length(glm::cross(pointB.Data() - pointA.Data(), pointC.Data() - pointA.Data())) / 2.f;
}

std::random_device rd;
std::mt19937 generator(rd());
std::uniform_real_distribution<float> dis(0.f, 1.f);

glm::vec3 Triangle::GetRandomPoint() const
{
    auto alpha = dis(generator);
    auto beta  = dis(generator) * (1 - alpha);

    return pointA.Data() + (pointB.Data() - pointA.Data()) * alpha + (pointC.Data() - pointA.Data()) * beta;
}
