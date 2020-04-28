#ifndef FLUIDSOLVER_NEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_NEIGHBORHOODSEARCH_HPP

class FluidSolverWindow;

namespace FluidUI {
    class NeighborhoodSearch {

    public:
        FluidSolverWindow *window = nullptr;

        explicit NeighborhoodSearch(FluidSolverWindow *window);

        void Run(bool *viewNeighborhoodSearch);

    private:

        enum NeighborhoodSearchType{
            SearchCompactHashing = 0,
            SearchHashed = 1,
        };

        NeighborhoodSearchType currentType ;

        static const char* names[2];

        bool isInit = false;
        void Init();

        void CheckChange();

    };
}

#endif //FLUIDSOLVER_NEIGHBORHOODSEARCH_HPP
