#include "Camera.hpp"

// std
#include <cassert>
#include <limits>

// Матрица ортогонального проецирования перестраивается каждый кадр, чтобы размеры ортогонального объёма просмотра
// всегда соответствовали текущему значению соотношения сторон окна.
// Aspect ratio подставляется именно в -left и right, чтобы соответствовать выражению: right - left = aspect * (bottom - top)
// И в таком случае ортогональный объём просмотра будет иметь такое же соотношение сторон, что и окно.
// Это избавляет отображаемый объект от искажений, связанных с соотношением сторон.
void WrpCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
{
    projectionMatrix = glm::mat4{1.0f};
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
}

// Установка матрицы проецирования перспективы.
// Первый аргумент - vertical field of view (обычно в диапазоне от 45 до 60 градусов), далее соотношение сторон окна,
// далее расстояние до ближней и затем дальней плоскости отсечения.
// Главное отличие от простого ортогонального проецирования - объект становится тем меньше, чем он дальше от передней плоскости.
// Это тип проецирования чаще всего используется в играх.
void WrpCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    projectionMatrix = glm::mat4{0.0f};
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
}

void WrpCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    // ортонормированный базис
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    // первые три столбца задают поворот камеры
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    // четвёртый столбец совершает сдвиг позиции обратно в начало координат
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    // вместе с этим сразу обновляем обратную матрицу просмотра (камеры)
    // для перевода позиций обратно в world space при необходимости
    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
}

void WrpCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    // Для аргумента direction задаётся вектор от позиции камеры до позиции цели
    setViewDirection(position, target - position, up);
}

void WrpCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    viewMatrix = glm::mat4{1.f};
    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);

    // вместе с этим сразу обновляем обратную матрицу просмотра (камеры)
    inverseViewMatrix = glm::mat4{1.f};
    inverseViewMatrix[0][0] = u.x;
    inverseViewMatrix[0][1] = u.y;
    inverseViewMatrix[0][2] = u.z;
    inverseViewMatrix[1][0] = v.x;
    inverseViewMatrix[1][1] = v.y;
    inverseViewMatrix[1][2] = v.z;
    inverseViewMatrix[2][0] = w.x;
    inverseViewMatrix[2][1] = w.y;
    inverseViewMatrix[2][2] = w.z;
    inverseViewMatrix[3][0] = position.x;
    inverseViewMatrix[3][1] = position.y;
    inverseViewMatrix[3][2] = position.z;
}
