#include "segment.h"

#include <iostream>

#include "ux.h"
#include "math.h"


namespace ux {

    void set_vertices(float vertices[], unsigned int index, float x, float y, float z, float nx, float ny, float nz)
    {
        vertices[index] = x;
        vertices[index+1] = y;
        vertices[index+2] = z;
        vertices[index+3] = nx;
        vertices[index+4] = ny;
        vertices[index+5] = nz;
    }

    void set_vertices(float vertices[], unsigned int index, glm::vec3 point, glm::vec3 normal)
    {
        set_vertices(vertices, index, point.x, point.y, point.z, normal.x, normal.y, normal.z);
    }

    glm::vec3 computeNormal(
        glm::vec3 const& a,
        glm::vec3 const& b,
        glm::vec3 const& c)
    {
        return glm::normalize(glm::cross(c - a, b - a));
    }

    glm::vec3 computeNormal(
        glm::vec3 const& a,
        glm::vec3 const& b,
        glm::vec3 const& c,
        glm::vec3 const& offset)
    {
        glm::vec3 aa = a + offset;
        glm::vec3 bb = b + offset;
        glm::vec3 cc = c + offset;
        return glm::normalize(glm::cross(cc - aa, bb - aa));
    }

    Segment::Segment(const float inner_radius, const float outer_radius,
                     const float start_angle,  const float end_angle, 
                     const float thickness)
    {
        printf("GENERATING SEGMENT -->\n");
        // @weird For some reason the normals have to be flipped in opposite directions if smooth or flat lighting is used!
        unsigned int smooth = GL_SMOOTH;
        unsigned int steps = 8;
        float ang_step = (end_angle - start_angle) / steps;
        
        unsigned int floats_per_vertex = 6;
        unsigned int repeats = 4;
        unsigned int num_vertices = (steps * repeats * floats_per_vertex + 12) * 4; // 4 points per quad here!, confusing.
        vertices = new float[num_vertices];

        unsigned int num_indicies = steps * repeats * 6 + 12; // 6 points make up a quad because 2 tri per quad
        indicies = new unsigned int[num_indicies];
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
        printf("MIN: %.6f %.6f %.6f\n", min_x, min_y, min_z);
        printf("MAX: %.6f %.6f %.6f\n", max_x, max_y, max_z);
        float offset_x = -(min_x + max_x) / 2.0f;
        float offset_y = (min_y + max_y) / 2.0f;
        float offset_z = -(min_z + max_z) / 2.0f;
        printf("OFFSET: %.6f %.6f %.6f\n", offset_x, offset_y, offset_z);
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
            
            set_vertices(vertices, index, a, normal_top);
            set_vertices(vertices, index+6, b, normal_top);
            set_vertices(vertices, index+12, c, normal_top);
            set_vertices(vertices, index+18, d, normal_top);

            indicies[idx] = i;
            indicies[idx+1] = i+1;
            indicies[idx+2] = i+2;

            indicies[idx+3] = i+0;
            indicies[idx+4] = i+2;
            indicies[idx+5] = i+3;

            index += 24;
            idx += 6;
            i += 4;

            // BOTTOM

            a.y = b.y = c.y = d.y = y_bottom;

            set_vertices(vertices, index, a, normal_bottom);
            set_vertices(vertices, index + 6, b, normal_bottom);
            set_vertices(vertices, index + 12, c, normal_bottom);
            set_vertices(vertices, index + 18, d, normal_bottom);

            indicies[idx] = i;
            indicies[idx + 1] = i + 1;
            indicies[idx + 2] = i + 2;

            indicies[idx + 3] = i + 0;
            indicies[idx + 4] = i + 2;
            indicies[idx + 5] = i + 3;

            index += 24;
            idx += 6;
            i += 4;
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
                    set_vertices(vertices, index, a, -glm::normalize(a));
                    set_vertices(vertices, index + 6, b, -glm::normalize(b));
                    set_vertices(vertices, index + 12, c, -glm::normalize(c));
                    set_vertices(vertices, index + 18, d, -glm::normalize(d));
                }
                else {
                    glm::vec3 normal = computeNormal(a, b, c);
                    set_vertices(vertices, index, a, normal);
                    set_vertices(vertices, index + 6, b, normal);
                    set_vertices(vertices, index + 12, c, normal);
                    set_vertices(vertices, index + 18, d, normal);
                }

                indicies[idx] = i;
                indicies[idx + 1] = i + 1;
                indicies[idx + 2] = i + 2;

                indicies[idx + 3] = i + 0;
                indicies[idx + 4] = i + 2;
                indicies[idx + 5] = i + 3;

                index += 24;
                idx += 6;
                i += 4;
            }
            // OUTER
            {
                glm::vec3 a = glm::vec3(outer_radius * cos(ang * TO_RAD), y_top, outer_radius * sin(ang * TO_RAD));
                glm::vec3 b = glm::vec3(outer_radius * cos((ang + ang_step) * TO_RAD), y_top, outer_radius * sin((ang + ang_step) * TO_RAD));
                glm::vec3 c = glm::vec3(outer_radius * cos((ang + ang_step) * TO_RAD), y_bottom, outer_radius * sin((ang + ang_step) * TO_RAD));
                glm::vec3 d = glm::vec3(outer_radius * cos(ang * TO_RAD), y_bottom, outer_radius * sin(ang * TO_RAD));

                if (smooth == GL_SMOOTH)
                {
                    set_vertices(vertices, index, a, glm::normalize(a));
                    set_vertices(vertices, index + 6, b, glm::normalize(b));
                    set_vertices(vertices, index + 12, c, glm::normalize(c));
                    set_vertices(vertices, index + 18, d, glm::normalize(d));
                }
                else {
                    glm::vec3 normal = -computeNormal(a, b, c);
                    //printf("A: %.6f %.6f %.6f\n", a.x, a.y, a.z);
                    //printf("N: %.6f %.6f %.6f\n", normal.x, normal.y, normal.z);
                    set_vertices(vertices, index, a, normal);
                    set_vertices(vertices, index + 6, b, normal);
                    set_vertices(vertices, index + 12, c, normal);
                    set_vertices(vertices, index + 18, d, normal);
                }
                
                indicies[idx] = i;
                indicies[idx + 1] = i + 1;
                indicies[idx + 2] = i + 2;

                indicies[idx + 3] = i + 0;
                indicies[idx + 4] = i + 2;
                indicies[idx + 5] = i + 3;

                index += 24;
                idx += 6;
                i += 4;
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

            glm::vec3 normal = computeNormal(a, b, c, offset);

            set_vertices(vertices, index, a, normal);
            set_vertices(vertices, index + 6, b, normal);
            set_vertices(vertices, index + 12, c, normal);
            set_vertices(vertices, index + 18, d, normal);

            indicies[idx] = i;
            indicies[idx + 1] = i + 1;
            indicies[idx + 2] = i + 2;

            indicies[idx + 3] = i + 0;
            indicies[idx + 4] = i + 2;
            indicies[idx + 5] = i + 3;

            index += 24;
            idx += 6;
            i += 4;
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

            glm::vec3 normal = -computeNormal(a, b, c, offset); // WAS NEGATIVE

            set_vertices(vertices, index, a, normal);
            set_vertices(vertices, index + 6, b, normal);
            set_vertices(vertices, index + 12, c, normal);
            set_vertices(vertices, index + 18, d, normal);

            indicies[idx] = i;
            indicies[idx + 1] = i + 1;
            indicies[idx + 2] = i + 2;

            indicies[idx + 3] = i + 0;
            indicies[idx + 4] = i + 2;
            indicies[idx + 5] = i + 3;

            index += 24;
            idx += 6;
            i += 4;
        }


        std::cout << "num_vertices: " << num_vertices << std::endl;
        std::cout << "Index Count: " << index << std::endl;
        std::cout << "num_indices: " << num_indicies << std::endl;      
        std::cout << "IDX Count: " << idx << std::endl;

        va = CreateRef<VertexArray>();
        // 6 = 3 coord xyz and 3 normal xyz, 36 = # verticies
        //vb = CreateRef<VertexBuffer>(vertices, 6 * 36 * sizeof(float));
        vb = CreateRef<VertexBuffer>(vertices, index * sizeof(float));
        layout = CreateRef<VertexBufferLayout>();
        layout->Push<float>(3);
        layout->Push<float>(3);
        va->AddBuffer(*vb, *layout);
        ib = CreateRef<IndexBuffer>(indicies, idx);
    }


    void Segment::Draw(const Renderer& renderer, const Shader& shader) const
    {
        renderer.Draw(GL_TRIANGLES, *va, *ib, shader);
    }


    Segment::~Segment()
    {
        delete vertices;
        delete indicies;
    }
}