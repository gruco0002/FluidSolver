//
// Created by Corbinian Gruber on 2019-05-06.
//

#include "FluidSolverParticleInfoGUI.hpp"
#include <cppgui/src/Stack.hpp>
#include <cppgui/src/DirectionalSpread.hpp>

FluidSolverParticleInfoGUI::FluidSolverParticleInfoGUI(uint32_t particleIndex, FluidSolver::IParticleCollection *particleCollection) : particleIndex(particleIndex), particleCollection(particleCollection), Panel()
{
    Setup();
    UpdateData();
}

void FluidSolverParticleInfoGUI::UpdateData()
{
    if (particleCollection == nullptr)
        return;
    if (particleCollection->GetSize() <= particleIndex)
        return;
    id->setText(std::to_string(particleIndex));
    auto particlePosition = particleCollection->GetPosition(particleIndex);
    position->setText(std::to_string(particlePosition.x) + ", " + std::to_string(particlePosition.y));
    auto particleVelocity = particleCollection->GetVelocity(particleIndex);
    velocity->setText(std::to_string(particleVelocity.x) + ", " + std::to_string(particleVelocity.y));
    density->setText(std::to_string(particleCollection->GetDensity(particleIndex)));
    pressure->setText(std::to_string(particleCollection->GetPressure(particleIndex)));
    mass->setText(std::to_string(particleCollection->GetMass(particleIndex)));
}

cppgui::Stack *generateInfoStack(std::string text, cppgui::StyledLabel *other)
{
    auto infoStack = new cppgui::Stack(cppgui::StackHorizontal);
    infoStack->setHeight(30);
    infoStack->setGrowWithContent(false);
    auto desc = new cppgui::StyledLabel(text, cppgui::LabelStyle::LabelStyleDescription, 80, 30);
    infoStack->addChild(desc);
    infoStack->addChild(other);
    return infoStack;
}

void FluidSolverParticleInfoGUI::Setup()
{
    auto stack = new cppgui::Stack();
    addChild(new cppgui::DirectionalSpread(stack));

    id = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    position = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    velocity = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    density = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    pressure = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    mass = new cppgui::StyledLabel("", cppgui::LabelStyle::LabelStyleDescription, 200, 30);
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("ID:", id)));
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("Position:", position)));
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("Velocity:", velocity)));
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("Density:", density)));
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("Pressure:", pressure)));
    stack->addChild(new cppgui::DirectionalSpread(generateInfoStack("Mass:", mass)));
}
