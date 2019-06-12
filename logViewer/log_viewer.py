#! python3
from matplotlib import pyplot as plt
from matplotlib import style
import numpy as np


def main(filepaths, showDens, showEner, showKinetic, showPotential, showVeloc, showCFL, showDead):

    style.use('ggplot')

    for filepath in filepaths:

        timesteps, avgDensity, relEnergy, kineticEnergy, potentialEnergy, maxVelocity, cflNumber, deadParticles = np.loadtxt(
            filepath, unpack=True, delimiter=";", skiprows=1, converters={7: lambda deadPartString: float(int(deadPartString))})

        if showDens:
            plt.plot(timesteps, avgDensity, label="Avg. Density " + filepath)

        if showEner:
            plt.plot(timesteps, relEnergy, label="Rel. Energy " + filepath)

        if showKinetic:
            plt.plot(timesteps, kineticEnergy, label="Kin. Energy " + filepath)

        if showPotential:
            plt.plot(timesteps, potentialEnergy,
                     label="Pot. Energy " + filepath)

        if showVeloc:
            plt.plot(timesteps, maxVelocity, label="Max. Velocity " + filepath)

        if showCFL:
            plt.plot(timesteps, cflNumber, label="CFL Number " + filepath)

        if showDead:
            plt.plot(timesteps, deadParticles,
                     label="Dead Particles " + filepath)

    plt.legend()
    plt.show()


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("logfiles", help="filepath of the logfile(s)",
                        type=str, nargs="+")
    parser.add_argument(
        "--show-energy", help="shows the relative energy graph", action='store_true', dest="showEner")
    parser.add_argument("--show-max-velocity",
                        help="shows the maximal velocity graph", action='store_true', dest="showVeloc")
    parser.add_argument(
        "--show-avg-density", help="shows the average density graph", action='store_true', dest="showDens")
    parser.add_argument(
        "--show-kin-energy", help="shows the kinetic energy graph", action='store_true', dest="showKinetic")
    parser.add_argument(
        "--show-pot-energy", help="shows the potential energy graph", action='store_true', dest="showPotential")
    parser.add_argument(
        "--show-cfl-number", help="shows the cfl number graph", action='store_true', dest="showCFL")
    parser.add_argument(
        "--show-dead-particles", help="shows the dead particles graph", action='store_true', dest="showDead")
    args = parser.parse_args()
    main(args.logfiles, args.showDens, args.showEner, args.showKinetic,
         args.showPotential, args.showVeloc, args.showCFL, args.showDead)
