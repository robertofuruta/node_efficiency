import os
import glob
from cd import cd


class cd:
    """Context manager for changing the current working directory"""

    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


with cd('test-data-eff'):
    edgelist_list = glob.glob('*edgelist')
    print(edgelist_list)
    for edgelist in edgelist_list:
        os.system(f'../node_eff_no_list {edgelist}')
        prefix = edgelist.split('.')[0]
        os.system(f'../compare-expected-trab-1 {prefix}.eff')
