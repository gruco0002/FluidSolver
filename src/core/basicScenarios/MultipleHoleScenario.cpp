//
// Created by corbi on 09.07.2019.
//

#include "MultipleHoleScenario.hpp"
#include <core/fluidSolver/particleCollection/SimpleParticleCollection.hpp>
#include <core/simulationModifiers/DeathBox.hpp>
#include <core/simulationModifiers/SpawnArea.hpp>

void
FluidSolver::MultipleHoleScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    const int xOffset = -96 / 2;
    const int yOffset = -32 / 2;

    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    // boundaries
    for (int x = 1; x <= 3; x++) {
        for (int y = 5; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 13; x <= 15; x++) {
        for (int y = 5; y <= 16; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 4; x <= 29; x++) {
        for (int y = 30; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 27; x <= 29; x++) {
        for (int y = 26; y <= 29; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 37; x <= 39; x++) {
        for (int y = 16; y <= 20; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 37; x <= 39; x++) {
        for (int y = 24; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 40; x <= 66; x++) {
        for (int y = 30; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 51; x <= 53; x++) {
        for (int y = 27; y <= 29; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 64; x <= 66; x++) {
        for (int y = 16; y <= 27; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 93; x <= 95; x++) {
        for (int y = 5; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 77; x <= 92; x++) {
        for (int y = 30; y <= 32; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 85; x <= 87; x++) {
        for (int y = 5; y <= 9; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }
    for (int x = 76; x <= 78; x++) {
        for (int y = 5; y <= 9; y++) {
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }

    // tilted boundaries
    for (int y = 5; y <= 15; y++) {
        const int cutoffLeft = 37;
        const int cutoffRight = 50;
        const int startX = 37;
        const int startY = 5;
        for (int i = 0; i < 5; i++) {
            int x = startX + i + y - startY;
            if (x <= cutoffLeft || x >= cutoffRight) continue;
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }

    for (int y = 15; y >= 5; y--) {
        const int cutoffLeft = 54;
        const int cutoffRight = 67;
        const int startX = 53;
        const int startY = 15;
        for (int i = 0; i < 5; i++) {
            int x = startX + i - y + startY;
            if (x <= cutoffLeft || x >= cutoffRight) continue;
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }

    for (int y = 10; y <= 20; y++) {
        const int cutoffLeft = 75;
        const int cutoffRight = 88;
        const int startX = 75;
        const int startY = 10;
        for (int i = 0; i < 5; i++) {
            int x = startX + i + y - startY;
            if (x <= cutoffLeft || x >= cutoffRight) continue;
            addBoundary(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }

    // particles
    for (int x = 84; x <= 92; x++) {
        for (int y = 24; y <= 29; y++) {
            add(&particles, restDensity, x + xOffset, y + yOffset);
        }
    }

    // dead particles
    for (int i = 0; i < 1000 * factor; i++) {
        addDead(&particles, restDensity);
    }

    // set particles
    auto coll = dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection);
    coll->SetParticles(particles);
}

int FluidSolver::MultipleHoleScenario::GetParticleCountX() {
    return 96 * factor;
}

int FluidSolver::MultipleHoleScenario::GetParticleCountY() {
    return 32 * factor;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::MultipleHoleScenario::GetSimulationModifiers() {
    const int xOffset = -96 / 2;
    const int yOffset = 32 / 2;

    const float spawnRateLeft = 25.0f * factor * factor;
    const float spawnRateMiddle = 50.0f * factor * factor;
    const float spawnRateRight = 20.0f * factor * factor;
    const glm::vec2 initialSpeed = glm::vec2(0.0f, -6.0f);

    return {new FluidSolver::DeathBox((-0 + yOffset) * factor, (-2 + xOffset) * factor, (-36 + yOffset) * factor,
                                      (98 + xOffset) * factor),

            new FluidSolver::SpawnArea((-3 + yOffset) * factor, (5 + xOffset) * factor, (-5 + yOffset) * factor,
                                       (12 + xOffset) * factor, spawnRateLeft, initialSpeed),
            new FluidSolver::SpawnArea((-3 + yOffset) * factor, (43 + xOffset) * factor, (-5 + yOffset) * factor,
                                       (62 + xOffset) * factor, spawnRateMiddle, initialSpeed),
            new FluidSolver::SpawnArea((-3 + yOffset) * factor, (80 + xOffset) * factor, (-5 + yOffset) * factor,
                                       (84 + xOffset) * factor, spawnRateRight, initialSpeed),
            new FluidSolver::SpawnArea((-3 + yOffset) * factor, (89 + xOffset) * factor, (-5 + yOffset) * factor,
                                       (92 + xOffset) * factor, spawnRateRight, initialSpeed)
    };
}

void
FluidSolver::MultipleHoleScenario::add(std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> *particles,
                                       float restDensity, float x, float y) {

    float mass = restDensity * GetParticleSize() * GetParticleSize();

    FluidSolver::SimpleParticleCollection::FluidParticle p;
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = restDensity;
    p.Mass = mass;
    p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;

    for (int i = 0; i < factor; i++) {
        for (int j = 0; j < factor; j++) {
            float calcX = 1.0f / (float)factor * i;
            float calcY = 1.0f / (float)factor * j;
            calcX -= 0.5f;
            calcY -= 0.5f;

            p.Position = glm::vec2(((float) x) + calcX, (-(float) y) + calcY) * (float) factor;
            particles->push_back(p);
        }
    }
}

void FluidSolver::MultipleHoleScenario::addBoundary(
        std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> *particles, float restDensity, float x,
        float y) {
    float mass = restDensity * GetParticleSize() * GetParticleSize();

    FluidSolver::SimpleParticleCollection::FluidParticle p;
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = restDensity;
    p.Mass = mass;
    p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;


    for (int i = 0; i < factor; i++) {
        for (int j = 0; j < factor; j++) {
            float calcX = 1.0f / (float)factor * i;
            float calcY = 1.0f / (float)factor * j;
            calcX -= 0.5f;
            calcY -= 0.5f;

            p.Position = glm::vec2(((float) x) + calcX, (-(float) y) + calcY) * (float) factor;
            particles->push_back(p);
        }
    }

}

void
FluidSolver::MultipleHoleScenario::addDead(std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> *particles,
                                           float restDensity) {
    float mass = restDensity * GetParticleSize() * GetParticleSize();

    FluidSolver::SimpleParticleCollection::FluidParticle p;
    p.Position = glm::vec2(0.0f, 0.0f) * (float) factor;
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = restDensity;
    p.Mass = mass;
    p.Type = FluidSolver::IParticleCollection::ParticleTypeDead;

    for (int i = 0; i < factor; i++) {
        particles->push_back(p);
    }
}

float FluidSolver::MultipleHoleScenario::GetParticleSize() {
    return 1.0f;
}

std::string FluidSolver::MultipleHoleScenario::GetName() {
    return "Multiple holes & sources";
}
