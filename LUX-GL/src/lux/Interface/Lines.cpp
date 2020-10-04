#include "Lines.hpp"


namespace lux {

    Lines::Lines()
    {     
    }

    void Lines::Add(const glm::vec3& p1, const glm::vec3& p2)
    {
        points.push_back(p1);
        points.push_back(p2);
        indices.push_back(index);
        index++;
        indices.push_back(index);
        index++;
    }

    void Lines::Build()
    {
        va = CreateRef<VertexArray>();
        vb = CreateRef<VertexBuffer>(&points[0], 3 * points.size() * sizeof(float));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3);
        va->AddBuffer(*vb, *layout);
        ib = CreateRef<IndexBuffer>(&indices[0], indices.size());
    }


    void Lines::Draw(const Renderer& renderer, const Shader& shader) const
    {
        renderer.Draw(GL_LINES, *va, *ib, shader);
    }

    void Lines::CreateGrid(float width, float depth, float xSteps, float zSteps, float y)
    {
        float xStep = width / xSteps;
        float zStep = depth / zSteps;
        for (float x = -width / 2.0f;x <= width / 2.0f;x += xStep)
        {
            Add(glm::vec3(x, y, -depth / 2.0f), glm::vec3(x, y, depth / 2.0f));
        }
        for (float z = -depth / 2.0f;z <= depth / 2.0f;z += xStep)
        {
            Add(glm::vec3(-width/2.0f, y, z), glm::vec3(width/2.0f, y, z));
        }
        Build();
    }


    void Lines::createCircle(float radius, float steps)
    {
        const float step = PI2f / steps;
        for (float ang = 0; ang < PI2f; ang += step)
        {
            Add(circle_xz(radius, ang), circle_xz(radius, ang + step));
            //    glm::vec3(radius * std::cosf(ang), 0, radius * std::sinf(ang)),
            //    glm::vec3(radius * std::cosf(ang + step), 0, radius * std::sinf(ang + step))
        }
        Build();
    }

    void Lines::createRing(float inner_radius, float outer_radius, float thickness, float steps)
    {
        double step = PI2 / steps;
        for (double ang = 0; ang < PI2; ang += step)
        {
            glm::vec3 p1 = glm::vec3(inner_radius * cos(ang), -thickness / 2, inner_radius * sin(ang));
            glm::vec3 p2 = glm::vec3(inner_radius * cos(ang + step), -thickness / 2, inner_radius * sin(ang + step));
            glm::vec3 p3 = glm::vec3(inner_radius * cos(ang), thickness / 2, inner_radius * sin(ang));
            glm::vec3 p4 = glm::vec3(inner_radius * cos(ang + step), thickness / 2, inner_radius * sin(ang + step));

            glm::vec3 p5 = glm::vec3(outer_radius * cos(ang), -thickness / 2, outer_radius * sin(ang));
            glm::vec3 p6 = glm::vec3(outer_radius * cos(ang + step), -thickness / 2, outer_radius * sin(ang + step));
            glm::vec3 p7 = glm::vec3(outer_radius * cos(ang), thickness / 2, outer_radius * sin(ang));
            glm::vec3 p8 = glm::vec3(outer_radius * cos(ang + step), thickness / 2, outer_radius * sin(ang + step));
            // front
            Add(p1, p2);
            Add(p3, p4);
            Add(p1, p3);
            // cross bars
            Add(p1, p5);
            Add(p3, p7);
            // back
            Add(p5, p6);
            Add(p7, p8);
            Add(p5, p7);
        }
        Build();
    }
}