#pragma once

#include "glm/glm.hpp"
#include "Core.hpp"
//#include "Color.hpp"
#include "../VertexBuffer.hpp"
#include "../IndexBuffer.hpp"
#include "../Renderer.hpp"
#include "../Shader.hpp"


// CCW is GL front facing.

namespace ux {

    class Mesh
    {
    public:
        Mesh();
        // TODO Remove transformation from constructor.
        Mesh(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transformation = glm::mat4(1.0f));
        // TODO Research what ~Dest() = default does!
        ~Mesh() = default;
        virtual void CreateGeometry() {};
        // Transform the geometry (vertex data) after creation.
        void TransformGeometry(const glm::mat4& transformation);
        // Append the geometry from the supplied mesh into *this mesh.
        // Transformation "if supplied" is done on the incoming mesh to append.
        void AppendGeometry(const Mesh& mesh, const glm::mat4& transformation = glm::mat4(1.0f));
        // Send all the magic over to the graphics card.
        void PublishGeometry() const;
        void Build()
        {
            CreateGeometry();
            PublishGeometry();
        }
        inline size_t GetVertexCount() { return vertices.size(); }
        void Draw(const Renderer& renderer, const Shader& shader) const
        {
            //if (!built) Build();
            renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
        }
        void Load(const std::string& mesh_name, const std::string& file_path);
        // TODO Possibly remove transformation and color.  Entity vars!
        inline glm::mat4 GetTransformation() { return transformation; }
        inline void SetTransformation(const glm::mat4& trans) { this->transformation = trans; }
        inline glm::vec4 GetColor() { return color; }
        inline void SetColor(const glm::vec4& color) { this->color = color; }
    protected:
        glm::vec3 min, max; // mesh bounds
        glm::mat4 transformation;
        glm::vec4 color;
        std::vector<float> vertices;
        std::vector<unsigned int> indicies;
        void AddIndex(unsigned int i1, unsigned int i2, unsigned int i3);
        void AddVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, const glm::vec3& normal);
        /**
         * Adds a triangle to the mesh and updates the verticies accordingly.  Vertices are expected
         * to be in a front facing CCW rotation.
         * @param p1 The first point of the traingle.
         * TODO Rename p1,p2,p3 to a,b,c
         * TODO Remove color, need to find another way to attach color data without storing in the mesh object.
         */
        void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& normal, const glm::vec4 color);
        // Adds a triangle and calculates the normal automatically.
        //void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const bool smooth, const glm::vec4 color);
        //void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
    private:
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;
    };
}