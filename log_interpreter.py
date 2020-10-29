import matplotlib.pyplot as plt
import numpy as np

with open("bach_process.log", "r") as f:
    text = f.read()
    no_mach_text = text.split('\n')
    N = []
    edges = []
    model = []
    eff = []
    print(len(no_mach_text))
    for i in range(len(no_mach_text)//4):
        label_i = no_mach_text[i*4].split('_')
        N.append(int(label_i[2]))
        k = int(label_i[4])
        edges.append(N[i]*k//2)

        model.append(N[i]+edges[i])
        eff.append(float(no_mach_text[i*4+1]))
    # print(label)

plt.scatter(N, eff)
plt.xlabel('number of nodes')
plt.ylabel('time(s)')
plt.show()


plt.scatter(edges, eff)
plt.xlabel('number of edges')
plt.ylabel('time(s)')
plt.title('time(s) x #edges')
plt.show()

plt.scatter(model, eff)
plt.xlabel('number of nodes + number of edges')
plt.ylabel('time(s)')
plt.title('time(s) x #nodes+#edges')
plt.show()
