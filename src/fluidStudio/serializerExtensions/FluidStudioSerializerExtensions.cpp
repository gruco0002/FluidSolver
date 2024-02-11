#include "FluidStudioSerializerExtensions.hpp"

#include "serializerExtensions/VseGlParticleRenderer.hpp"
#include "serializerExtensions/VseGlParticleRenderer3D.hpp"

namespace FluidStudio::SerializerExtensions
{

    LibFluid::Serialization::SerializerExtensions create_serializer_extenstions()
    {
        LibFluid::Serialization::SerializerExtensions serializer_extensions;

        serializer_extensions.visualizer_serializer_extensions.push_back(std::make_shared<VseGlParticleRenderer>());
        serializer_extensions.visualizer_serializer_extensions.push_back(std::make_shared<VseGlParticleRenderer3D>());

        return serializer_extensions;
    }
} // namespace FluidStudio::SerializerExtensions
