#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace lux {

    class MatrixUtils
    {
    public:
        MatrixUtils() {}
        ~MatrixUtils() = default;

        static glm::mat4 RotateAroundAxis(glm::mat4 model, glm::vec3 axis, glm::vec3 angle)
        {
            //glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), angle.x, glm::vec3(1.0, 0.0, 0.0));
            //rotMat = glm::rotate(rotMat, angle.y, glm::vec3(0.0, 1.0, 0.0));
            //rotMat = glm::rotate(rotMat, angle.z, glm::vec3(0.0, 0.0, 1.0));
            //rotMat = glm::translate(rotMat, axis);
            //return rotMat * model;
            return glm::translate(
                glm::rotate(
                    glm::rotate(
                        glm::rotate(glm::mat4(1.0),
                            angle.x, glm::vec3(1.0, 0.0, 0.0)),
                        angle.y, glm::vec3(0.0, 1.0, 0.0)),
                    angle.z, glm::vec3(0.0, 0.0, 1.0)),
                axis) * model;
        }

        static glm::mat4 Transform(glm::mat4 root, glm::vec3 scale, glm::vec3 translate, glm::vec3 rotate)
        {
            glm::mat4 m2scale = glm::scale(glm::mat4(1.0), scale);
            glm::mat4 m2trans = glm::translate(glm::mat4(1.0), translate);
            glm::mat4 m2rotate1 = glm::rotate(glm::mat4(1.0), rotate[0], glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 m2rotate2 = glm::rotate(glm::mat4(1.0), rotate[1], glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 m2rotate3 = glm::rotate(glm::mat4(1.0), rotate[2], glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 matrix = m2rotate3 * m2rotate2 * m2rotate1 * m2trans * m2scale;
            return root * matrix;
        }
    };
}