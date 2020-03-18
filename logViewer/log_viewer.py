#! python3
from matplotlib import pyplot as plt
from matplotlib import style
import numpy as np


class Names:

    AVERAGE_DENSITY = 1
    ENERGY = 2
    KINETIC_ENERGY = 3
    POTENTIAL_ENERGY = 4
    MAX_VELOCITY = 5
    DEAD_PARTICLES = 6
    NORMAL_PARTICLES = 7
    BOUNDARY_PARTICLES = 8
    CFL_NUMBER = 9
    DIAGONAL_ELEMENT = 10
    SOLVER_ITERATIONS = 11
    PREDICTED_DENSITY_ERROR = 12
    AVERAGE_DENSITY_ERROR = 13


def main(filepaths: list, showLegend: bool, plotData: list):

    # style.use('ggplot')
    style.use('seaborn-whitegrid')

    for filepath in filepaths:
        def conv(deadPartString): return float(int(deadPartString))
        timesteps, avgDensity, relEnergy, maxVelocity, deadParticles, kineticEnergy, potentialEnergy, boundaryParticles, \
            normalParticles, cflNumber, diagonalElement, SolverIterations, predicted_density_error, avg_density_error = np.loadtxt(
                filepath, unpack=True, delimiter=";", skiprows=1, converters={4: conv, 7: conv, 8: conv, 11: conv})

        if Names.AVERAGE_DENSITY in plotData:
            plt.plot(timesteps, avgDensity, label="Avg. Density " + filepath)

        if Names.ENERGY in plotData:
            plt.plot(timesteps, relEnergy, label="Rel. Energy " + filepath)

        if Names.KINETIC_ENERGY in plotData:
            plt.plot(timesteps, kineticEnergy, label="Kin. Energy " + filepath)

        if Names.POTENTIAL_ENERGY in plotData:
            plt.plot(timesteps, potentialEnergy,
                     label="Pot. Energy " + filepath)

        if Names.MAX_VELOCITY in plotData:
            plt.plot(timesteps, maxVelocity, label="Max. Velocity " + filepath)

        if Names.CFL_NUMBER in plotData:
            plt.plot(timesteps, cflNumber, label="CFL Number " + filepath)

        if Names.DEAD_PARTICLES in plotData:
            plt.plot(timesteps, deadParticles,
                     label="Dead Particles " + filepath)
        if Names.NORMAL_PARTICLES in plotData:
            plt.plot(timesteps, normalParticles,
                     label="Normal Particles " + filepath)
        if Names.BOUNDARY_PARTICLES in plotData:
            plt.plot(timesteps, boundaryParticles,
                     label="Boundary Particles " + filepath)
        if Names.DIAGONAL_ELEMENT in plotData:
            plt.plot(timesteps, diagonalElement,
                     label="Diagonal Element " + filepath)
        if Names.SOLVER_ITERATIONS in plotData:
            plt.plot(timesteps, SolverIterations,
                     label="Solver Iterations " + filepath)
        if Names.PREDICTED_DENSITY_ERROR in plotData:
            plt.plot(timesteps, predicted_density_error,
                     label="Predicted Density Error " + filepath)
        if Names.AVERAGE_DENSITY_ERROR in plotData:
            plt.xlabel("Zeit $t$")
            plt.ylabel("Durchschnittlicher Dichtefehler")
            label = "Avg. Density Error " + filepath
            if "iisph" in label:
                label = "IISPH"
            else:
                label = "SESPH"
            plt.plot(timesteps, avg_density_error,
                     label=label)

    if showLegend:
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
    parser.add_argument(
        "--show-density-error", help="shows the avg density error", action='store_true', dest="densError")
    parser.add_argument(
        "--show-iteration-count", help="shows the solvers iteration count", action='store_true', dest="iterCount")
    parser.add_argument(
        "--show-predicted-density-error", help="shows the predicted density error", action='store_true', dest="predDensError")

    parser.add_argument(
        "--hide-legend", help="Hides the graphs legend", action='store_true', dest="hideLegend")
    args = parser.parse_args()

    show = []
    if args.showDens:
        show.append(Names.AVERAGE_DENSITY)
    if args.showEner:
        show.append(Names.ENERGY)
    if args.showKinetic:
        show.append(Names.KINETIC_ENERGY)
    if args.showPotential:
        show.append(Names.POTENTIAL_ENERGY)
    if args.showVeloc:
        show.append(Names.MAX_VELOCITY)
    if args.showCFL:
        show.append(Names.CFL_NUMBER)
    if args.showDead:
        show.append(Names.DEAD_PARTICLES)
    if args.densError:
        show.append(Names.AVERAGE_DENSITY_ERROR)
    if args.iterCount:
        show.append(Names.SOLVER_ITERATIONS)
    if args.predDensError:
        show.append(Names.PREDICTED_DENSITY_ERROR)

    main(args.logfiles, not args.hideLegend, show)
