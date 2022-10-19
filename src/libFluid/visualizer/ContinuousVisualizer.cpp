#include "ContinuousVisualizer.hpp"

#include "parallelization/StdParallelForEach.hpp"
#include "LibFluidMath.hpp"

namespace LibFluid {
    ContinuousVisualizer::ContinuousVisualizer()
        : image(0, 0), kernel() {
    }

    void ContinuousVisualizer::initialize() {
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();

            kernel.kernel_support = simulation_data.particle_size * Math::kernel_support_factor;
            kernel.initialize();
        }

        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();

            image = Image(parameters.render_target.width, parameters.render_target.height);
            recalculate_viewport();
        }
    }

    void ContinuousVisualizer::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(ContinuousVisualizer));

        if (simulation_data.collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!simulation_data.collection->is_type_present<MovementData>()) {
                report.add_issue("Particles are missing the MovementData attribute.");
            }
            if (!simulation_data.collection->is_type_present<ParticleData>()) {
                report.add_issue("Particles are missing the ParticleData attribute.");
            }
            if (!simulation_data.collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
        }

        if (simulation_data.particle_size <= 0.0f) {
            report.add_issue("Particle size is smaller or equal to zero.");
        }
        if (simulation_data.neighborhood_interface == nullptr) {
            report.add_issue("Neighborhood search interface is null.");
        }

        report.end_scope();
    }

    void ContinuousVisualizer::render() {
        // calculate color for each pixel

        using par = StdParallelForEach;

        par::loop_for(0, image.size(), [&](size_t i) {
            size_t x = i % image.width();
            size_t y = (i - (i % image.width())) / image.width();

            image.data()[i] = calculate_color_for_pixel(x, y);
        });
    }

    Image ContinuousVisualizer::get_image_data() {
        return image;
    }

    void ContinuousVisualizer::recalculate_viewport() {
        auto value = settings.viewport;

        Viewport2D res;
        res.top = value.top;
        res.left = value.left;

        if (value.width() / value.height() * (float)image.height() < (float)image.width()) {
            // height is okay, width must be larger

            auto height = value.height();
            auto width = (float)image.width() / (float)image.height() * value.height();

            // center the additional gained width
            res.left -= (width - value.width()) / 2.0f;

            // set width and height
            res.bottom = res.top - height;
            res.right = res.left + width;
        } else {
            // width is okay, height must be larger
            auto width = value.width();
            auto height = (float)image.height() / (float)image.width() * value.width();

            // center the additional gained height
            res.top += (height - value.height()) / 2.0f;

            // set width and height
            res.bottom = res.top - height;
            res.right = res.left + width;
        }

        internal_viewport = res;
    }

    Image::Color ContinuousVisualizer::calculate_color_for_pixel(size_t x, size_t y) {
        FLUID_ASSERT(simulation_data.neighborhood_interface != nullptr);
        FLUID_ASSERT(simulation_data.collection != nullptr);
        FLUID_ASSERT(simulation_data.collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(simulation_data.collection->is_type_present<MovementData>());
        FLUID_ASSERT(simulation_data.collection->is_type_present<ParticleData>());

        glm::vec2 position = calculate_particle_space_position_for_pixel(x, y);


        // calculate densities
        float boundaryDensity = 0.0f;
        float normalDensity = 0.0f;

        auto neighbors = simulation_data.neighborhood_interface->get_neighbors(position);

        for (size_t neighbor : neighbors) {
            const auto& pi = simulation_data.collection->get<ParticleInfo>(neighbor);
            if (pi.type == ParticleTypeInactive) {
                continue; // don*t calculate unnecessary values for inactive particles.
            }

            const auto& pm = simulation_data.collection->get<MovementData>(neighbor);
            const auto& pd = simulation_data.collection->get<ParticleData>(neighbor);

            float densityContribution = pd.mass * kernel.GetKernelValue(position, pm.position);

            if (pi.type == ParticleTypeNormal) {
                normalDensity += densityContribution;
            } else if (pi.type == ParticleTypeBoundary) {
                boundaryDensity += densityContribution;
            }
        }

        float density = normalDensity + boundaryDensity;

        // if density is too small return nothing
        if (density < settings.minimum_render_density)
            return settings.clear_color;

        // check which type of density is larger and return the corresponding color
        // first of, boundary density should dominate on a boundary
        if (boundaryDensity >= settings.minimum_render_density) {
            // boundary
            return Image::Color(128, 128, 128);
        } else {
            // fluid
            return Image::Color(0, 0, 255);
        }
    }

    glm::vec2 ContinuousVisualizer::calculate_particle_space_position_for_pixel(size_t x, size_t y) {
        float xCoord = internal_viewport.left;
        float yCoord = internal_viewport.top;
        xCoord += x / (float)image.width() * internal_viewport.width();
        yCoord -= y / (float)image.height() * internal_viewport.height();
        return glm::vec2(xCoord, yCoord);
    }
} // namespace FluidSolver