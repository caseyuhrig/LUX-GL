#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace lux {

    class MatrixUtils
    {
    public:
        MatrixUtils() {}
        ~MatrixUtils() = default;

        static const glm::mat4& RotateAroundAxis(const glm::mat4& model, const glm::vec3& axis, const glm::vec3& angle)
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

        static const glm::mat4& Transform(const glm::mat4& root, const glm::vec3& scale, const glm::vec3& translate, const glm::vec3& rotate)
        {
            const auto& identity = glm::mat4(1.0);
            const glm::mat4& m2scale = glm::scale(identity, scale);
            const glm::mat4& m2trans = glm::translate(identity, translate);
            const glm::mat4& m2rotate1 = glm::rotate(identity, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
            const glm::mat4& m2rotate2 = glm::rotate(identity, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
            const glm::mat4& m2rotate3 = glm::rotate(identity, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));
            const glm::mat4& matrix = m2rotate3 * m2rotate2 * m2rotate1 * m2trans * m2scale;
            return root * matrix;
        }
    };
}