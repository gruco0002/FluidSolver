#pragma once

namespace FluidSolver {

    // TODO: find a better name
    struct DataChangeStruct {
      private:
        bool data_has_changed = true;

      public:
        /**
         * States if data of this struct (i.e. the struct / class that inherited from this one)
         * changed.
         */
        bool has_data_changed() const;

        /**
         * Its the programmers own responsibility to state whether data of this class/struct
         * was altered by him.
         * If that was the case this function has to be called in order for other tools
         * (i.e. the data owning / managing class) to recognize the change.
         * This allows for direct access (without getters and setters) whilst avoiding unnecessary
         * checks or rebuilds of structures when there are no changes.
         *
         * It is safe to call notify_that_data_changed() no matter the result of has_data_changed().
         *
         * This method should only be called if a member of this struct changed and not if its content
         * (i.e. the members of the member) changed.
         */
        void notify_that_data_changed();

        /**
         * If the changes are checked and actions are taken the data owning / managing tool can
         * acknowledge the changes and set the has_data_changed flag back to false.
         */
        void acknowledge_data_change();
    };
} // namespace FluidSolver