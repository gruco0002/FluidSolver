//
// Created by gruberc on 20.05.19.
//

#ifndef FLUIDSOLVER_HELPERFUNCTIONS_HPP
#define FLUIDSOLVER_HELPERFUNCTIONS_HPP

#include <core/interface/INeighborhoodSearch.hpp>

/**
 * Creates a grid of positions starting from start with step size in both axis directions.
 * @param start Inclusive start position.
 * @param step Step size of the positions.
 * @param stop Maximum for the grid value in each axis. Is inclusive if step is exactly stop
 * @return
 * @note Grid is sorted from left to right, then top to bottom.
 */
std::vector<glm::vec2> GetSampleGrid(float start, float step, float stop);

/**
 * Checks if a value exists inside a vector
 * @param val The vector
 * @param search The value to search
 * @return True iff search exists in val
 */
bool Exists(FluidSolver::Neighbors val, uint32_t search);

#endif //FLUIDSOLVER_HELPERFUNCTIONS_HPP
