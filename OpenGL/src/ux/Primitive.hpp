#pragma once

#include "Core.hpp"
#include "Color.hpp"
#include "../Renderer.hpp"


// CCW is GL front facing.
// what is the term when you manipulate vectors and matrixes together
// tweening? trimming croping



namespace ux {

    class Primitive
    {
    private:

        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
        // LINES
        //Ref<VertexArray> vaLines;
        //Ref<VertexBuffer> vbLines;
        //Ref<VertexBufferLayout> layoutLines;
        //Ref<IndexBuffer> ibLines;

        glm::vec4 color;
        glm::mat4 transformation;

    protected:

        glm::vec3 min;
        glm::vec3 max;
        std::vector<float> vertices;
        std::vector<unsigned int> indicies;
          
        void _AddIndex(std::vector<unsigned int>& list, unsigned int i1, unsigned int i2, unsigned int i3) const
        {
            list.push_back(i1);
            list.push_back(i2);
            list.push_back(i3);
        }

        void _AddVertex(
            std::vector<float>& list,
            const glm::vec3& position,
            const glm::vec4& color,
            const glm::vec2& texCoord,
            const glm::vec3& normal) const
        {
            // position
            list.push_back(position.x);
            list.push_back(position.y);
            list.push_back(position.z);
            // color
            list.push_back(color.r);
            list.push_back(color.g);
            list.push_back(color.b);
            list.push_back(color.a);
            // texture coordinate
            list.push_back(texCoord.x);
            list.push_back(texCoord.y);
            // normal
            list.push_back(normal.x);
            list.push_back(normal.y);
            list.push_back(normal.z);
        }

    public:

        inline Primitive(const glm::vec3& min, const glm::vec3& max) 
            : min(min), max(max), color(1), transformation(0),
            va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()), 
            ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}

        //inline ~Primitive() {}

        //void transform(glm::mat4& transformation);

        inline virtual void CreateGeometry() = 0;
        //inline virtual void TransformGeometry(const glm::mat4& transformation) = 0;
        //inline virtual void AppendGeometry(const Primitive& primitive, const glm::mat4& transformation = glm::mat4(0)) = 0;

        inline void PublishGeometry() const
        {
            vb->Init(&vertices[0], vertices.size() * sizeof(float));
            layout->Push<float>(3); // position xyz
            layout->Push<float>(4); // color rgba
            layout->Push<float>(2); // texture coordinates xy
            layout->Push<float>(3); // normal xyz
            va->AddBuffer(*vb, *layout);
            ib->Init(&indicies[0], indicies.size() - 1);
        }

        inline void Build() // CreateAndPublish() BuildGeometry()
        {
            CreateGeometry();
            PublishGeometry();
        }

        inline glm::mat4& GetTransformation() { return transformation; }
        inline glm::vec4& GetColor() { return color; }
        inline void SetColor(const glm::vec4& color) { this->color = color; }
    
        inline void Draw(const Renderer& renderer, const Shader& shader) const
        {
            renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
        }

        //inline void DrawOutline(const Renderer& renderer, const Shader& shader) const
        //{
        //    renderer.Draw(GL_LINES, *vaLines, *ibLines, shader);
        //}

     
        /*
        inline void Load(const std::string& filepath)
        {
            objl::Loader loader;
            bool loaded = loader.LoadFile(filepath);

            if (!loaded)
            {
                std::cout << "[ERROR] Failed to load " << filepath << std::endl;
            }
            if (loaded)
            {
                std::cout << "LOADED: " << filepath << std::endl;

                for (int i = 0; i < loader.LoadedMeshes.size(); i++)
                {
                    objl::Mesh curMesh = loader.LoadedMeshes[i];

                    // Print Mesh Name
                    std::cout << "Mesh " << i << ": " << curMesh.MeshName << "\n";

                    // Print Vertices
                    std::cout << "Vertices:\n";

                    // Go through each vertex and print its number,
                    //  position, normal, and texture coordinate
                    for (int j = 0; j < curMesh.Vertices.size(); j++)
                    {
                        std::cout << "V" << j << ": " <<
                            "P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
                            "N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
                            "TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
                    }

                    // Print Indices
                    std::cout << "Indices:\n";

                    // Go through every 3rd index and print the
                    //	triangle that these indices represent
                    for (int j = 0; j < curMesh.Indices.size(); j += 3)
                    {
                        std::cout << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
                    }


                }
            }
        }
        */
    };

}