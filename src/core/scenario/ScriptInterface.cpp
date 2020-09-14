#include "ScriptInterface.hpp"


void FluidSolver::ScriptInterface::make_available() {

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
    chaiscript::utility::add_class<ScenarioInfo>(*m, "ScenarioInfo",
                                                 {
                                                 },
                                                 {
                                                         {chaiscript::fun(
                                                                 &ScenarioInfo::particle_size),         "particleSize"},
                                                         {chaiscript::fun(&ScenarioInfo::rest_density), "restDensity"},
                                                         {chaiscript::fun(&ScenarioInfo::name),         "scenarioName"},
                                                 });


    chai.add(m);

    chai.add_global(chaiscript::var(std::ref(info)), "scenarioInfo");
    chai.add(chaiscript::fun(&ScriptInterface::add_particle, this), "addParticle");


    chai.add_global_const(chaiscript::const_var((uint8_t) ParticleTypeNormal), "TYPE_NORMAL");
    chai.add_global_const(chaiscript::const_var((uint8_t) ParticleTypeBoundary), "TYPE_BOUNDARY");
    chai.add_global_const(chaiscript::const_var((uint8_t) ParticleTypeDead), "TYPE_INACTIVE");
}

FluidSolver::pIndex_t FluidSolver::ScriptInterface::add_particle(FluidSolver::ScriptInterface::Particle particle) {
    FLUID_ASSERT(data != nullptr)
    FLUID_ASSERT(data->collection.is_type_present<MovementData>())
    FLUID_ASSERT(data->collection.is_type_present<ParticleData>())
    FLUID_ASSERT(data->collection.is_type_present<ParticleInfo>())
    FLUID_ASSERT(data->collection.is_type_present<ExternalForces>())

    ParticleCollection *collection = &data->collection;

    auto id = collection->add();
    auto &mData = collection->get<MovementData>(id);
    auto &pData = collection->get<ParticleData>(id);
    auto &iData = collection->get<ParticleInfo>(id);

    mData.position = glm::vec2((float)particle.x, (float)particle.y);
    mData.velocity = glm::vec2(0.0f);
    mData.acceleration = glm::vec2(0.0f);
    pData.pressure = 0.0f;
    pData.density = particle.density;
    pData.mass = particle.mass;
    iData.type = particle.type;
    iData.tag = id;

    return id;
}

void FluidSolver::ScriptInterface::load_file(const std::string &filepath) {
    make_available();
    try {
        chai.eval_file(filepath);
    } catch (const chaiscript::exception::eval_error &e) {
        std::cerr << "Error\n" << e.pretty_print() << std::endl;
    }
    transfer_data();
}

void FluidSolver::ScriptInterface::transfer_data() {
    FLUID_ASSERT(data != nullptr);
    data->name = info.name;
    data->rest_density = info.rest_density;
    data->particle_size = info.particle_size;
}
