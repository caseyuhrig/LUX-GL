#include "Mesh.hpp"
//#include "Color.hpp"
//#include "../Renderer.hpp"
#include "OBJ_Loader.h"


// CCW is GL front facing.
// what is the term when you manipulate vectors and matrixes together



namespace ux {

    Mesh::Mesh()
        : min(0), max(0), color(1), transformation(0),
        va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()),
        ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}

    Mesh::Mesh(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transformation)
        : min(min), max(max), color(1), transformation(transformation),
        va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()),
        ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}

    void Mesh::AddIndex(unsigned int i1, unsigned int i2, unsigned int i3)
    {
        indicies.push_back(i1);
        indicies.push_back(i2);
        indicies.push_back(i3);
    }

    void Mesh::AddVertex(
        const glm::vec3& position,
        const glm::vec4& color,
        const glm::vec2& texCoord,
        const glm::vec3& normal)
    {
        // position
        vertices.push_back(position.x);
        vertices.push_back(position.y);
        vertices.push_back(position.z);
        // normal
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
        // color
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(color.a);
        // texture coordinate
        vertices.push_back(texCoord.x);
        vertices.push_back(texCoord.y);
    }

    /**
     * Adds a triangle to the mesh and updates the verticies accordingly.  Points are expected
     * to be in a front facing CCW rotation.
     * @param p1 The first point of the traingle.
     */
    void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& normal, const glm::vec4 color)
    {
        AddVertex(p1, color, glm::vec2(0.0f), normal);
        AddVertex(p2, color, glm::vec2(0.0f), normal);
        AddVertex(p3, color, glm::vec2(0.0f), normal);
        indicies.push_back(indicies.size());
        indicies.push_back(indicies.size());
        indicies.push_back(indicies.size());
    }


    void Mesh::AppendGeometry(const Mesh& mesh, const glm::mat4& transformation)
    {
        // TODO finish AppendGeometry.
    }


    void Mesh::TransformGeometry(const glm::mat4& transformation)
    {
        // TODO finish TransformGeometry.
    }


    void Mesh::PublishGeometry() const
    {
        vb->Init(&vertices[0], vertices.size() * sizeof(float));
        layout->Push<float>(3); // position xyz,    location = 0
        layout->Push<float>(3); // normal xyz,      location = 1
        layout->Push<float>(4); // color rgba
        layout->Push<float>(2); // texture coordinates xy
        va->AddBuffer(*vb, *layout);
        ib->Init(&indicies[0], indicies.size());
    }


    void Mesh::Load(const std::string& mesh_name, const std::string& file_path)
    {
        objl::Loader loader;
        bool loaded = loader.LoadFile(file_path);

        if (!loaded)
        {
            std::cout << "[ERROR] Failed to load " << file_path << std::endl;
        }
        if (loaded)
        {
            std::cout << "READING: " << file_path << std::endl;
            for (int i = 0; i < loader.LoadedMeshes.size(); i++)
            {
                objl::Mesh curMesh = loader.LoadedMeshes[i];
                std::cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";
                if (mesh_name == curMesh.MeshName)
                {
                    std::cout << "LOADING: " << mesh_name << std::endl;
                    for (int j = 0; j < curMesh.Vertices.size(); j++)
                    {
                        /*
                        std::cout << "V" << j << ": " <<
                            "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                            "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                            "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
*/
                        glm::vec3 position = { curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z };
                        glm::vec2 texCoord = { curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y };
                        glm::vec3 normal = { curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z };

                        // z = up
                        // y = depth
                        //float save_z = normal.z;
                        //normal.z = normal.y;
                        //normal.y = save_z;
                        //normal.z = -normal.z;
                        //normal.y = -normal.y;
                        //glm::new_normal = glm::computeNormal();
                        AddVertex(position, glm::vec4(1), texCoord, normal);
                    }

                    std::cout << "Indices:\n";

                    // Go through every 3rd index and print the
                    //	triangle that these indices represent
                    for (int j = 0; j < curMesh.Indices.size(); j += 3)
                    {
                        //std::cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";

                        unsigned int index1 = curMesh.Indices[j];
                        unsigned int index2 = curMesh.Indices[j + 1];
                        unsigned int index3 = curMesh.Indices[j + 2];

                        //std::cout << "T" << j / 3 << ": " << index1 << ", " << index2 << ", " << index3 << "\n";

                        AddIndex(index1, index2, index3);
                    }
                }

            }
        }
    }

}