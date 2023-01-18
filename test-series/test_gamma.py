import argparse
import os

# used for plotting
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
import seaborn as sns
import pandas as pd

plt.rcParams.update({'font.size': 7})

import framework.test_series as test_series


def create_data():
    param_gamma2 = test_series.Parameter(
        ["solver", "gamma"], [0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8])

    param_gamma1 = test_series.Parameter(["solver", "single-layer-settings", "gamma-1"], [
                                         0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5])

    param_timestep = test_series.Parameter(["solver", "timestep", "timestep"], [
                                           0.075, 0.05, 0.025, 0.01])

    runner = test_series.TestSeriesRunner(
        "./../cmake-build-relwithdebinfo/FluidConsole", "test_gamma.json", [param_timestep, param_gamma1, param_gamma2], simulation_length=10.0)
    runner.evaluate()


def analyze_data():
    analyzer = test_series.TestSeriesSensorAnalyzer(
        "./output/instance_docs.json", ["iisph.sensor", "particleCount.sensor", "density.sensor"])

    gamma1 = {}
    gamma2 = {}
    avg_iteration_count = {}
    avg_density_variance = {}
    avg_density_avg = {}

    for instance_info, sensor_readers in analyzer.get_instances():

        timestep = instance_info["solver.timestep.timestep"]
        if timestep not in gamma1:
            gamma1[timestep] = []
            gamma2[timestep] = []
            avg_iteration_count[timestep] = []
            avg_density_variance[timestep] = []
            avg_density_avg[timestep] = []

        iisph_sensor, particle_count_sensor, density_sensor = sensor_readers

        mean_iter_count = iisph_sensor.get_data_mean("Last Iteration Count")

        variance = density_sensor.get_data_sample_variance("Average Density")
        avg_dens = density_sensor.get_data_mean("Average Density")

        max_inactive_particles = particle_count_sensor.get_data_max(
            "Inactive Particles")

        gamma1[timestep].append(
            instance_info["solver.single-layer-settings.gamma-1"])
        gamma2[timestep].append(instance_info["solver.gamma"])
        if max_inactive_particles > 0:
            # invalid simulation
            avg_iteration_count[timestep].append(np.nan)
            avg_density_variance[timestep].append(np.nan)
            avg_density_avg[timestep].append(np.nan)
        else:
            # valid simulation
            avg_iteration_count[timestep].append(mean_iter_count)
            avg_density_variance[timestep].append(variance)
            avg_density_avg[timestep].append(avg_dens)

    _show_plot_average_iteration_count(gamma1, gamma2, avg_iteration_count)
    # _show_plot_density_variance(gamma1, gamma2, avg_density_variance)
    # _show_plot_average_density(gamma1, gamma2, avg_density_avg)


def _show_plot_average_iteration_count(gamma1, gamma2, avg_iteration_count):

    timesteps = sorted(list(gamma1.keys()))

    assert(len(timesteps) % 2 == 0)

    fig, axes = plt.subplots(2, len(timesteps) // 2, figsize=[6.4, 5.8])

    for i, timestep in enumerate(timesteps):

        ax = axes[i // 2][i % 2]

        df = pd.DataFrame.from_dict(
            np.array([gamma1[timestep], gamma2[timestep], avg_iteration_count[timestep]]).T)
        df.columns = ['Gamma 1', 'Gamma 2', 'Average Iteration Count']

        pivotted = df.pivot('Gamma 1', 'Gamma 2', 'Average Iteration Count')

        mask = pivotted.isnull()
        sns.heatmap(pivotted, cmap="magma", ax=ax, mask=mask)
        ax.set_title("Timestep " + str(timestep) + "s")

    # fig.suptitle("Average Iteration Count")
    fig.tight_layout()

    plt.savefig("plot-of-average-iteration-count.pdf")  
    plt.show()

def _show_plot_average_density(gamma1, gamma2, avg_density_avg):

    timesteps = sorted(list(gamma1.keys()))

    fig, axes = plt.subplots(1, len(timesteps))

    for i, timestep in enumerate(timesteps):

        ax = axes[i]

        df = pd.DataFrame.from_dict(
            np.array([gamma1[timestep], gamma2[timestep], avg_density_avg[timestep]]).T)
        df.columns = ['Gamma 1', 'Gamma 2', 'Average Density']

        pivotted = df.pivot('Gamma 1', 'Gamma 2', 'Average Density')

        mask = pivotted.isnull()
        sns.heatmap(pivotted, cmap="viridis", ax=ax, mask=mask)
        ax.set_title("Timestep " + str(timestep) + "s")

    fig.suptitle("Average Density")
    plt.show()

def _show_plot_density_variance(gamma1, gamma2, avg_density_variance):

    max_iter = 100.0
    min_iter = 2.0

    timesteps = sorted(list(gamma1.keys()))

    fig, axes = plt.subplots(1, len(timesteps))

    for i, timestep in enumerate(timesteps):

        ax = axes[i]

        df = pd.DataFrame.from_dict(
            np.array([gamma1[timestep], gamma2[timestep], np.sqrt(avg_density_variance[timestep])]).T)
        df.columns = ['Gamma 1', 'Gamma 2', 'Standard Deviation of Average Density']

        pivotted = df.pivot('Gamma 1', 'Gamma 2',
                            'Standard Deviation of Average Density')

        mask = pivotted.isnull()
        sns.heatmap(pivotted, cmap="viridis", ax=ax, mask=mask, norm=LogNorm())
        ax.set_title("Timestep " + str(timestep) + "s")

    fig.suptitle("Standard Deviation of Average Density")
    plt.show()


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
