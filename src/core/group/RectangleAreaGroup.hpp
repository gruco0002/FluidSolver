#ifndef FLUIDSOLVER_RECTANGLEAREAGROUP_HPP
#define FLUIDSOLVER_RECTANGLEAREAGROUP_HPP

#include "../FluidInclude.hpp"
#include "../fluidSolver/ParticleCollection.hpp"

namespace FluidSolver {

    class RectangleAreaGroup {
    public:
        struct Rectangle {
            pFloat left;
            pFloat right;

            pFloat bottom;
            pFloat top;
        } area = {};

        bool inside = false;

        ParticleCollection *collection = nullptr;

        bool is_inside(pIndex_t index) const;

        bool is_member(pIndex_t index) const;

    public:

        struct RectangleAreaGroupIterator;

        // iterator defines
        using T = pIndex_t;
        using iterator = RectangleAreaGroupIterator;
        using const_iterator = RectangleAreaGroupIterator;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;

        struct RectangleAreaGroupIterator {
            RectangleAreaGroup *data;
            T current;

            bool operator==(const RectangleAreaGroupIterator &other) const;

            bool operator!=(const RectangleAreaGroupIterator &other) const;

            T &operator*();

            RectangleAreaGroupIterator &operator++();

            const RectangleAreaGroupIterator operator++(int);

        };

        RectangleAreaGroupIterator begin();

        RectangleAreaGroupIterator end();


    };


}


#endif //FLUIDSOLVER_RECTANGLEAREAGROUP_HPP
