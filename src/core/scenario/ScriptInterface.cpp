#include "ScriptInterface.hpp"


void FluidSolver::ScriptInterface::make_available(chaiscript::ChaiScript &chai) {

    chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

    chaiscript::utility::add_class<Particle>(*m,
                                             "Particle",
                                             {
                                                     chaiscript::constructor<Particle()>()
                                             },
                                             {
                                                     {chaiscript::fun(&Particle::x),       "x"},
                                                     {chaiscript::fun(&Particle::y),       "y"},
                                                     {chaiscript::fun(&Particle::type),    "type"},
                                                     {chaiscript::fun(&Particle::density), "density"},
                                                     {chaiscript::fun(&Particle::mass),    "mass"},
                                             });
    chai.add(m);

    chai.add(chaiscript::fun(&ScriptInterface::add_particle, this), "addParticle");
}

FluidSolver::pIndex_t FluidSolver::ScriptInterface::add_particle(FluidSolver::ScriptInterface::Particle particle) {
    FLUID_ASSERT(current != nullptr)
    FLUID_ASSERT(current->collection.is_type_present<MovementData>())
    FLUID_ASSERT(current->collection.is_type_present<ParticleData>())
    FLUID_ASSERT(current->collection.is_type_present<ParticleInfo>())
    FLUID_ASSERT(current->collection.is_type_present<ExternalForces>())

    ParticleCollection *collection = &current->collection;

    auto id = collection->add();
    auto &mData = collection->get<MovementData>(id);
    auto &pData = collection->get<ParticleData>(id);
    auto &iData = collection->get<ParticleInfo>(id);

    mData.position = glm::vec2(particle.x, particle.y);
    mData.velocity = glm::vec2(0.0f);
    mData.acceleration = glm::vec2(0.0f);
    pData.pressure = 0.0f;
    pData.density = particle.density;
    pData.mass = particle.mass;
    iData.type = particle.type;
    iData.tag = id;

    return id;
}
