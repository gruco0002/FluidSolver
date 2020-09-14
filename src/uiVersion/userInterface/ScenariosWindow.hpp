#ifndef FLUIDSOLVER_SCENARIOSWINDOW_HPP
#define FLUIDSOLVER_SCENARIOSWINDOW_HPP

#include <vector>


class FluidSolverWindow;
class ScenariosWindow{
public:
    FluidSolverWindow* window = nullptr;

    void render();

    void initialize();

    ~ScenariosWindow();

private:
    std::vector<char*> scenarios;

    bool initialized = false;

    void update_path();

    void reload();



} ;


#endif //FLUIDSOLVER_SCENARIOSWINDOW_HPP
