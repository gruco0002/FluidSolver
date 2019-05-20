//
// Created by gruberc on 20.05.19.
//


#include "HelperFunctions.hpp"

std::vector<glm::vec2> GetSampleGrid(float start, float step, float stop) {

    std::vector<glm::vec2> pos;

    for (float y = start; y <= stop; y += step) {
        for (float x = start; x <= stop; x += step) {
            pos.push_back(glm::vec2(x, y));
        }
    }


    return pos;
}


bool Exists(FluidSolver::Neighbors val, uint32_t search) {
    for (uint32_t &i : val) {
        if (i == search) {
            return true;
        }
    }
    return false;
}