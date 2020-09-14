#pragma once

#include "glm/glm.hpp"
#include "../Types.hpp"
//#include "Color.hpp"
#include "../Renderer/VertexBuffer.hpp"
#include "../Renderer/IndexBuffer.hpp"
#include "../Renderer/Renderer.hpp"
#include "../Renderer/Shader.hpp"


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
                UX_LOG_INFO("Build() has already been called...  SKIPPING");
        }
        inline size_t GetVertexCount() { return vertices.size(); }
        void Draw(const Renderer& renderer, const Shader& shader) const
        {
            //if (!built) Build();
            renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
        }
        void Load(const std::string& mesh_name, const std::string& file_path);
        // TODO Possibly remove transformation and color.  Entity vars!
        inline glm::mat4 GetTransformation() const { return transformation; }
        inline void SetTransformation(const glm::mat4& trans) { this->transformation = trans; }
        inline glm::vec4 GetColor() const { return color; }
        inline void SetColor(const glm::vec4& color) { this->color = color; }
    protected:
        glm::vec3 min, max; // mesh bounds
        glm::mat4 transformation;
        glm::vec4 color = glm::vec4(1);
        std::vector<float> vertices;
        std::vector<unsigned int> indicies;
        bool m_Built = false;
        void AddIndex(unsigned int i1, unsigned int i2, unsigned int i3);
        void AddVertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, const glm::vec3& normal);
        /**
         * Adds a triangle to the mesh and updates the verticies accordingly.  Vertices are expected
         * to be in a front facing CCW rotation.
         * @param p1 The first point of the traingle.
         * TODO Rename p1,p2,p3 to a,b,c
         * TODO Remove color, need to find another way to attach color data without storing in the mesh object.
         */
        void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& normal, const glm::vec4& color);
        // Adds a triangle and calculates the normal automatically.
        //void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const bool smooth, const glm::vec4 color);
        //void AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
        void AddQuad(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4,
                     const glm::vec3& normal, const glm::vec4& color);

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