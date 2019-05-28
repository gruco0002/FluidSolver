#! python3
from matplotlib import pyplot as plt
from matplotlib import style
import numpy as np


def main(filepaths, showDens, showEner, showVeloc):

    style.use('ggplot')

    for filepath in filepaths:
        timesteps, avgDensity, relEnergy, maxVelocity = np.loadtxt(
            filepath, unpack=True, delimiter=";", skiprows=1)

        if showDens:
            plt.plot(timesteps, avgDensity, label="Avg. Density " + filepath)
        
        if showEner:
            plt.plot(timesteps, relEnergy, label="Rel. Energy " + filepath)

        if showVeloc:
            plt.plot(timesteps, maxVelocity, label="Max. Velocity " + filepath)

    plt.legend()
    plt.show()


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("logfiles", help="filepath of the logfile(s)",
                        type=str, nargs="+")
    parser.add_argument(
        "--show-energy", help="shows the relative energy graph", action='store_true', dest="showEnerg")
    parser.add_argument("--show-max-velocity",
                        help="shows the maximal velocity graph", action='store_true', dest="showVeloc")
    parser.add_argument(
        "--hide-avg-density", help="hides the average density graph", action='store_true', dest="hideDens")
    args = parser.parse_args()
    main(args.logfiles, not args.hideDens, args.showEnerg, args.showVeloc)
