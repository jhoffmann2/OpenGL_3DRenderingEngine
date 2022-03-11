// -----------------------------------------------------------------------------
// @Project Name    SwitchBlade
// @filename        LowDiscrepancyRandom.h
// @date            3/5/2022
// @author          Jordan Hoffmann
//
// Copyright � 2021 DigiPen, All rights reserved.
// -----------------------------------------------------------------------------
#pragma once
#include <glm/glm.hpp>
#include <array>
#include <random>

template<size_t n>
std::array<glm::vec4, n> Hammersley2D()
{
    float p = 0;
    float u = 0;
    size_t kk = 0;
    std::array<glm::vec4, n> out;

    for (int k=0; k<n; k++) {
        for (p=0.5f, kk=k, u=0.0f; kk; p*=0.5f, kk >>= 1)
            if (kk & 1)
                u += p;
        float v = ((float)k + 0.5f) / (float)n;
        out[k] = glm::vec4(u,v, 0, 0); // using vec4 so it packs correctly on the gpu
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(out.begin(), out.end(), rng);
    return out;
}