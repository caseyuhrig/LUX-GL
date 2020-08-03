#include "Triangles.h";





namespace ux {

    void Triangles::add(XYZ p1, XYZ p2, XYZ p3)
    {
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        indices.push_back(index++);
        indices.push_back(index++);
        indices.push_back(index++);
    }

    void Triangles::add(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
    {
        add({ p1, glm::normalize(p1) }, { p2, glm::normalize(p2) }, { p3, glm::normalize(p3) });
    }

    void Triangles::commit()
    {
        va = CreateRef<VertexArray>();
        vb = CreateRef<VertexBuffer>(&points[0], 6 * points.size() * sizeof(float));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3); // point xyz
        layout->Push<float>(3); // normal xyz
        va->AddBuffer(*vb, *layout);
        ib = CreateRef<IndexBuffer>(&indices[0], indices.size());
    }


    void Triangles::Draw(const Renderer& renderer, const Shader& shader) const
    {
        renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
    }


    void Triangles::createRing(double inner_radius, double outer_radius, double thickness, double steps)
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

            // left side
            add(p1, p2, p3);
            add(p2, p3, p4);
            // right side
            add(p5, p6, p7);
            add(p6, p7, p8);
            // top
            add(p3, p4, p8);
            add(p8, p7, p3);
            // bottom
            add(p1, p2, p5);
            add(p4, p5, p6);
        }
        commit();
    }
}