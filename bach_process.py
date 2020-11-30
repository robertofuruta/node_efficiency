import os
import glob


class cd:
    """Context manager for changing the current working directory"""

    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


paralell_bool = int(input(
    'input 0 for sequential processing, 1 for parallel processing with OpenMP\n'))
if paralell_bool:
    num_threads = int(input('please enter the number of threads\n'))

with cd('redes-grandes'):
    # with cd('test-data-eff'):
    edgelist_list = glob.glob('*edgelist')
    print(edgelist_list)
    for edgelist in edgelist_list:
        if paralell_bool:
            os.system(
                f'../node_eff_parallel_array_openmp {edgelist} {num_threads}')
        else:
            os.system(f'../node_eff_sequential_array {edgelist}')
        # prefix = edgelist.split('.')[0]
        # os.system(f'../compare-expected-trab-1 {prefix}.eff')
