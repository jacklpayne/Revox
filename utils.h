#pragma once
namespace SM {
    constexpr int STANDARD_CHUNK_SIZE{ 16 };

    template <typename T>
    struct vec3 {
        T x, y, z;

        vec3 operator+(const vec3& other) const {
            return { x + other.x, y + other.y, z + other.z };
        }
        vec3 operator-(const vec3& other) const {
            return { x - other.x, y - other.y, z - other.z };
        }
        vec3 operator*(T scalar) const {
            return { x * scalar, y * scalar, z * scalar };
        }
        vec3 operator/(T scalar) const {
            return { x / scalar, y / scalar, z / scalar };
        }
        vec3& operator+=(const vec3& other) {
            x += other.x; y += other.y; z += other.z;
            return *this;
        }
        vec3& operator-=(const vec3& other) {
            x -= other.x; y -= other.y; z -= other.z;
            return *this;
        }
        vec3& operator+=(T scalar) {
            x += scalar; y += scalar; z += scalar;
            return *this;
        }
        vec3& operator*=(T scalar) {
            x *= scalar; y *= scalar; z *= scalar;
            return *this;
        }
        vec3& operator/=(T scalar) {
            x /= scalar; y /= scalar; z /= scalar;
            return *this;
        }
        vec3 operator-() const {
            return { -x, -y, -z };
        }
    };

    struct DrawParams {
        SM::vec3<float> origin;
        SM::vec3<float> color;
        float size_mult;
    };
}