//
// Created by Göksu Güvendiren on 23/03/2017.
//

#pragma once

#include <vector>
#include <glm/vec3.hpp>

class Vertex
{
    glm::vec3 data;
    unsigned int id;

public:
    Vertex(glm::vec3 d, int i) : data(d), id(i){}

    Vertex& operator=(const Vertex& vert) = default;
    
    auto Data() const { return data; }
    auto ID() const { return id; }
    
    float EuclideanDistance(const Vertex& vert);
};
