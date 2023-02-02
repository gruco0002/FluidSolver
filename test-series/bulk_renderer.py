import os
import sys

import framework.test_series as test_series


def create_data(simulation_file):

    simulation_dir = os.path.dirname(simulation_file)

    files = []
    for filename in os.listdir(simulation_dir):
        if filename.endswith(".data"):
            files.append(filename)

    files.sort(key=lambda x: int(x.replace(".data", "")))

    param_input_data = test_series.Parameter(
        ["scenario", "particles"], files)

    runner = test_series.RenderSeriesRunner(
        "./../cmake-build-relwithdebinfo/FluidConsole", simulation_file, [param_input_data], output_directory="./renderOutput")
    runner.evaluate()


if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Error, missing argument!")
        exit(-1)

    simulation_file = sys.argv[1]
    create_data(simulation_file)
