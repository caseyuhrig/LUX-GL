#include "Cube.h"

#include <iostream>


namespace ux {


    Cube::Cube(const glm::vec3& min, const glm::vec3& max, const glm::mat4& transform)
    {
        float verticesBox[] = {
            min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            min.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,

            min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            min.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,

            min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,
            min.x, max.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, max.z, -1.0f,  0.0f,  0.0f,
            min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,

            max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,
            max.x, max.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, max.z,  1.0f,  0.0f,  0.0f,
            max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,

            min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            min.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,

            min.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            min.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            min.x, max.y, min.z,  0.0f,  1.0f,  0.0f
        };

        const size_t vb_size = 6 * 6 * 6;
        for (size_t n = 0;n < vb_size;n+=6) {
            glm::vec3 point(verticesBox[n], verticesBox[n + 1], verticesBox[n + 2]);
            point = transform * glm::vec4(point, 1.0);
            verticesBox[n] = point.x;
            verticesBox[n+1] = point.y;
            verticesBox[n+2] = point.z;
        }
        
        va = CreateRef<VertexArray>();
        vb = CreateRef<VertexBuffer>(verticesBox, sizeof(verticesBox));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3); // position xyz
        layout->Push<float>(3); // normal xyz

        va->AddBuffer(*vb, *layout);
        unsigned int indiciesBox[36];
        for (unsigned int n = 0;n < 36;n++)
        {
            indiciesBox[n] = n;
        }
        ib = CreateRef<IndexBuffer>(indiciesBox, 36);

        float positionsCubeLines[] = {
            min.x, min.y, min.z, // 0 - a - left bottom
            max.x, min.y, min.z, // 1 - b - right bottom
            max.x,  max.y, min.z, // 2 - c - right top
            min.x,  max.y, min.z, // 3 - d - left top

            min.x, min.y,  max.z, // 0 - a - left bottom
            max.x, min.y,  max.z, // 1 - b - right bottom
            max.x,  max.y,  max.z, // 2 - c - right top
            min.x,  max.y,  max.z, // 3 - d - left top
        };

        vaLines = CreateRef<VertexArray>();
        vbLines = CreateRef<VertexBuffer>(positionsCubeLines, 3 * 8 * sizeof(float));
        layoutLines = CreateRef<VertexBufferLayout>();
        layoutLines->Push<float>(3);
        vaLines->AddBuffer(*vbLines, *layoutLines);
        unsigned int indiciesCubeLines[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // front on 0 z plane
            4, 5, 5, 6, 6, 7, 7, 4, // back on 100 z plane
            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };
        ibLines = CreateRef<IndexBuffer>(indiciesCubeLines, 24);
    }


    Cube::Cube(const glm::vec3& min, const glm::vec3& max)
    {
        float verticesBox[] = {
            min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, min.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            max.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            min.x, max.y, min.z,  0.0f,  0.0f, -1.0f,
            min.x, min.y, min.z,  0.0f,  0.0f, -1.0f,

            min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, min.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            max.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            min.x, max.y, max.z,  0.0f,  0.0f,  1.0f,
            min.x, min.y, max.z,  0.0f,  0.0f,  1.0f,

            min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,
            min.x, max.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, min.z, -1.0f,  0.0f,  0.0f,
            min.x, min.y, max.z, -1.0f,  0.0f,  0.0f,
            min.x, max.y, max.z, -1.0f,  0.0f,  0.0f,

            max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,
            max.x, max.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, min.z,  1.0f,  0.0f,  0.0f,
            max.x, min.y, max.z,  1.0f,  0.0f,  0.0f,
            max.x, max.y, max.z,  1.0f,  0.0f,  0.0f,

            min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, min.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            max.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            min.x, min.y, max.z,  0.0f, -1.0f,  0.0f,
            min.x, min.y, min.z,  0.0f, -1.0f,  0.0f,

            min.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, min.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            max.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            min.x, max.y, max.z,  0.0f,  1.0f,  0.0f,
            min.x, max.y, min.z,  0.0f,  1.0f,  0.0f
        };

        va = CreateRef<VertexArray>();
        vb = CreateRef<VertexBuffer>(verticesBox, sizeof(verticesBox));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3); // position xyz
        layout->Push<float>(3); // normal xyz

        va->AddBuffer(*vb, *layout);
        unsigned int indiciesBox[36];
        for (unsigned int n = 0;n < 36;n++)
        {
            indiciesBox[n] = n;
        }
        ib = CreateRef<IndexBuffer>(indiciesBox, 36);

        float positionsCubeLines[] = {
            min.x, min.y, min.z, // 0 - a - left bottom
            max.x, min.y, min.z, // 1 - b - right bottom
            max.x,  max.y, min.z, // 2 - c - right top
            min.x,  max.y, min.z, // 3 - d - left top

            min.x, min.y,  max.z, // 0 - a - left bottom
            max.x, min.y,  max.z, // 1 - b - right bottom
            max.x,  max.y,  max.z, // 2 - c - right top
            min.x,  max.y,  max.z, // 3 - d - left top
        };

        vaLines = CreateRef<VertexArray>();
        vbLines = CreateRef<VertexBuffer>(positionsCubeLines, 3 * 8 * sizeof(float));
        layoutLines = CreateRef<VertexBufferLayout>();
        layoutLines->Push<float>(3);
        vaLines->AddBuffer(*vbLines, *layoutLines);
        unsigned int indiciesCubeLines[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // front on 0 z plane
            4, 5, 5, 6, 6, 7, 7, 4, // back on 100 z plane
            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };
        ibLines = CreateRef<IndexBuffer>(indiciesCubeLines, 24);
    }

    Cube::Cube(const glm::vec3& size)
    {
        float width = size.x / 2.0f;
        float height = size.y / 2.0f;
        float depth = size.z / 2.0f;

        float verticesBox[] = {
            -width, -height, -depth,  0.0f,  0.0f, -1.0f,
             width, -height, -depth,  0.0f,  0.0f, -1.0f,
             width,  height, -depth,  0.0f,  0.0f, -1.0f,
             width,  height, -depth,  0.0f,  0.0f, -1.0f,
            -width,  height, -depth,  0.0f,  0.0f, -1.0f,
            -width, -height, -depth,  0.0f,  0.0f, -1.0f,

            -width, -height,  depth,  0.0f,  0.0f,  1.0f,
             width, -height,  depth,  0.0f,  0.0f,  1.0f,
             width,  height,  depth,  0.0f,  0.0f,  1.0f,
             width,  height,  depth,  0.0f,  0.0f,  1.0f,
            -width,  height,  depth,  0.0f,  0.0f,  1.0f,
            -width, -height,  depth,  0.0f,  0.0f,  1.0f,

            -width,  height,  depth, -1.0f,  0.0f,  0.0f,
            -width,  height, -depth, -1.0f,  0.0f,  0.0f,
            -width, -height, -depth, -1.0f,  0.0f,  0.0f,
            -width, -height, -depth, -1.0f,  0.0f,  0.0f,
            -width, -height,  depth, -1.0f,  0.0f,  0.0f,
            -width,  height,  depth, -1.0f,  0.0f,  0.0f,

             width,  height,  depth,  1.0f,  0.0f,  0.0f,
             width,  height, -depth,  1.0f,  0.0f,  0.0f,
             width, -height, -depth,  1.0f,  0.0f,  0.0f,
             width, -height, -depth,  1.0f,  0.0f,  0.0f,
             width, -height,  depth,  1.0f,  0.0f,  0.0f,
             width,  height,  depth,  1.0f,  0.0f,  0.0f,

            -width, -height, -depth,  0.0f, -1.0f,  0.0f,
             width, -height, -depth,  0.0f, -1.0f,  0.0f,
             width, -height,  depth,  0.0f, -1.0f,  0.0f,
             width, -height,  depth,  0.0f, -1.0f,  0.0f,
            -width, -height,  depth,  0.0f, -1.0f,  0.0f,
            -width, -height, -depth,  0.0f, -1.0f,  0.0f,

            -width,  height, -depth,  0.0f,  1.0f,  0.0f,
             width,  height, -depth,  0.0f,  1.0f,  0.0f,
             width,  height,  depth,  0.0f,  1.0f,  0.0f,
             width,  height,  depth,  0.0f,  1.0f,  0.0f,
            -width,  height,  depth,  0.0f,  1.0f,  0.0f,
            -width,  height, -depth,  0.0f,  1.0f,  0.0f
        };

        float positionsCubeLines[] = {
            -width, -height, -0.5f, // 0 - a - left bottom
             width, -height, -0.5f, // 1 - b - right bottom
             width,  height, -0.5f, // 2 - c - right top
            -width,  height, -0.5f, // 3 - d - left top

            -width, -height,  0.5f, // 0 - a - left bottom
             width, -height,  0.5f, // 1 - b - right bottom
             width,  height,  0.5f, // 2 - c - right top
            -width,  height,  0.5f, // 3 - d - left top
        };

        va = CreateRef<VertexArray>();
        vb = CreateRef<VertexBuffer>(verticesBox, sizeof(verticesBox));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3); // position xyz
        layout->Push<float>(3); // normal xyz

        va->AddBuffer(*vb, *layout);
        unsigned int indiciesBox[36];
        for (unsigned int n = 0;n < 36;n++)
        {
            indiciesBox[n] = n;
        }
        ib = CreateRef<IndexBuffer>(indiciesBox, 36);

        vaLines = CreateRef<VertexArray>();
        vbLines = CreateRef<VertexBuffer>(positionsCubeLines, 3 * 8 * sizeof(float));
        layoutLines = CreateRef<VertexBufferLayout>();
        layoutLines->Push<float>(3);
        vaLines->AddBuffer(*vbLines, *layoutLines);
        unsigned int indiciesCubeLines[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // front on 0 z plane
            4, 5, 5, 6, 6, 7, 7, 4, // back on 100 z plane
            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };
        ibLines = CreateRef<IndexBuffer>(indiciesCubeLines, 24);

    }


    void Cube::commit() const {
        if (!has_commited)
        {
        }
    }

    void Cube::Draw(const Renderer& renderer, const Shader& shader) const
    {
        //commit();
        renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
    }


    void Cube::DrawOutline(const Renderer& renderer, const Shader& shader) const
    {
        //commit();
        renderer.Draw(GL_LINES, *vaLines, *ibLines, shader);
    }

    void Cube::transform(glm::mat4& transformation)
    {
    }
}