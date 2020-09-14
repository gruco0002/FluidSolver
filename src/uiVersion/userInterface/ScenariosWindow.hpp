#ifndef FLUIDSOLVER_SCENARIOSWINDOW_HPP
#define FLUIDSOLVER_SCENARIOSWINDOW_HPP

#include <vector>
#include <string>

class FluidSolverWindow;
class ScenariosWindow{
public:
    FluidSolverWindow* window = nullptr;

    void render();

    void initialize();

    ~ScenariosWindow();

private:
    std::vector<char*> scenarios;

    std::string path = "../scenarios/";

    int current_item = 0;

    bool initialized = false;

    void update_path();

    void load();



} ;


#endif //FLUIDSOLVER_SCENARIOSWINDOW_HPP
