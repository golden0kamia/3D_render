#include "vertice.hpp"

vertice_3_f rot_x(vertice_3_f vertice, float alpha)
{
    return {
        vertice.x,
        ((vertice.y * cosf(alpha)) + (vertice.z * sinf(alpha))),
        ((vertice.z * cosf(alpha)) - (vertice.y * sinf(alpha)))
    };
}

vertice_3_f rot_y(vertice_3_f vertice, float alpha)
{
    return {
        vertice.x * cosf(alpha) - vertice.z * sinf(alpha),
        vertice.y,
        vertice.x * sinf(alpha) + vertice.z * cosf(alpha)
    };
}

vertice_3_f rot_z(vertice_3_f vertice, float alpha)
{
    return {
        vertice.x * cosf(alpha) + vertice.y * sinf(alpha),
        vertice.y * cosf(alpha) - vertice.x * sinf(alpha),
        vertice.z
    };
}

vertice_3_f rot_xyz(vertice_3_f vertice, vertice_3_f rotation)
{
    return rot_z(rot_y(rot_x(vertice, rotation.x), rotation.y), rotation.z);
}

float dot_product(vertice_3_f v1, vertice_3_f v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

vertice_3_f cross_product(vertice_3_f v1, vertice_3_f v2)
{
    return {
        (v1.y * v2.z) - (v1.z * v2.y),
        (v1.z * v2.x) - (v1.x * v2.z),
        (v1.x * v2.y) - (v1.y * v2.x)
    };
}

vertice_3_f diff_vertice(vertice_3_f v1, vertice_3_f v2)
{
    return {
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    };
}
