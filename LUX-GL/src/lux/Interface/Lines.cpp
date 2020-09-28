#include "Lines.hpp"


namespace lux {

    Lines::Lines()
    {     
    }

    void Lines::add(const glm::vec3& p1, const glm::vec3& p2)
    {
        points.push_back(p1);
        points.push_back(p2);
        indices.push_back(index);
        index++;
        indices.push_back(index);
        index++;
    }

    void Lines::commit()
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


    void Lines::createCircle(double radius, double steps)
    {
        double step = PI2 / steps;
        for (double ang = 0; ang < PI2; ang += step)
        {
            add(
                glm::vec3(radius * cos(ang), 0, radius * sin(ang)),
                glm::vec3(radius * cos(ang + step), 0, radius * sin(ang + step))
            );
        }
        commit();
    }

    void Lines::createRing(double inner_radius, double outer_radius, double thickness, double steps)
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
            add(p1, p2);
            add(p3, p4);
            add(p1, p3);
            // cross bars
            add(p1, p5);
            add(p3, p7);
            // back
            add(p5, p6);
            add(p7, p8);
            add(p5, p7);
        }
        commit();
    }
}