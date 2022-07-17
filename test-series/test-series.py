
class Parameter:

    def __init__(self, parameter_path: str, parameter_values: list):
        self.parameter_path = parameter_path
        self.parameter_values = parameter_values

        self._current = 0
        assert(len(parameter_values) > 0)

    
    def current(self):
        assert(self._current < len(self.parameter_values))
        return self.parameter_values[self._current]
    
    def next(self):
        self._current += 1
    
    def at_end(self):
        return self._current >= len(self.parameter_values)

    def reset(self):
        self._current = 0




class TestSeriesRunner:

    def __init__(self):
        self.executable = ""
        self.base_file = ""
        self.parameters = []


    def _param_recurse(self, index = 0):
        assert(index < len(self.parameters))
        
        param = self.parameters[index]

        while not param.at_end():            
            if index + 1 < len(self.parameters):
                # there is a parameter left
                self._param_recurse(index + 1)
            else:
                # no parameters left
                self._run_instance()

            param.next()

    
    def _run_instance(self):
        
        # TODO: create file from params

        # TODO: create output directory

        # TODO: run simulation

        pass
    
    def _run_simulation(self, final_file, output_directory):
        pass






