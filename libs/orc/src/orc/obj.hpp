#pragma once

#include <glm/glm.hpp>

namespace orc
{
    class Obj
    {
        public:
        Obj();

        void Translate(float x, float y, float z);

        void Rotate(float yaw, float pitch, float roll);

        virtual void ComputeMxs(glm::mat4 parentMx);

        glm::mat4 GetModelMx() const;

        glm::vec3 GetFront() const;

        glm::vec3 GetRight() const;

        glm::vec3 GetUp() const;

        glm::vec3 GetPosition() const;

        private:
        glm::vec3 pendingTranslation;
        glm::vec3 pendingRotation;
        glm::mat4 localMx;
        glm::mat4 worldMx;
    };
}
