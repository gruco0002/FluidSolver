#! python3
from matplotlib import pyplot as plt
from matplotlib import style
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.colors as colors
import numpy as np
import statistics

MAX_DEAD_PARTICLES = 15


class LogFile:

    def __init__(self, filename, stiffness, viscosity, timestep):
        self.filename = filename
        self.stiffness = stiffness
        self.viscosity = viscosity
        self.timestep = timestep

        # init yet unused stuff
        self.timesteps = []
        self.avgDensity = []
        self.relEnergy = []
        self.potentialEnergy = []
        self.maxVelocity = []
        self.cflNumber = []
        self.deadParticles = []

    def __str__(self):
        return "LogFile(filename=" + self.filename + ", stiffness=" + str(self.stiffness) + ", viscosity=" + str(self.viscosity) + ", timestep=" + str(self.timestep) + ")"

    def __repr__(self):
        return str(self)

    def read_contents(self):
        self.timesteps, self.avgDensity, self.relEnergy, self.kineticEnergy, self.potentialEnergy, self.maxVelocity, self.cflNumber, self.deadParticles = np.loadtxt(
            self.filename, unpack=True, delimiter=";", skiprows=1, converters={7: lambda deadPartString: float(int(deadPartString))})

    def get_max_dead_particles(self):
        return max(self.deadParticles)

    def get_meam_cfl_number(self):
        return statistics.mean(self.cflNumber)

    def is_bad_simulation(self):
        if self.get_max_dead_particles() > MAX_DEAD_PARTICLES:
            return True
        return False

    def get_mean_max_velocity(self):
        return statistics.mean(self.maxVelocity)

    def get_mean_avg_density(self):
        return statistics.mean(self.avgDensity)
    
    def get_mean_kinetic_energy(self):
        return statistics.mean(self.kineticEnergy)


class LogFileDesc:

    def __init__(self, filename):
        self.filename = filename

        self.log_files = []

    def read_contents(self):
        self.log_files.clear()
        with open(self.filename, "r") as f:
            for line in f:
                if not line.strip():
                    continue  # empty line
                tmpFilename, tmpStiffness, tmpViscosity, tmpTimestep = line.split(
                    "\t")

                # edit stuff
                tmpFilename = tmpFilename[0:-1]  # delete last character: ':'
                tmpStiffness = tmpStiffness.replace("stiffness=", "")
                tmpViscosity = tmpViscosity.replace("viscosity=", "")
                tmpTimestep = tmpTimestep.replace("timestep=", "")

                # parse stuff
                stiffness = float(tmpStiffness)
                viscosity = float(tmpViscosity)
                timestep = float(tmpTimestep)

                # generate log file
                logFile = LogFile(tmpFilename, stiffness, viscosity, timestep)
                self.log_files.append(logFile)

    def get_log_files(self):
        return self.log_files


def plot_avg_density_timestep(good):
    timestepChangeSameStiffness = list([(a, b, c, d) for a in good for b in good for c in good for d in good if len(set([a.timestep, b.timestep, c.timestep, d.timestep])) ==
                                        4 and a.viscosity == b.viscosity == c.viscosity == d.viscosity and a.stiffness == b.stiffness == c.stiffness == d.stiffness and a.timestep < b.timestep < c.timestep < d.timestep])

    style.use('seaborn-whitegrid')

    for s in timestepChangeSameStiffness:
        a, b, c, d = s
        if a.stiffness == 10000.0 and a.viscosity == 3.0:
            for x in s:
                plt.plot(x.timesteps, x.avgDensity,
                         label="Δt = " + str(x.timestep))
            print(str(a), str(b), str(c), str(d), "", sep='\n')

    plt.legend()
    plt.xlabel("Zeit (s)")
    plt.ylabel("Durchschnittliche Dichte")
    plt.show()


def plot_avg_density_stiffness(good):
    stiffnessChangeSameTimestep = list([(a, b, c, d) for a in good for b in good for c in good for d in good if len(set([a.stiffness, b.stiffness, c.stiffness, d.stiffness])) ==
                                        4 and a.viscosity == b.viscosity == c.viscosity == d.viscosity and a.timestep == b.timestep == c.timestep == d.timestep and a.stiffness < b.stiffness < c.stiffness < d.stiffness])

    style.use('seaborn-whitegrid')

    for s in stiffnessChangeSameTimestep:
        a, b, c, d = s
        if a.timestep == 0.001 and a.viscosity == 3.0:
            for x in s:
                plt.plot(x.timesteps, x.avgDensity,
                         label="k = " + str(x.stiffness))

            print(str(a), str(b), str(c), str(d), "", sep='\n')

    plt.legend()
    plt.xlabel("Zeit (s)")
    plt.ylabel("Durchschnittliche Dichte")
    plt.show()


def plot_average_cfl_viscosity(good):

    #good.sort(key=lambda log: log.get_meam_cfl_number())
    #for g in good:
    #    print(str(g), "cfl=" + str(g.get_meam_cfl_number()))

    style.use('seaborn-whitegrid')

    cfls = []
    viscosities = []
    timesteps = []
    stiffness = []
    max_vel = []
    kin_ener = []
    avg_dens = []
    for g in good:
        if g.stiffness != 10000.0:
            continue
        cfls.append(g.get_meam_cfl_number())
        viscosities.append(g.viscosity)
        timesteps.append(g.timestep)
        stiffness.append(g.stiffness)
        max_vel.append(g.get_mean_max_velocity())
        kin_ener.append(g.get_mean_kinetic_energy())
        avg_dens.append(g.get_mean_avg_density())

    scatt = plt.scatter(timesteps, cfls, c=avg_dens, cmap=cm.viridis, norm=colors.PowerNorm(gamma=1.0/3.0))
    plt.xlabel("Zeitschritt $\\Delta t$")
    plt.ylabel("CFL-Zahl $c$")
    cbar = plt.colorbar(scatt)
    cbar.set_label("Durchschnittliche Dichte")

    plt.axis([-0.001, 0.011, 0.0, 0.14])
    plt.show()


def process_logs(logs):
    good = []
    for log in logs:
        if not log.is_bad_simulation():
            good.append(log)

    print("All good data was determined")

    plot_average_cfl_viscosity(good)


def main():
    print("Start Reading")
    desc = LogFileDesc("logDesc.csv")
    desc.read_contents()
    logs = desc.get_log_files()
    for log in logs:
        log.read_contents()
    print("Read everything, started processing")
    process_logs(logs)


if __name__ == "__main__":
    main()
