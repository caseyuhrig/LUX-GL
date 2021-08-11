#pragma once

#include <pch.hpp>

#include "lux/Types.hpp"
//#include "Color.hpp"
#include "lux/Renderer/VertexBuffer.hpp"
#include "lux/Renderer/VertexArray.hpp"
#include "lux/Renderer/IndexBuffer.hpp"
//#include "../Renderer/Renderer.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Color.hpp"
#include "lux/Log.hpp"



// CCW is GL front facing.

namespace lux {

    enum class CornerPosition {
        TopLeft = 1,
        BottomLeft = 2,
        TopRight = 3,
        BottomRight = 4
    };

    class Mesh
    {
    public:
        Mesh();
        //Mesh(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transformation = glm::mat4(1.0f));
        // TODO Remove transformation constructor.
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
            if (!m_Built)
            {
                CreateGeometry();
                PublishGeometry();
                m_Built = true;
            }
            else
                spdlog::warn("Build() has already been called...  SKIPPING");
        }
        const size_t GetVertexCount() const { return vertices.size(); }
        /*
        void Draw(const Ref<Shader>& shader) const
        {
            Renderer::Draw(GL_TRIANGLES, va, ib, shader);

            
               auto shaderVisualizeNormals = lux::Shader("res/shaders/visualize_normals.glsl");

               shaderVisualizeNormals.SetUniformMat4f("projection", camera.GetProjection());
               shaderVisualizeNormals.SetUniformMat4f("view", camera.GetView());
               shaderVisualizeNormals.SetUniformMat4f("model", model * transformation);
               mesh.Draw(renderer, shaderVisualizeNormals);
               
        }
        */
        void Load(const std::string& mesh_name, const std::string& file_path);
        // TODO Possibly remove transformation and color.  Entity vars!
        glm::mat4 GetTransformation() const { return transformation; }
        void SetTransformation(const glm::mat4& trans) { this->transformation = trans; }
        glm::vec4 GetColor() const { return color; }
        void SetColor(const glm::vec4& color) { this->color = color; }
        const Ref<VertexArray> GetVertexArray() const { return va; }
        const Ref<IndexBuffer> GetIndexBuffer() const { return ib; }

        void AddVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, const glm::vec3& normal);
        void AddIndex(uint32_t index);

    protected:
        glm::vec3 min, max; // mesh bounds
        glm::mat4 transformation;
        glm::vec4 color = glm::vec4(1);
        std::vector<float> vertices;
        std::vector<uint32_t> indicies;
        bool m_Built = false;

        const uint32_t GetVerticesSize() const { return static_cast<uint32_t>(vertices.size()); }
        const uint32_t GetIndiciesSize() const { return static_cast<uint32_t>(indicies.size()); }

        void AddIndex(uint32_t i1, uint32_t i2, uint32_t i3);
        
        /**
         * Adds a triangle to the mesh and updates the verticies accordingly.  Vertices are expected
         * to be in a front facing CCW rotation.
         * @param p1 The first point of the traingle.
         * TODO Rename p1,p2,p3 to a,b,c
         * TODO Remove color, need to find another way to attach color data without storing in the mesh object.
         */
        void AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& normal, const glm::vec4& color);
        // Adds a triangle and calculates the normal automatically.
        void AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
        //void AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec4& color);
        void AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4,
                     const glm::vec3& normal, const glm::vec4& color);
        void AddPentagon(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, 
                         const glm::vec3& v4, const glm::vec3& v5, const float length);
        void AddEllipse(const glm::vec2& center, const glm::vec2& radius, 
                        const float startAngle, const float endAngle, const float step,
                        const glm::vec3& normal, const glm::vec4& color);

        void AddHorzBar(const glm::vec2& min, const glm::vec2& max, const glm::vec3& normal, const glm::vec4& color);

        void AddRoundedCorner(const CornerPosition& position, const glm::vec2& min, const glm::vec2& max, 
                              const glm::vec3& normal, const glm::vec4& color);

        void AddScoopedCorner(const CornerPosition& position, const glm::vec2& min, const glm::vec2& max,
                              const glm::vec3& normal, const glm::vec4& color);

        void AddTopCar(const glm::vec2& min, const glm::vec2& max,
            float radius, float scoopRadius,
            float sideWidth, float horzHeight,
            const glm::vec3& normal, const glm::vec4& color);

        void AddBottomCar(const glm::vec2& min, const glm::vec2& max,
            float radius, float scoopRadius,
            float sideWidth, float horzHeight,
            const glm::vec3& normal, const glm::vec4& color);
    private:
        Ref<VertexArray> va;
        Ref<VertexBuffer> vb;
        Ref<VertexBufferLayout> layout;
        Ref<IndexBuffer> ib;      
    };
}