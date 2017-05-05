//
// Created by Göksu Güvendiren on 23/03/2017.
//

#include <iostream>
#include <fstream>
#include "Mesh.h"

std::vector<glm::vec3> Mesh::SamplePoints(int sampleCount) const
{
    std::map<int, int> tickets;

    auto totalArea = GetTotalArea();
    float ticketSize = 0.001f;

    int beginning = 0;
    for (auto& face : faces)
    {
        auto ticketCount = int(face.Area() / ticketSize);
//        std::cerr << ticketCount << '\n';
        tickets.insert(std::make_pair(beginning + ticketCount, face.ID()));
        beginning += ticketCount;
    }

    // here, beginning is the total number of tickets.

    std::mt19937 generator(123);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    std::vector<glm::vec3> pointCloud(100);
    for (int i = 0; i < 100; i++)
    {
        double random_val = dis(generator) * beginning;
        auto random_ticket = tickets.upper_bound(random_val);

        auto& face = GetFace(random_ticket->second);
        glm::vec3 sample_point = face.GetRandomPoint();
        pointCloud[i] = sample_point;
    }

    return pointCloud;
}

float Mesh::GetTotalArea() const
{
    float area = 0.f;
    for (auto& face : faces)
    {
        area += face.Area();
    }

    return area;
}

void Mesh::LoadMesh(const std::string& path)
{
    std::ifstream stream;
    stream.open(path);

    std::string filetype; stream >> filetype;

    std::cerr << filetype << '\n';

    if (filetype == "OFF"){
        loadOFF(stream);
    }
    else if (filetype == "# OBJ"){
        loadOBJ(stream);
    }
    else
    {
        std::cerr << "Could not load mesh...\n";
        abort();
    }
}

void Mesh::loadOFF(std::ifstream& stream)
{
    int nullval;
    stream >> numVertices; stream >> numFaces; stream >> nullval;

    vertices.reserve(numVertices);
    faces.reserve(numFaces);

    for (int i = 0; i < numVertices; i++){
        float x, y, z;
        stream >> x; stream >> y; stream >> z;
        vertices.push_back(Vertex({x, y, z}, i));
    }

    for (int i = 0; i < numFaces; i++){
        int numVert; stream >> numVert;
        assert(numVert == 3);

        int a, b, c;
        stream >> a; stream >> b; stream >> c;

        addNeighbor(a, b);
        addNeighbor(a, c);
        
        addNeighbor(b, a);
        addNeighbor(b, c);
        
        addNeighbor(c, a);
        addNeighbor(c, b);
        
        faces.push_back(Triangle(vertices[a], vertices[b], vertices[c], i));
    }
}

void Mesh::loadOBJ(std::ifstream& stream)
{
    std::cerr << "loading obj " << '\n';
}

auto Mesh::FindMin(const std::vector<std::pair<float, unsigned int>>& costs, const std::vector<bool>& beenProcessed) const
{
    auto min = std::numeric_limits<unsigned int>::max();
    auto vert = GetVertex(0);

    for (int i = 0; i < costs.size(); i++){
        if (!beenProcessed[i] && costs[i].first < min){
            min = costs[i].first;
            vert = GetVertex(i);
        }
    }

    return vert;
}
