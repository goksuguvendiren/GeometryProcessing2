//
// Created by Göksu Güvendiren on 23/03/2017.
//

#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
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
        tickets.insert(std::make_pair(beginning + ticketCount, face.ID()));
        beginning += ticketCount;
    }

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

void Mesh::CreateBoundingBox()
{
    glm::vec3 minimums = {1000, 1000, 1000};
    glm::vec3 maximums = {-1000, -1000, -1000};
    for (auto& vertex : vertices)
    {
        minimums = glm::min(minimums, vertex.Data());
        maximums = glm::max(maximums, vertex.Data());
    }

    box = BoundingBox(minimums, maximums);
}

void Mesh::Translate(glm::vec3 translation)
{
    auto matrix = glm::translate(glm::mat4(1.), translation);
    for (int i = 0; i < vertices.size(); i++) {
        auto vert = glm::vec4(vertices[i].Data(), 1);

        vert = matrix * vert;
        vertices[i] = Vertex({vert.x, vert.y, vert.z}, vertices[i].ID());
    }

    for (int i = 0; i < faces.size(); i++)
    {
        auto vertA = glm::vec4(faces[i].PointA().Data(), 1);
        auto vertB = glm::vec4(faces[i].PointB().Data(), 1);
        auto vertC = glm::vec4(faces[i].PointC().Data(), 1);

        vertA = matrix * vertA;
        vertB = matrix * vertB;
        vertC = matrix * vertC;

        Vertex a = Vertex({vertA.x, vertA.y, vertA.z}, faces[i].PointA().ID());
        Vertex b = Vertex({vertB.x, vertB.y, vertB.z}, faces[i].PointB().ID());
        Vertex c = Vertex({vertC.x, vertC.y, vertC.z}, faces[i].PointC().ID());

        faces[i].PointA() = a;
        faces[i].PointB() = b;
        faces[i].PointC() = c;
    }
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
