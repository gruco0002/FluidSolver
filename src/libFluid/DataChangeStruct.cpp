#include "DataChangeStruct.hpp"
namespace FluidSolver {

    bool DataChangeStruct::has_data_changed() const {
        return data_has_changed;
    }
    void DataChangeStruct::notify_that_data_changed() {
        data_has_changed = true;
    }
    void DataChangeStruct::acknowledge_data_change() {
        data_has_changed = false;
    }

} // namespace FluidSolver
