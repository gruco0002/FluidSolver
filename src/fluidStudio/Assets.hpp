#pragma once

#include <string>

namespace FluidUi
{
    struct Assets
    {
        enum class Asset
        {
            ParticleRendererVertexShader,
            ParticleRendererGeometryShader,
            ParticleRendererFragmentShader,

            ParticleRenderer3DVertexShader,
            ParticleRenderer3DGeometryShader,
            ParticleRenderer3DFragmentShader,
        };

        static std::string get_string_asset(Asset asset);
    };
} // namespace FluidUi
