#! python3
import subprocess


class ParameterValue:

    def __init__(self, name, description, value):
        self.name = name
        self.description = description
        self.value = value

    def __str__(self):
        return self.name + "=" + str(self.value)


class ParameterRange:

    def __init__(self, name, description, startValue, endValue, stepSize):
        self.description = description
        self.name = name
        self.startValue = startValue
        self.endValue = endValue
        self.stepSize = stepSize
        self._currentValue = self.startValue

    def reset(self):
        self._currentValue = self.startValue

    def next_step(self):
        self._currentValue += self.stepSize
        self._currentValue = min(self._currentValue, self.endValue)

    def get_current_value(self):
        return ParameterValue(self.name, self.description, self._currentValue)

    def is_at_end(self):
        if self._currentValue == self.endValue:
            return True
        return False

class ParameterRangeExponential(ParameterRange):

    def next_step(self):
        self._currentValue *= self.stepSize
        self._currentValue = min(self._currentValue, self.endValue) 


def walk_through_matrix(parameterRanges, fnc):
    """ Walks through the given list of parameters generating every possible
        combination

    Args:
        parameterRanges (list of ParameterRange): The parameters to walk
                                                  through
        fnc (Function): Takes a list of ParameterValue as argument. Will be
                        called every time a new combination is generated
    """
    # reset everything
    for p_range in parameterRanges:
        p_range.reset()

    if len(parameterRanges) == 0:
        return

    # call the function initally
    fnc(list([x.get_current_value() for x in parameterRanges]))

    # iterate over every possible combination
    while not parameterRanges[0].is_at_end():

        # increment the value
        for p_range in reversed(parameterRanges):
            p_range.next_step()
            # check if we reached the end
            if not p_range.is_at_end():
                break  # we do not want to increment values any further
            else:
                p_range.reset()  # reset value, increment next values

        # call the function
        fnc(list([x.get_current_value() for x in parameterRanges]))


def generate_parameter_list(params):
    ret = []
    for param_value in params:
        ret.append("--" + param_value.name + "=" + str(param_value.value))
    return ret


def call_fluid_solver(executable_path, log_name, params):
    call_list = [executable_path, "-c", "-l 30.0" "--output=" + log_name]
    param_list = generate_parameter_list(params)
    p = subprocess.Popen(call_list + param_list, bufsize=0 , stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    p.wait()


def run_for_all(params, executable_path, log_description_path, log_prefix):

    with open(log_description_path, "w+", buffering=1) as log_desc:

        current_number = 0

        def inside(param_values):
            nonlocal current_number
            current_number += 1
            # save config
            log_desc.write(log_prefix + str(current_number) + ".csv:\t" +
                           "\t".join([str(x) for x in param_values]) + "\n")
            # print it
            print("Starting", list([str(x) for x in param_values]))

            # call fluid solver
            call_fluid_solver(executable_path, log_prefix +
                              str(current_number) + ".csv", param_values)

        walk_through_matrix(params, inside)


def main(executable_path, log_description_path, log_prefix):
    params = [ParameterRangeExponential("timestep", "Timestep", 0.0001, 0.1, 10),
              ParameterRange("stiffness", "Stiffness",
                             1000.0, 1000000.0, 1000.0),
              ParameterRange("viscosity", "Viscosity", 0.1, 10.0, 0.1)]
    
    run_for_all(params, executable_path, log_description_path, log_prefix)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("path", help="filepath of the fluid solvers executable",
                        type=str)
    parser.add_argument("--log-description", help="Sets the log description file path",
                        type=str, default='logDesc.csv', dest="log_description_path")
    parser.add_argument("--log-prefix", help="Sets the logs prefix",
                        type=str, default='log', dest="log_prefix")

    args = parser.parse_args()

    main(args.path, args.log_description_path, args.log_prefix)
