#include "ScriptInterface.hpp"
#include "chaiscript/chaiscript.hpp"
#include "core/entities/IEntity.hpp"
#include "core/entities/ParticleSpawner.hpp"

chaiscript::ChaiScript *getPtr(void *ptr) { return (chaiscript::ChaiScript *) ptr; }

chaiscript::ChaiScript &getRef(void *ptr) { return *((chaiscript::ChaiScript *) ptr); }

void FluidSolver::ScriptInterface::make_available() {

    chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());


    chaiscript::utility::add_class<vec2>(*m, "vec2",
                                         {
                                         },
                                         {
                                                 {chaiscript::fun(
                                                         &vec2::x), "x"},
                                                 {chaiscript::fun(
                                                         &vec2::y), "y"},
                                         });

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
    chaiscript::utility::add_class<ScenarioData::Viewport>(*m, "ScenarioViewport",
                                                           {
                                                           },
                                                           {
                                                                   {chaiscript::fun(
                                                                           &ScenarioData::Viewport::left),   "left"},
                                                                   {chaiscript::fun(
                                                                           &ScenarioData::Viewport::right),  "right"},
                                                                   {chaiscript::fun(
                                                                           &ScenarioData::Viewport::top),    "top"},
                                                                   {chaiscript::fun(
                                                                           &ScenarioData::Viewport::bottom), "bottom"},
                                                           });


    chaiscript::utility::add_class<ParticleSpawner::Parameters::Area>(*m, "ParticleSpawnerParametersArea",
                                                                      {
                                                                      },
                                                                      {
                                                                              {chaiscript::fun(
                                                                                      &ParticleSpawner::Parameters::Area::bottom), "bottom"},
                                                                              {chaiscript::fun(
                                                                                      &ParticleSpawner::Parameters::Area::top),    "top"},
                                                                              {chaiscript::fun(
                                                                                      &ParticleSpawner::Parameters::Area::left),   "left"},
                                                                              {chaiscript::fun(
                                                                                      &ParticleSpawner::Parameters::Area::right),  "right"},
                                                                      });


    chaiscript::utility::add_class<ParticleSpawner::Parameters>(*m, "ParticleSpawnerParameters",
                                                                {
                                                                },
                                                                {
                                                                        {chaiscript::fun(
                                                                                &ParticleSpawner::Parameters::rest_density),         "restDensity"},
                                                                        {chaiscript::fun(
                                                                                &ParticleSpawner::Parameters::particles_per_second), "particlesPerSecond"},
                                                                        {chaiscript::fun(
                                                                                &ParticleSpawner::Parameters::mass),                 "mass"},
                                                                        {chaiscript::fun(
                                                                                &ParticleSpawner::Parameters::initial_velocity),     "initialVelocity"},
                                                                        {chaiscript::fun(
                                                                                &ParticleSpawner::Parameters::area),                 "area"},
                                                                });

    chaiscript::utility::add_class<ParticleSpawner>(*m, "ParticleSpawner",
                                                    {
                                                            chaiscript::constructor<ParticleSpawner()>()
                                                    },
                                                    {
                                                            {chaiscript::fun(
                                                                    &ParticleSpawner::parameters), "parameters"},
                                                    });

    m->add(chaiscript::base_class<IEntity, ParticleSpawner>());

    auto &chai = getRef(chai_ptr);
    chai.add(m);

    chai.add_global(chaiscript::var(std::ref(info)), "scenarioInfo");
    chai.add_global(chaiscript::var(std::ref(data->viewport)), "scenarioViewport");
    chai.add(chaiscript::fun(&ScriptInterface::add_particle, this), "addParticle");
    chai.add(chaiscript::fun(&ScriptInterface::add_entity, this), "addEntity");


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

    mData.position = glm::vec2((float) particle.x, (float) particle.y);
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
    auto &chai = getRef(chai_ptr);
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

FluidSolver::ScriptInterface::ScriptInterface() {
    chai_ptr = new chaiscript::ChaiScript();
}

FluidSolver::ScriptInterface::~ScriptInterface() {
    delete ((chaiscript::ChaiScript *) chai_ptr);
}

void FluidSolver::ScriptInterface::add_entity(FluidSolver::IEntity *entity) {
    FLUID_ASSERT(data != nullptr);
    data->entities.push_back(entity);
}