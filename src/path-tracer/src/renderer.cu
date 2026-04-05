#include "renderer.cuh"

#include <iostream>
#include <cstdint>

void start_render(
    vec<3>* verts, uint32_t verts_len,
    uint32_t* tris, uint32_t tris_len,
    Materials mats,
    uint32_t* mat_indices, uint32_t mat_indices_len,
    vec<3> cam_pos, vec<3> cam_dir, float fov )
{
    std::cout << "=== start_render DEBUG ===\n";

    // verts
    std::cout << "verts (len=" << verts_len << "):\n";
    if (verts) {
        for (uint32_t i = 0; i < verts_len; ++i) {
            std::cout << "  [" << i << "] = ("
                      << verts[i].x << ", "
                      << verts[i].y << ", "
                      << verts[i].z << ")\n";
        }
    } else {
        std::cout << "  nullptr\n";
    }

    // tris
    std::cout << "tris (len=" << tris_len << "):\n";
    if (tris) {
        for (uint32_t i = 0; i < tris_len; ++i) {
            std::cout << "  [" << i << "] = " << tris[i] << "\n";
        }
    } else {
        std::cout << "  nullptr\n";
    }

    // materials
    std::cout << "Materials:\n";
    std::cout << "  length = " << mats.length << "\n";

    // colors
    std::cout << "  colors:\n";
    if (mats.colors) {
        for (uint32_t i = 0; i < mats.length; ++i) {
            std::cout << "    [" << i << "] = ("
                      << mats.colors[i].x << ", "
                      << mats.colors[i].y << ", "
                      << mats.colors[i].z << ")\n";
        }
    } else {
        std::cout << "    nullptr\n";
    }

    // smoothness
    std::cout << "  smoothness:\n";
    if (mats.smoothness) {
        for (uint32_t i = 0; i < mats.length; ++i) {
            std::cout << "    [" << i << "] = "
                      << mats.smoothness[i] << "\n";
        }
    } else {
        std::cout << "    nullptr\n";
    }

    // metallic
    std::cout << "  metallic:\n";
    if (mats.metallic) {
        for (uint32_t i = 0; i < mats.length; ++i) {
            std::cout << "    [" << i << "] = "
                      << mats.metallic[i] << "\n";
        }
    } else {
        std::cout << "    nullptr\n";
    }

    // material indices
    std::cout << "mat_indices (len=" << mat_indices_len << "):\n";
    if (mat_indices) {
        for (uint32_t i = 0; i < mat_indices_len; ++i) {
            std::cout << "  [" << i << "] = " << mat_indices[i] << "\n";
        }
    } else {
        std::cout << "  nullptr\n";
    }

    // camera
    std::cout << "cam_pos = ("
              << cam_pos.x << ", "
              << cam_pos.y << ", "
              << cam_pos.z << ")\n";

    std::cout << "cam_dir = ("
              << cam_dir.x << ", "
              << cam_dir.y << ", "
              << cam_dir.z << ")\n";

    std::cout << "fov = " << fov << "\n";

    std::cout << "=== end DEBUG ===\n";
}
