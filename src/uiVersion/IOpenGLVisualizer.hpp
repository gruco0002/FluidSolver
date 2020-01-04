//
// Created by corbi on 04.01.2020.
//

#ifndef FLUIDSOLVER_IOPENGLVISUALIZER_HPP
#define FLUIDSOLVER_IOPENGLVISUALIZER_HPP

#include <engine/graphics/Texture2D.hpp>

class IOpenGLVisualizer {

public:
    virtual Engine::Graphics::Texture2D *GetTexture() = 0;

};

#endif //FLUIDSOLVER_IOPENGLVISUALIZER_HPP
