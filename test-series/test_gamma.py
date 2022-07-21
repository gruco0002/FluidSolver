import test_series


def main():

    param_gamma = test_series.Parameter(
        ["solver", "gamma"], list(range(0.0, 2.0, 0.1)))

    runner = test_series.TestSeriesRunner(
        "./../cmake-build-relwithdebinfo/FluidConsole", "test_gamma.yaml", [param_gamma], simulation_length=5.0)
    runner.evaluate()


if __name__ == "__main__":
    main()
