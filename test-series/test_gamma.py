import test_series


def main():

    param_gamma = test_series.Parameter(
        ["solver", "gamma"], [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3])

    runner = test_series.TestSeriesRunner(
        "./../cmake-build-relwithdebinfo/FluidConsole", "test_gamma.json", [param_gamma], simulation_length=5.0)
    runner.evaluate()


if __name__ == "__main__":
    main()
