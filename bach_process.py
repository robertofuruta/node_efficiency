import os
import glob
from cd import cd

with cd('test-data-eff'):
    edgelist_list = glob.glob('*edgelist')
    print(edgelist_list)
    for edgelist in edgelist_list:
        os.system(f'../node_eff {edgelist}')
        prefix = edgelist.split('.')[0]
        os.system(f'../compare-expected-trab-1 {prefix}.eff')
