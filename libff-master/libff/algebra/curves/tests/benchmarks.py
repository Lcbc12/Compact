import pandas as pd
import matplotlib.pyplot as plt


data = pd.read_csv('./results/benchmarks.csv')

# INITIALISATION
plt.figure(figsize=(6.8, 4.2))
plt.rcParams["figure.autolayout"] = True

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot
ax.plot(data.s, data.init_time, color="red", marker="o")

# set x-axis label
ax.set_xlabel("s",fontsize=14)
# set y-axis label
ax.set_ylabel("seconds",color="red",fontsize=14)

# twin object for two different y-axis on the sample plot
ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot(data.s, data.init_size,color="blue",marker="o")
ax2.set_ylabel("octets",color="blue",fontsize=14)

fig.savefig('./results/graphs/initialisation_step.jpg',
            format='jpeg',
            dpi=100,
            bbox_inches='tight')

# SIGNING
plt.figure(figsize=(6.8, 4.2))
plt.rcParams["figure.autolayout"] = True

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot
ax.plot(data.s, data.signing_time, color="red", marker="o")

# set x-axis label
ax.set_xlabel("s",fontsize=14)
# set y-axis label
ax.set_ylabel("seconds",color="red",fontsize=14)

# twin object for two different y-axis on the sample plot
ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot(data.s, data.signing_size,color="blue",marker="o")
ax2.set_ylabel("octets",color="blue",fontsize=14)

fig.savefig('./results/graphs/signing_step.jpg',
            format='jpeg',
            dpi=100,
            bbox_inches='tight')

# CHALLENGING
plt.figure(figsize=(6.8, 4.2))
plt.rcParams["figure.autolayout"] = True

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot
ax.plot(data.s, data.challenging_time, color="red", marker="o")

# set x-axis label
ax.set_xlabel("s",fontsize=14)
# set y-axis label
ax.set_ylabel("seconds",color="red",fontsize=14)

# twin object for two different y-axis on the sample plot
ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot(data.s, data.challenging_size,color="blue",marker="o")
ax2.set_ylabel("octets",color="blue",fontsize=14)

fig.savefig('./results/graphs/challenging_step.jpg',
            format='jpeg',
            dpi=100,
            bbox_inches='tight')


# PROVING
plt.figure(figsize=(6.8, 4.2))
plt.rcParams["figure.autolayout"] = True

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot
ax.plot(data.s, data.proving_time, color="red", marker="o")

# set x-axis label
ax.set_xlabel("s",fontsize=14)
# set y-axis label
ax.set_ylabel("seconds",color="red",fontsize=14)

# twin object for two different y-axis on the sample plot
ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot(data.s, data.proving_size,color="blue",marker="o")
ax2.set_ylabel("octets",color="blue",fontsize=14)

fig.savefig('./results/graphs/proving_step.jpg',
            format='jpeg',
            dpi=100,
            bbox_inches='tight')

# VERIFICATION
plt.figure(figsize=(6.8, 4.2))
plt.rcParams["figure.autolayout"] = True

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot
ax.plot(data.s, data.verif_time, color="red", marker="o")

# set x-axis label
ax.set_xlabel("s",fontsize=14)
# set y-axis label
ax.set_ylabel("seconds",color="red",fontsize=14)

fig.savefig('./results/graphs/verification_step.jpg',
            format='jpeg',
            dpi=100,
            bbox_inches='tight')
