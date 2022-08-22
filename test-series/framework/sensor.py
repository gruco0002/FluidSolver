import json


class SensorReader:

    def __init__(self, filepath: str) -> None:
        self.filepath = filepath

        self._indexed_data = {}
        self._definitions = []
        self._field_name_to_index = {}
        self._sensor_name = ""
        self._timestep_data = []

        self._read_file()

    def get_data(self, field_name: str) -> list:
        index = self._get_index_from_fieldname(field_name)
        return self._indexed_data[index][:]

    def get_data_mean(self, field_name: str) -> float:
        index = self._get_index_from_fieldname(field_name)

        avg = 0.0
        data = self._indexed_data[index]
        for value in data:
            avg += value

        if len(data) > 0:
            avg /= len(data)

        return avg

    def get_data_max(self, field_name: str) -> float:
        pass  # TODO: implement

    def get_name(self):
        return self._sensor_name

    def _get_index_from_fieldname(self, field_name: str) -> int:
        if field_name not in self._field_name_to_index:
            print(self._field_name_to_index)
            raise Exception(
                "Could not find specified fieldname: " + field_name)

        return self._field_name_to_index[field_name]

    def _read_file(self):
        with open(self.filepath, "r") as f:
            for line in f:
                line_data = json.loads(line)
                if type(line_data) is dict:
                    # we found the definition object
                    self._read_definition_object(line_data)
                elif type(line_data) is list:
                    # we found a "normal" entry
                    self._read_sensor_entry(line_data)

    def _read_definition_object(self, obj: dict):
        self._sensor_name = obj["name"]

        # reset other values
        self._indexed_data = {}
        self._definitions = []
        self._field_name_to_index = {}
        self._timestep_data = []

        for i, definition in enumerate(obj["definitions"]):
            self._definitions.append(definition)
            self._field_name_to_index[definition["fieldName"]] = i
            self._indexed_data[i] = []

    def _read_sensor_entry(self, entry: list):
        assert(len(self._definitions) + 1 == len(entry))

        for i, value in enumerate(entry):
            if i == 0:
                # timestep entry
                self._timestep_data.append(value)
            else:
                # value entry
                index = i - 1
                self._indexed_data[index].append(value)
