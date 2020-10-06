#ifndef FLUIDSOLVER_SCRIPTINTERFACE_HPP
#define FLUIDSOLVER_SCRIPTINTERFACE_HPP

#include <core/entities/IEntity.hpp>
#include "ScenarioData.hpp"

namespace FluidSolver {
    class ScriptInterface {
    private:

        void* chai_ptr;

        void make_available();

        void transfer_data();

    private:
        struct Particle {
            float x = 0.0f;
            float y = 0.0f;
            uint8_t type = ParticleType::ParticleTypeNormal;
            float density;
            float mass;
        };

        struct ScenarioInfo{
            float particle_size = 1.0f;
            float rest_density = 1.0f;
            std::string name;
        } info;




        pIndex_t add_particle(Particle particle);

        void add_entity(IEntity *entity);

    public:

        ScenarioData *data = nullptr;

        void load_file(const std::string &filepath);

        ScriptInterface();
        ~ScriptInterface();

    };

}

#endif //FLUIDSOLVER_SCRIPTINTERFACE_HPP
