import json
import os
import subprocess

from framework.sensor import SensorReader


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

    def path_as_string(self):
        return ".".join(self.parameter_path)


class TestSeriesRunner:

    def __init__(self, executable: str, base_file: str, parameters: list, simulation_length=60.0, output_directory="./output"):
        self.executable = executable
        self.base_file = base_file
        self.parameters = parameters
        self.simulation_length = simulation_length
        self.output_directory = output_directory

        self._instance_counter = 0
        self._documentation = []

        os.makedirs(self.output_directory)

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

        # reset so that next iteration invoked from one level up can start
        param.reset()

    def _create_file(self, new_filename: str):
        # open existing file
        with open(self.base_file, "r") as stream:
            data = json.load(stream)

        # set params accordingly
        for param in self.parameters:
            # get object
            obj = data
            for i in range(len(param.parameter_path) - 1):
                obj = obj[param.parameter_path[i]]

            # set value
            obj[param.parameter_path[-1]] = param.current()

        # correct the data path to avoid copying the particle data
        data["scenario"]["particles"] = "../" + data["scenario"]["particles"]

        # write to new file
        with open(new_filename, "w+") as stream:
            stream.write(json.dumps(data, indent=4))

    def _create_param_documentation(self) -> list:
        res = {"instanceId": self._instance_counter}
        for param in self.parameters:
            res[param.path_as_string()] = param.current()
        return res

    def _create_output_directory(self) -> str:
        result = self.output_directory + "/i" + \
            str(self._instance_counter) + "/"
        os.makedirs(result)
        return result

    def _run_instance(self):
        new_filepath = self.output_directory + "/i" + \
            str(self._instance_counter) + ".json"
        self._create_file(new_filepath)
        output_directory = self._create_output_directory()

        self._documentation.append(self._create_param_documentation())

        self._run_simulation(new_filepath, output_directory)

        self._instance_counter += 1

    def _run_simulation(self, final_file, output_directory):
        subprocess.call((self.executable, "-f", "" + final_file + "", "-o",
                        output_directory, "-l", str(self.simulation_length)), cwd=os.getcwd())

    def _save_documentation(self):
        with open(self.output_directory + "/instance_docs.json", "w+") as stream:
            stream.write(json.dumps(self._documentation, indent=4))

    def evaluate(self):
        assert(self._instance_counter == 0)
        self._param_recurse()
        self._save_documentation()


class TestSeriesSensorAnalyzer:

    def __init__(self, documentation_path: str, sensor_filename: str) -> None:
        self.documentation_path = documentation_path
        self.sensor_filename = sensor_filename

        self._sensor_readers = []
        self._instance_information = []

        self._read_files()

    def get_instances(self) -> list:
        return zip(self._instance_information, self._sensor_readers)

    def _read_files(self):
        with open(self.documentation_path, "r") as f:
            documentation_data = json.load(f)
            for instance in documentation_data:
                self._instance_information.append(instance)
                reader = self._read_instance_sensor(instance["instanceId"])
                self._sensor_readers.append(reader)

    def _read_instance_sensor(self, instanceId: int) -> SensorReader:
        dir = os.path.dirname(self.documentation_path)
        path = os.path.join(dir, "i" + str(instanceId), self.sensor_filename)

        return SensorReader(path)
