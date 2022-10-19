#pragma once

namespace LibFluid {

    /**
     * Provides an interface / pattern for a cleaner codebase.
     */
    class Initializable {
      public:
        /**
         * "initialize" should be called before each timestep calculation and before all other
         * things that rely on parameters being set inside the component or propagated down
         * to other components like for instance the "create_compatibility_report".
         *
         * The function should thereby only do work if necessary, meaning if nothing changed
         * since the last call to initialize no work should be done. The overall footprint
         * of calling initialize should therefore be very tiny.
         */
        virtual void initialize() = 0;

        virtual ~Initializable() = default;
    };


} // namespace FluidSolver