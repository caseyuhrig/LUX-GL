#pragma once

#include <glm/gtx/normal.hpp>

#include "lux/Types.hpp"
#include "lux/Primitive/Mesh.hpp"
#include "lux/Renderer/Renderer.hpp"
#include "lux/Renderer/Shader.hpp"
#include "lux/Renderer/VertexBufferLayout.hpp"


namespace lux {

   

    class Segment : public Mesh
    {
    public:
        Segment(const float inner_radius, const float outer_radius,
            const float start_angle, const float end_angle,
            const float thickness) 
            : inner_radius(inner_radius), outer_radius(outer_radius), 
            start_angle(start_angle), end_angle(end_angle),
            thickness(thickness)
        {
        }

        void CreateGeometry() override
        {
            // @weird For some reason the normals have to be flipped in opposite directions if smooth or flat lighting is used!
            unsigned int smooth = GL_SMOOTH;
            unsigned int steps = 8;
            float ang_step = (end_angle - start_angle) / steps;

            
            // mins
            float min_x = 10000.0f, min_y = -thickness / 2.0f, min_z = 10000.0f;
            float max_x = -10000.0f, max_y = thickness / 2.0f, max_z = -10000.0f;
            for (float ang = start_angle; ang <= end_angle; ang += ang_step)
            {
                glm::vec3 p1 = glm::vec3(inner_radius * cos(ang * TO_RAD), 0.0f, inner_radius * sin(ang * TO_RAD));
                glm::vec3 p2 = glm::vec3(outer_radius * cos(ang * TO_RAD), 0.0f, outer_radius * sin(ang * TO_RAD));
                min_x = glm::min(glm::min(min_x, p1.x), p2.x);
                min_z = glm::min(glm::min(min_z, p1.z), p2.z);
                max_x = glm::max(glm::max(max_x, p1.x), p2.x);
                max_z = glm::max(glm::max(max_z, p1.z), p2.z);
            }
            //printf("MIN: %.6f %.6f %.6f\n", min_x, min_y, min_z);
            //printf("MAX: %.6f %.6f %.6f\n", max_x, max_y, max_z);
            float offset_x = -(min_x + max_x) / 2.0f;
            float offset_y = (min_y + max_y) / 2.0f;
            float offset_z = -(min_z + max_z) / 2.0f;
            //printf("OFFSET: %.6f %.6f %.6f\n", offset_x, offset_y, offset_z);
            glm::vec3 offset = glm::vec3(offset_x, offset_y, offset_z);
            //std::cout << "MIN: " << min_x << ' ' << min_y << ' ' << min_z << std::endl;
            //std::cout << "MAX: " << max_x << ' ' << max_y << ' ' << max_z << std::endl;
            // draw on the x/z plane
            unsigned int index = 0;
            unsigned int idx = 0;
            unsigned int i = 0;
            glm::vec3 normal_top = glm::vec3(0.0, 1.0, 0.0);
            glm::vec3 normal_bottom = glm::vec3(0.0, -1.0, 0.0);
            float y_top = thickness / 2.0;
            float y_bottom = -thickness / 2.0;

            for (float ang = start_angle; ang < end_angle; ang += ang_step)
            {
                // TOP

                glm::vec3 a = glm::vec3(outer_radius * cos(ang * TO_RAD), y_top, outer_radius * sin(ang * TO_RAD));
                glm::vec3 b = glm::vec3(outer_radius * cos((ang + ang_step) * TO_RAD), y_top, outer_radius * sin((ang + ang_step) * TO_RAD));
                glm::vec3 c = glm::vec3(inner_radius * cos((ang + ang_step) * TO_RAD), y_top, inner_radius * sin((ang + ang_step) * TO_RAD));
                glm::vec3 d = glm::vec3(inner_radius * cos(ang * TO_RAD), y_top, inner_radius * sin(ang * TO_RAD));
               
                AddQuad(a, b, c, d, normal_top, color);

                // BOTTOM

                a.y = b.y = c.y = d.y = y_bottom;
                AddQuad(a, b, c, d, normal_bottom, color);
            }

            for (float ang = start_angle; ang < end_angle; ang += ang_step)
            {
                // INNER
                {
                    glm::vec3 a = glm::vec3(inner_radius * cos(ang * TO_RAD), y_top, inner_radius * sin(ang * TO_RAD));
                    glm::vec3 b = glm::vec3(inner_radius * cos((ang + ang_step) * TO_RAD), y_top, inner_radius * sin((ang + ang_step) * TO_RAD));
                    glm::vec3 c = glm::vec3(inner_radius * cos((ang + ang_step) * TO_RAD), y_bottom, inner_radius * sin((ang + ang_step) * TO_RAD));
                    glm::vec3 d = glm::vec3(inner_radius * cos(ang * TO_RAD), y_bottom, inner_radius * sin(ang * TO_RAD));

                    if (smooth == GL_SMOOTH)
                    {
                        glm::vec3 normal = -glm::triangleNormal(a, b, c);
                        //set_vertices(vertices, index, a, -glm::normalize(a));
                        //set_vertices(vertices, index + 6, b, -glm::normalize(b));
                        //set_vertices(vertices, index + 12, c, -glm::normalize(c));
                        //set_vertices(vertices, index + 18, d, -glm::normalize(d));
                        AddQuad(a, b, c, d, normal, color);
                    }
                    else {
                        glm::vec3 normal = -glm::triangleNormal(a, b, c);
                       
                        AddQuad(a, b, c, d, normal, color);
                    }
                }
                // OUTER
                {
                    glm::vec3 a = glm::vec3(outer_radius * cos(ang * TO_RAD), y_top, outer_radius * sin(ang * TO_RAD));
                    glm::vec3 b = glm::vec3(outer_radius * cos((ang + ang_step) * TO_RAD), y_top, outer_radius * sin((ang + ang_step) * TO_RAD));
                    glm::vec3 c = glm::vec3(outer_radius * cos((ang + ang_step) * TO_RAD), y_bottom, outer_radius * sin((ang + ang_step) * TO_RAD));
                    glm::vec3 d = glm::vec3(outer_radius * cos(ang * TO_RAD), y_bottom, outer_radius * sin(ang * TO_RAD));

                    if (smooth == GL_SMOOTH)
                    {
                        glm::vec3 normal = glm::triangleNormal(a, b, c);
                        //set_vertices(vertices, index, a, glm::normalize(a));
                        //set_vertices(vertices, index + 6, b, glm::normalize(b));
                        //set_vertices(vertices, index + 12, c, glm::normalize(c));
                        //set_vertices(vertices, index + 18, d, glm::normalize(d));
                        AddQuad(a, b, c, d, normal, color);
                    }
                    else {
                        glm::vec3 normal = glm::triangleNormal(a, b, c);
                        AddQuad(a, b, c, d, normal, color);
                    }
                }
            }

            // DRAW THE 2 TRIANGLES FOR THE FIRST SIDE FACE

            {
                float ang = start_angle;
                float y_top = thickness / 2.0;
                float y_bottom = -thickness / 2.0;

                glm::vec3 a = glm::vec3(outer_radius * cos(ang * TO_RAD), y_bottom, outer_radius * sin(ang * TO_RAD));
                glm::vec3 b = glm::vec3(outer_radius * cos(ang * TO_RAD), y_top, outer_radius * sin(ang * TO_RAD));
                glm::vec3 c = glm::vec3(inner_radius * cos(ang * TO_RAD), y_top, inner_radius * sin(ang * TO_RAD));
                glm::vec3 d = glm::vec3(inner_radius * cos(ang * TO_RAD), y_bottom, inner_radius * sin(ang * TO_RAD));

                // TODO !!! I believe just the x normal needs to be inversed.
                glm::vec3 normal = -glm::triangleNormal(a + offset, b + offset, c + offset);
                AddQuad(a, b, c, d, normal, color);
            }

            // DRAW THE 2 TRIANGLES FOR THE END ANGLE FACE

            // CENTER THE OBJECT BEFORE CALCULATING NORMALS THEN TRANSLATE BACK. 

            {
                float ang = end_angle;
                float y_top = thickness / 2.0;
                float y_bottom = -thickness / 2.0;

                glm::vec3 a = glm::vec3(outer_radius * cos(ang * TO_RAD), y_bottom, outer_radius * sin(ang * TO_RAD));
                glm::vec3 b = glm::vec3(outer_radius * cos(ang * TO_RAD), y_top, outer_radius * sin(ang * TO_RAD));
                glm::vec3 c = glm::vec3(inner_radius * cos(ang * TO_RAD), y_top, inner_radius * sin(ang * TO_RAD));
                glm::vec3 d = glm::vec3(inner_radius * cos(ang * TO_RAD), y_bottom, inner_radius * sin(ang * TO_RAD));

                glm::vec3 normal = glm::triangleNormal(a + offset, b + offset, c + offset); // WAS NEGATIVE
                AddQuad(a, b, c, d, normal, color);
            }

        }
    private:
        float inner_radius, outer_radius;
        float start_angle, end_angle;
        float thickness;
    };

}