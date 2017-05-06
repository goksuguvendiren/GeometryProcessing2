//
// Created by Göksu Güvendiren on 06/03/2017.
//

#pragma once

#include <map>
#include <vector>
#include "Triangle.h"

enum class DataStructure
{
    Array,
    MinHeap,
    FibHeap
};

struct BoundingBox
{
    glm::vec3 mins;
    glm::vec3 maxs;

    BoundingBox() : mins({0, 0, 0}), maxs({0, 0, 0}) {}
    BoundingBox(glm::vec3 ns, glm::vec3 xs) : mins(ns), maxs(xs) {}
};

class Mesh
{
    int id;
    unsigned int numVertices;
    unsigned int numFaces;

    std::vector<Triangle> faces;
    std::vector<Vertex> vertices;

    std::map<int, std::vector<int>> neighbors;

    void loadOFF(std::ifstream& stream);
    void loadOBJ(std::ifstream& stream);

    BoundingBox box;

public:
    Mesh(int mid = 1) : id(mid){}
    Mesh(const Mesh& m) = delete;
    Mesh(Mesh&& m) = default;

    void addNeighbor(int id, int neigh)
    {
        if (std::find(neighbors[id].begin(), neighbors[id].end(), neigh) == neighbors[id].end())
            neighbors[id].push_back(neigh);
    }
    void AddVertex(Vertex&& vert)
    {
        vertices.push_back(std::move(vert));
    }

    void AddFace(Triangle&& face)
    {
        faces.push_back(std::move(face));
    }

    std::vector<glm::vec3> SamplePoints(int sampleCount) const;
    float GetTotalArea() const;
    int NumVertices() { return numVertices; }

    int ID() const { return id; }
    const std::vector<Triangle>& Faces() const { return faces; }

    void LoadMesh(const std::string& path);

    const Vertex& GetVertex(unsigned int id) const { return vertices[id]; }
    const Triangle& GetFace(unsigned int id) const { return faces[id]; }

    std::vector<unsigned int> GetFaces() const
    {
        std::vector<unsigned int> data;
        data.reserve(numFaces * 3);

        std::for_each(faces.begin(), faces.end(), [&data](const Triangle& tri){
            data.push_back(tri.PointA().ID());
            data.push_back(tri.PointB().ID());
            data.push_back(tri.PointC().ID());
        });

        return data;
    }

    const BoundingBox& Box() const { return box; }
    std::vector<glm::vec3> GetVertexData() const
    {
        std::vector<glm::vec3> data;

        data.resize(numVertices);
        std::transform(vertices.begin(), vertices.end(), data.begin(), [](const Vertex& vertex){
            return vertex.Data();
        });

        return data;
    }

    void CreateBoundingBox();

    auto FindMin(const std::vector<std::pair<float, unsigned int>>& costs, const std::vector<bool>& beenProcessed) const;
    const std::vector<int>& GetNeighbors(unsigned int id) { return neighbors[id]; }

    void Translate(glm::vec3 translation);
};
