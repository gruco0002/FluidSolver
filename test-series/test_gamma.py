import argparse
import os

import framework.test_series as test_series


def create_data():
    param_gamma2 = test_series.Parameter(
        ["solver", "gamma"], [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5])

    param_gamma1 = test_series.Parameter(["solver", "single-layer-settings", "gamma-1"], [
                                         0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5])

    runner = test_series.TestSeriesRunner(
        "./../cmake-build-relwithdebinfo/FluidConsole", "test_gamma.json", [param_gamma1, param_gamma2], simulation_length=10.0)
    runner.evaluate()


def analyze_data():
    analyzer = test_series.TestSeriesSensorAnalyzer(
        "./output/instance_docs.json", ["iisph.sensor", "particleCount.sensor"])

    gamma1 = []
    gamma2 = []
    avg_iteration_count = []
    valid_simulation = []

    for instance_info, sensor_readers in analyzer.get_instances():

        iisph_sensor, particle_count_sensor = sensor_readers

        gamma1.append(instance_info["solver.single-layer-settings.gamma-1"])
        gamma2.append(instance_info["solver.gamma"])

        mean_iter_count = iisph_sensor.get_data_mean("Last Iteration Count")
        max_inactive_particles = particle_count_sensor.get_data_max(
            "Inactive Particles")

        avg_iteration_count.append(mean_iter_count)
        if max_inactive_particles > 0:
            valid_simulation.append(False)
        else:
            valid_simulation.append(True)

    print(list(zip(gamma1, gamma2, avg_iteration_count, valid_simulation)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--create", help="Create the test data by launching all simulations.", action="store_true")
    parser.add_argument(
        "--analyze", help="Analyze the test data using the previously created simulation data.", action="store_true")

    args = parser.parse_args()

    if args.create:
        create_data()

    if args.analyze:
        analyze_data()

    if not args.analyze and not args.create:
        parser.print_usage()
