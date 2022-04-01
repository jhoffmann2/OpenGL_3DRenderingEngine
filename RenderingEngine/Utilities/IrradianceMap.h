// -----------------------------------------------------------------------------
// @Project Name    SwitchBlade
// @filename        IrradianceMap.h
// @date            3/21/2022
// @author          Jordan Hoffmann
//
// Copyright � 2021 DigiPen, All rights reserved.
// -----------------------------------------------------------------------------
#pragma once

#include <Materials/Texture.h>

class IrradianceMap
{
public:
    static Texture Generate(const std::filesystem::path& inFile);
private:
};

