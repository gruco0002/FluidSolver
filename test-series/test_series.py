import yaml
import os
import subprocess


class Parameter:

    def __init__(self, parameter_path: list, parameter_values: list):
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

    def __init__(self, executable: str, base_file: str, parameters: list, simulation_length=60.0):
        self.executable = executable
        self.base_file = base_file
        self.parameters = parameters
        self.simulation_length = simulation_length

        self._instance_counter = 0
        self._documentation = []

    def _param_recurse(self, index=0):
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

    def _create_file(self, new_filename: str):
        # open existing file
        with open(self.base_file, "r") as stream:
            data = yaml.safe_load(stream)

        # set params accordingly
        for param in self.parameters:
            # get object
            obj = data
            for i in range(len(param.parameter_path) - 1):
                obj = obj[param.parameter_path[i]]

            # set value
            obj[param.parameter_path[-1]] = param.current()

        # write to new file
        with open(new_filename, "w+") as stream:
            stream.write(yaml.safe_dump(data))

    def _create_param_documentation(self) -> list:
        res = [str(self._instance_counter)]
        for param in self.parameters:
            res += [str(param.current())]
        return res

    def _create_output_directory(self) -> str:
        result = "i" + str(self._instance_counter) + "/"
        os.makedirs(result)
        return result

    def _run_instance(self):
        new_filepath = "i" + str(self._instance_counter) + ".yaml"
        self._create_file(new_filepath)
        output_directory = self._create_output_directory()

        self._documentation.append(self._create_param_documentation())

        self._run_simulation(new_filepath, output_directory)

        self._instance_counter += 1

    def _run_simulation(self, final_file, output_directory):
        subprocess.call((self.executable, "-f " + final_file, "-o " +
                        output_directory, "-l " + str(self.simulation_length)))

    def _save_documentation(self):
        header = ["instance id"]
        for param in self.parameters:
            header.append(" ".join(param.parameter_path))

        document = [header] + self._documentation
        with open("instance_docs.csv", "w+", newline="\r\n") as stream:
            lines = list([";".join(line) for line in document])
            stream.writelines(lines)

    def evaluate(self):
        assert(self._instance_counter == 0)
        self._param_recurse()
        self._save_documentation()
