#pragma once

#include "importer/MeshData.hpp"

#include <filesystem>

namespace LibFluid::Importer
{
    class ObjLoader
    {

      public:
        ObjLoader(std::filesystem::path filepath);

        float scale = 1.0f;

        MeshData load_as_meshdata();

      private:
        std::filesystem::path filepath;
    };
} // namespace LibFluid::Importer
