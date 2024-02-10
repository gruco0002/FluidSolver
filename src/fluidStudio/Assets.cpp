#include "Assets.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#ifndef WIN32
#define ASSETS_INCBIN_AVAILABLE
#endif

#ifdef ASSETS_INCBIN_AVAILABLE
#include <incbin/incbin_helper.hpp>

INCBIN(gl_particle_renderer_vertex_shader, "visualizer/shader/GLParticleRenderer.vert.glsl");
INCBIN(gl_particle_renderer_geometry_shader, "visualizer/shader/GLParticleRenderer.geom.glsl");
INCBIN(gl_particle_renderer_fragment_shader, "visualizer/shader/GLParticleRenderer.frag.glsl");

INCBIN(gl_particle_renderer_3d_vertex_shader, "visualizer/shader/GLParticleRenderer3D.vert.glsl");
INCBIN(gl_particle_renderer_3d_geometry_shader, "visualizer/shader/GLParticleRenderer3D.geom.glsl");
INCBIN(gl_particle_renderer_3d_fragment_shader, "visualizer/shader/GLParticleRenderer3D.frag.glsl");
#endif

constexpr auto ASSET_BASE_PATH = "./src/fluidStudio/";

namespace FluidStudio {

    std::filesystem::path get_relative_asset_path(Assets::Asset asset)
    {
        switch (asset)
        {
            case Assets::Asset::ParticleRendererVertexShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer.vert.glsl";
            case Assets::Asset::ParticleRendererGeometryShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer.geom.glsl";
            case Assets::Asset::ParticleRendererFragmentShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer.frag.glsl";

            case Assets::Asset::ParticleRenderer3DVertexShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer3D.vert.glsl";
            case Assets::Asset::ParticleRenderer3DGeometryShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer3D.geom.glsl";
            case Assets::Asset::ParticleRenderer3DFragmentShader:
                return std::filesystem::path(ASSET_BASE_PATH) / "visualizer/shader/GLParticleRenderer3D.frag.glsl";

            default:
                throw std::invalid_argument("Could not find asset!");
        }
    }

    std::filesystem::path get_asset_path(Assets::Asset asset)
    {
       auto relative_path = get_relative_asset_path(asset);
       const auto max_depth_check = 10;

       auto current_path = relative_path;
       for(auto i = 0; i < max_depth_check; i++)
       {
           if(std::filesystem::exists(current_path)){
               return current_path;
           }
           current_path = ".." / current_path;
       }

       throw std::runtime_error("Could not find asset: " + relative_path.string());
    }


    std::string load_string_asset_from_file(Assets::Asset asset)
    {
        auto path = get_asset_path(asset);

        std::ifstream file_stream(path);
        std::string file_content((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());

        return std::move(file_content);
    }

#ifdef ASSETS_INCBIN_AVAILABLE
    std::string load_string_asset_from_incbin(Assets::Asset asset)
    {
        switch (asset)
        {
        case Assets::Asset::ParticleRendererVertexShader:
            return incbin_as_string(g_gl_particle_renderer_vertex_shader_data,
                                    g_gl_particle_renderer_vertex_shader_size);
        case Assets::Asset::ParticleRendererGeometryShader:
            return incbin_as_string(g_gl_particle_renderer_geometry_shader_data,
                                    g_gl_particle_renderer_geometry_shader_size);
        case Assets::Asset::ParticleRendererFragmentShader:
            return incbin_as_string(g_gl_particle_renderer_fragment_shader_data,
                                    g_gl_particle_renderer_fragment_shader_size);

        case Assets::Asset::ParticleRenderer3DVertexShader:
            return incbin_as_string(g_gl_particle_renderer_3d_vertex_shader_data,
                                    g_gl_particle_renderer_3d_vertex_shader_size);
        case Assets::Asset::ParticleRenderer3DGeometryShader:
            return incbin_as_string(g_gl_particle_renderer_3d_geometry_shader_data,
                                    g_gl_particle_renderer_3d_geometry_shader_size);
        case Assets::Asset::ParticleRenderer3DFragmentShader:
            return incbin_as_string(g_gl_particle_renderer_3d_fragment_shader_data,
                                    g_gl_particle_renderer_3d_fragment_shader_size);

        default:
            throw std::invalid_argument("Could not find asset!");
        }
    }
#endif


    std::string Assets::get_string_asset(Assets::Asset asset)
    {
#ifdef ASSETS_INCBIN_AVAILABLE
        return std::move(load_string_asset_from_incbin(asset));
#else
        return std::move(load_string_asset_from_file(asset));
#endif
    }


} // namespace FluidUi