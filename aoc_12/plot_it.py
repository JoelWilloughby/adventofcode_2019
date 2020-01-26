import csv
import matplotlib.pyplot as plt

xs = [[], [], [], []]
vs = [[], [], [], []]
with open('temp') as csv_file:
    csv_reader = csv.reader(csv_file)
    for row in csv_reader:
        for i in range(4):
            xs[i].append(int(row[2*i]))
            vs[i].append(int(row[2*i+1]))

fig, ax = plt.subplots()
xx = range(len(xs[0]))
for i in range(1):
    x = xs[i]
    v = vs[i]
    ax.plot(xx, x, label=f'moon {i} x')
    ax.plot(xx, v, label=f'moon {i} v')
#ax.set_yticks(ax.get_yticks()[::10])
ax.legend()
ax.hlines([0.0], 0, len(xs[0]))
plt.show()

