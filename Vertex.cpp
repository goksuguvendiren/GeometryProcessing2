//
// Created by Göksu Güvendiren on 23/03/2017.
//

#include <vector>
#include <sstream>
#include <cmath>
#include "Vertex.h"

float Vertex::EuclideanDistance(const Vertex &vert)
{
    return std::sqrt(std::pow(data.x - vert.data.x, 2) +
                     std::pow(data.y - vert.data.y, 2) +
                     std::pow(data.z - vert.data.z, 2));
}

