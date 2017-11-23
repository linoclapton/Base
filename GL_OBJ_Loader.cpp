#include "GL_OBJ_Loader.h"


namespace ULY{
    namespace math
    {
        // Vector3 Cross Product
        glm::vec3 CrossV3(const glm::vec3 a, const glm::vec3 b)
        {
            return glm::vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
        }

        // Vector3 Magnitude Calculation
        float MagnitudeV3(const glm::vec3 in)
        {
            return (sqrtf(powf(in.x, 2) + powf(in.y, 2) + powf(in.z, 2)));
        }

        // Vector3 DotProduct
        float DotV3(const glm::vec3 a, const glm::vec3 b)
        {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        // Angle between 2 Vector3 Objects
        float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b)
        {
            float angle = DotV3(a, b);
            angle /= (MagnitudeV3(a) * MagnitudeV3(b));
            return angle = acosf(angle);
        }
    }
    namespace algorithm
    {
        // Vector3 Multiplication Opertor Overload

        // Check to see if a Vector3 Point is within a 3 Vector3 Triangle
        bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
        {
            // Starting vars
            glm::vec3 u = tri2 - tri1;
            glm::vec3 v = tri3 - tri1;
            glm::vec3 w = point - tri1;
            glm::vec3 n = math::CrossV3(u, v);

            float y = (math::DotV3(math::CrossV3(u, w), n) / math::DotV3(n, n));
            float b = (math::DotV3(math::CrossV3(u, w), n) / math::DotV3(n, n));
            float a = 1 - y - b;

            // Projected point
            glm::vec3 p = (a * tri1) + (b * tri2) + (y * tri3);

            if (a >= 0 && a <= 1
                && b >= 0 && b <= 1
                && y >= 0 && y <= 1)
            {
                return true;
            }
            else
                return false;
        }

    }

}