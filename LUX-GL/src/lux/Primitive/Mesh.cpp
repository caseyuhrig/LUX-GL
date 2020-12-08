#include "Mesh.hpp"

#include <glm/gtx/normal.hpp>
#include <glm/gtx/string_cast.hpp>

#include "lux/Color.hpp"
#include "lux/Log.hpp"
#include "OBJ_Loader.h"


// CCW is GL front facing.
// what is the term when you manipulate vectors and matrixes together



namespace lux {

    Mesh::Mesh()
        : min(0), max(0), color(1), transformation(0),
        va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()),
        ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}
    /*
    Mesh::Mesh(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max), color(1.0f), transformation(0.0f),
        va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()),
        ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}
    */
    Mesh::Mesh(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transformation)
        : min(min), max(max), color(1), transformation(transformation),
        va(CreateRef<VertexArray>()), vb(CreateRef<VertexBuffer>()),
        ib(CreateRef<IndexBuffer>()), layout(CreateRef<VertexBufferLayout>()) {}

    void Mesh::AddIndex(uint32_t index)
    {
        indicies.push_back(index);
    }

    void Mesh::AddIndex(uint32_t i1, uint32_t i2, uint32_t i3)
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
    void Mesh::AddTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& normal, const glm::vec4& color)
    {
        AddVertex(p1, color, glm::vec2(0.0f), normal);
        AddVertex(p2, color, glm::vec2(0.0f), normal);
        AddVertex(p3, color, glm::vec2(0.0f), normal);
        indicies.push_back(GetIndiciesSize());
        indicies.push_back(GetIndiciesSize());
        indicies.push_back(GetIndiciesSize());
    }

    void Mesh::AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
    {
        AddTriangle(v1, v2, v3, glm::triangleNormal(v1, v2, v3), color);
    }
    /*
    void Mesh::AddTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec4& color)
    {
        AddTriangle(v1, v2, v3, glm::triangleNormal(v1, v2, v3), color);
    }
    */
    void Mesh::AddQuad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4,
        const glm::vec3& normal, const glm::vec4& color)
    {
        AddTriangle(v1, v2, v3, normal, color);
        AddTriangle(v3, v4, v1, normal, color);
    }

    void Mesh::AddPentagon(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, 
        const glm::vec3& v4, const glm::vec3& v5, const float length)
    {
        const auto center = (v1 + v2 + v3 + v4 + v5) / 5.0f;
        /*
        std::cout << "v1 " << glm::to_string(v1) << std::endl;
        std::cout << "v2 " << glm::to_string(v2) << std::endl;
        std::cout << "v3 " << glm::to_string(v3) << std::endl;
        std::cout << "v4 " << glm::to_string(v4) << std::endl;
        std::cout << "v5 " << glm::to_string(v5) << std::endl;
        std::cout << "cen " << glm::to_string(center) << std::endl;
        */
        /*
        AddTriangle(v1, v2, center, lux::Colors::AMBER);
        AddTriangle(v2, v3, center, lux::Colors::MEDIUM_SPRING_GREEN);
        AddTriangle(v3, v4, center, lux::Colors::ORANGE);
        AddTriangle(v4, v5, center, lux::Colors::STEEL_BLUE);
        AddTriangle(v5, v1, center, lux::Colors::DEEP_PINK);
        */
        //auto n4 = glm::vec3(v4.x, v4.y, v4.z);
        glm::vec3 normal = glm::normalize(center);
        //glm::triangleNormal(v1, v2, center)
        AddTriangle(v1, v2, center, normal, lux::Colors::Amber);
        AddTriangle(v2, v3, center, normal, lux::Colors::MediumSpringGreen);
        AddTriangle(v3, v4, center, normal, lux::Colors::Orange);
        AddTriangle(v4, v5, center, normal, lux::Colors::SteelBlue);
        AddTriangle(v5, v1, center, normal, lux::Colors::DeepPink);
    }

    void Mesh::AddEllipse(const glm::vec2& center, const glm::vec2& radius, 
                          const float startAngle, const float endAngle, const float step, 
                          const glm::vec3& normal, const glm::vec4& color)
    {
        for (float angle = startAngle; angle < endAngle; angle += step)
        {
            glm::vec3 a = { 
                center.x + radius.x * cosf(angle * TO_RADf),
                center.y + radius.y * sinf(angle * TO_RADf),
                0.0f
            };
            glm::vec3 b = {
                center.x + radius.x * cosf((angle + step) * TO_RADf),
                center.y + radius.y * sinf((angle + step) * TO_RADf),
                0.0f
            };
            AddTriangle(glm::vec3(center, 0.0f), a, b, normal, color);
        }
    }

    // Rectangle with a rounded right end.
    void Mesh::AddHorzBar(const glm::vec2& min, const glm::vec2& max, const glm::vec3& normal, const glm::vec4& color)
    {
        /*
        //glm::vec4 color = lux::COLOR_AMBER; //GetColor();

        glm::vec2 texCoord = { 0.0f, 0.0f };
        //glm::vec3 normal = { 0.0f, 0.0f, -1.0f };

        glm::vec3 center = { (min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, 0.0f };

        AddVertex(center, color, texCoord, normal); // 0
        AddVertex({ center.x, max.y, 0.0f }, color, texCoord, normal); // 1
        AddVertex({ min.x, max.y, 0.0f }, color, texCoord, normal); // 2
        AddVertex({ min.x, min.y, 0.0f }, color, texCoord, normal); // 3
        AddVertex({ center.x, min.y, 0.0f }, color, texCoord, normal); // 4

        AddIndex(1, 2, 3);
        AddIndex(3, 4, 1);

        int arc_steps = 8;
        float angle_step = 180.0f / (float)arc_steps;
        float arc_radius = center.y - min.y;
        size_t index = 4;

        float angle_start = -90.0 + angle_step;
        float angle_end = angle_start + 180.0;
        // draw the arc from <center.x, min.y> to <center.x, max.y>
        for (float angle = angle_start;angle < angle_end;angle += angle_step)
        {
            float x = center.x + arc_radius * cos(angle * TO_RAD);
            float y = center.y + arc_radius * sin(angle * TO_RAD);
            AddVertex({ x, y, 0.0f }, color, texCoord, normal);
            AddIndex(0, index, index + 1);
            index++;
        }

        AddIndex(0, index, 1);
        */
        float radius = (max.y - min.y) / 2.0f;
        glm::vec3 p0 = { min.x, min.y, 0.0f };
        glm::vec3 p1 = { max.x - radius, p0.y, 0.0f };
        glm::vec3 p2 = { p1.x, max.y, 0.0f };
        glm::vec3 p3 = { p0.x, p2.y, 0.0f };
        glm::vec2 center = { p1.x, p1.y + radius };
        AddQuad(p0, p1, p2, p3, normal, color);
        AddEllipse(center, { radius, radius }, 270, 270 + 180, 10, normal, color);
    }

    void Mesh::AddRoundedCorner(const CornerPosition& position, const glm::vec2& min, const glm::vec2& max, const glm::vec3& normal, const glm::vec4& color)
    {
        // top left on the xy plane
            //glm::vec3 v0 = { min.x, min.y + max.y, 0.0f };
        glm::vec3 center = { 0.0f, 0.0f, 0.0f }; // = { min.x + max.x, min.y, 0.0f };
        //glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
        float start_angle = 0.0f;
        float end_angle = 0.0f;
        switch (position)
        {
        case CornerPosition::TopRight:
            // bottom left is the center point
            center = { min.x, min.y, 0.0f };
            start_angle = 0.0f;
            end_angle = 90.0f;
            break;
        case CornerPosition::BottomLeft:
            // bottom left is the center point
            center = { max.x, max.y, 0.0f };
            start_angle = 180.0f;
            end_angle = 270.0f;
            break;
        case CornerPosition::TopLeft:
            // bottom left is the center point
            center = { max.x, min.y, 0.0f };
            start_angle = 90.0f;
            end_angle = 180.0f;
            break;
        default:
            UX_LOG_ERROR("Corner Position %d Invalid.", position);
            break;
        }
        // draw an arc from the bottom left to the top right.
        // the angle goes 90 deg starting at 90 degrees.
        float steps = 8.0f;
        float step = 90.0f / steps;
        float width = max.x - min.x;
        float height = max.y - min.y;
        glm::vec3 v1 = {
            center.x + width * std::cosf(start_angle * TO_RADf),
            center.y + height * std::sinf(start_angle * TO_RADf),
            0.0f
        };
        for (float angle = start_angle;angle < end_angle;angle += step)
        {
            glm::vec3 v2 = {
                center.x + width * std::cosf((angle + step) * TO_RADf),
                center.y + height * std::sinf((angle + step) * TO_RADf),
                0.0f
            };
            AddTriangle(center, v1, v2, normal, color);
            v1 = v2;
        }
    }

    void Mesh::AddScoopedCorner(const CornerPosition& position, const glm::vec2& min, const glm::vec2& max, const glm::vec3& normal, const glm::vec4& color)
    {
        glm::vec3 center = glm::vec3(0.0f);
        float start_angle = 90.0f;
        float end_angle = start_angle + 90.0f;
        glm::vec3 v0 = glm::vec3(0.0f);

        switch (position)
        {
        case CornerPosition::TopRight:
            center = { max.x, max.y, 0.0f };
            v0 = { min.x, min.y, 0.0f };
            start_angle = 180.0f;
            end_angle = 270.0f;
            break;
        case CornerPosition::BottomRight:
            center = { max.x, min.y, 0.0f };
            v0 = { min.x, max.y, 0.0f };
            start_angle = 90.0f;
            end_angle = 180.0f;
            break;
        default:
            UX_LOG_ERROR("Scooped Corner Position %d Invalid.", position);
            break;
        }        
        //glm::vec3 normal = { 0.0f, 0.0f, -1.0f };
        // draw an arc from the bottom left to the top right.
        // the angle goes 90 deg starting at 90 degrees.
        float steps = 8.0f;
        float step = 90.0f / steps;
        
        float width = max.x - min.x;
        float height = max.y - min.y;
        glm::vec3 v1 = {
            center.x + width * std::cosf(start_angle * TO_RADf),
            center.y + height * std::sinf(start_angle * TO_RADf),
            0.0f
        };
        for (float angle = start_angle;angle < end_angle;angle += step)
        {
            glm::vec3 v2 = {
                center.x + width * std::cosf((angle + step) * TO_RADf),
                center.y + height * std::sinf((angle + step) * TO_RADf),
                0.0f
            };
            AddTriangle(v0, v1, v2, normal, color);
            v1 = v2;
        }
    }

    void Mesh::AddTopCar(const glm::vec2& min, const glm::vec2& max,
        float radius, float scoopRadius,
        float sideWidth, float horzHeight,
        const glm::vec3& normal, const glm::vec4& color)
    {
        glm::vec3 v0 = { min.x + radius, max.y - radius, 0.0f };
        glm::vec3 v1 = { min.x, v0.y, 0.0f };
        glm::vec3 v2 = { min.x, min.y, 0.0f };
        glm::vec3 v3 = { v0.x, v2.y, 0.0f };
        glm::vec3 v4 = { min.x + sideWidth, v2.y, 0.0f };
        glm::vec3 v5 = { v4.x, max.y - horzHeight, 0.0f };
        glm::vec3 v6 = { max.x, v5.y, 0.0f };
        glm::vec3 v7 = { v6.x, max.y, 0.0f };
        glm::vec3 v8 = { v4.x, v7.y, 0.0f };
        glm::vec3 v9 = { v0.x, v7.y, 0.0f };
        glm::vec3 v10 = { v4.x, v5.y - scoopRadius, 0.0f };
        glm::vec3 v11 = { v10.x + scoopRadius, v10.y, 0.0f };
        glm::vec3 v12 = { v11.x, v5.y, 0.0f };
        AddQuad(v0, v1, v2, v3, normal, color); // Q0
        AddQuad(v8, v9, v3, v4, normal, color); // Q1
        AddQuad(v7, v8, v5, v6, normal, color); // Q2
        AddRoundedCorner(CornerPosition::TopLeft, { v1.x, v1.y }, { v9.x,v9.y }, normal, color);
        AddScoopedCorner(CornerPosition::BottomRight, { v10.x, v10.y }, { v12.x,v12.y }, normal, color);
    }

    void Mesh::AddBottomCar(const glm::vec2& min, const glm::vec2& max,
        float radius, float scoopRadius,
        float sideWidth, float horzHeight,
        const glm::vec3& normal, const glm::vec4& color)
    {
        glm::vec3 v0 = { min.x + radius, min.y + radius, 0.0f };
        glm::vec3 v1 = { v0.x, max.y, 0.0f };
        glm::vec3 v2 = { min.x, max.y, 0.0f };
        glm::vec3 v3 = { v2.x, v0.y, 0.0f };
        glm::vec3 v4 = { v0.x, min.y, 0.0f };
        glm::vec3 v5 = { v2.x + sideWidth, v4.y, 0.0f };
        glm::vec3 v6 = { max.x, v4.y, 0.0f };
        glm::vec3 v7 = { v6.x, v6.y + horzHeight, 0.0f };
        glm::vec3 v8 = { v5.x, v7.y, 0.0f };
        glm::vec3 v9 = { v5.x, v1.y, 0.0f };
        glm::vec3 v10 = { v8.x + scoopRadius, v7.y, 0.0f };
        glm::vec3 v11 = { v10.x, v10.y + scoopRadius, 0.0f };
        glm::vec3 v12 = { v5.x, v11.y, 0.0f };
        AddQuad(v0, v1, v2, v3, normal, color); // Q0
        AddQuad(v4, v5, v9, v1, normal, color); // Q1
        AddQuad(v5, v6, v7, v8, normal, color); // Q2
        AddRoundedCorner(CornerPosition::BottomLeft, { min.x, min.y }, { v0.x, v0.y }, normal, color);
        AddScoopedCorner(CornerPosition::TopRight, { v8.x, v8.y }, { v11.x, v11.y }, normal, color);
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
        va->AddBuffer(vb, layout);
        ib->Init(indicies.data(), indicies.size());
    }


    void Mesh::Load(const std::string& mesh_name, const std::string& file_path)
    {
        objl::Loader loader;
        bool loaded = loader.LoadFile(file_path);

        if (!loaded)
        {
            UX_LOG_ERROR("Failed to load %s", file_path);
        }
        if (loaded)
        {
            UX_LOG_INFO("READING: %s", file_path);
            for (int i = 0; i < loader.LoadedMeshes.size(); i++)
            {
                objl::Mesh curMesh = loader.LoadedMeshes[i];
                UX_LOG_INFO("Mesh %d: %s", i, curMesh.MeshName);
                if (mesh_name == curMesh.MeshName)
                {
                    UX_LOG_INFO("LOADING: %s", mesh_name);
                    for (size_t j = 0; j < curMesh.Vertices.size(); j++)
                    {
                        glm::vec3 position = { curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z };
                        glm::vec2 texCoord = { curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y };
                        glm::vec3 normal = { curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z };
                        AddVertex(position, glm::vec4(1), texCoord, normal);
                    }

                    // Go through every 3rd index and print the
                    //	triangle that these indices represent
                    for (size_t j = 0; j < curMesh.Indices.size(); j += 3)
                    {
                        uint32_t index1 = curMesh.Indices[j];
                        uint32_t index2 = curMesh.Indices[j + 1];
                        uint32_t index3 = curMesh.Indices[j + 2];
                        AddIndex(index1, index2, index3);
                    }
                }

            }
            Build();
        }
    }

}