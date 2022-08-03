import subprocess
from pathlib import Path
import math

SOURCE_ROOT = str(Path("..").resolve())
print(SOURCE_ROOT)
NUM_THREADS = 4

def get_main_binary():
    binary = Path(SOURCE_ROOT + "/build/main")
    if not binary.is_file():
        if not Path(SOURCE_ROOT + "/build").is_dir():
            Path(SOURCE_ROOT + "/build").mkdir()
        subprocess.run(['cmake', '..'], cwd=SOURCE_ROOT + "/build")
        subprocess.run(['make', '-j', str(NUM_THREADS)], cwd=SOURCE_ROOT + "/build")
    return binary

def get_oberlin_trace():
    trace_dir = SOURCE_ROOT + "/traces"
    if not Path(trace_dir).is_dir():
        Path(trace_dir).mkdir()
        if not Path(trace_dir + "/traces.zip").is_file():
            subprocess.run(['wget', 'https://occs.oberlin.edu/~ctaylor/classes/210SP13/traces.zip'])
    subprocess.run(['unzip', '-n', 'traces.zip'])
    subprocess.run(['rm', '-rf', '__MACOSX'])
    subprocess.run(['gunzip', '-f', 'traces/art.trace.gz'])
    subprocess.run(['gunzip', '-f', 'traces/mcf.trace.gz'])

def get_ucsd_trace():
    trace_dir = SOURCE_ROOT + "/traces"
    if not Path(trace_dir).is_dir():
        Path(trace_dir).mkdir()
    if not Path("./proj1-traces.tar.gz").is_file():
        subprocess.run(['wget', 'http://cseweb.ucsd.edu/classes/fa07/cse240a/proj1-traces.tar.gz'])
    if not Path('./proj1').is_dir():
        Path('./proj1').mkdir()
    subprocess.run(['tar', '-xf', 'proj1-traces.tar.gz', '--directory', 'proj1'])

def get_result_oberlin(binary, result_dict, trace = 'art', a = 1, b = 64, c = 16384, r = 'L', w = 'B'):
    result_dict['oberlin'] = {}
    if trace == 'art':
        print("For file art.trace, we have: ")
        out = subprocess.check_output([binary, '-f', 'traces/art.trace', '-a', str(a), '-b', str(b), '-c', str(c), '-r', str(r), '-w', str(w),'-s'], cwd=SOURCE_ROOT + '/examples')
        dct = parse(out, result_dict['oberlin'])
    elif trace == 'mcf':
        print("For file mcf.trace, we have: ")
        out = subprocess.check_output([binary, '-f', 'traces/mcf.trace', '-a', a, '-b', b, '-c', c, '-r', str(r), '-w', str(w), '-s'], cwd=SOURCE_ROOT + '/examples') 
    return result_dict['oberlin']

def parse(string, result_dict):
    for line in string.split():
        line = line.decode("utf-8")
        if ':' in line:
            name = line.split(':')[0]
            value = line.split(':')[1]
            result_dict[name] = value

def get_result_oberlin_assoc(binary, result_dict):
    for i in range(6):
        i = 2 ** i
        result_dict[i] = {}
        get_result_oberlin(binary, result_dict[i], a = i)

def get_result_oberlin_capacity(binary, result_dict):
    for i in range(7, 17):
        i = 2 ** i
        result_dict[i] = {}
        get_result_oberlin(binary, result_dict[i], c = i)

def get_result_oberlin_bsize(binary, result_dict):
    for i in range(0, 8):
        i = 2 ** i
        result_dict[i] = {}
        get_result_oberlin(binary, result_dict[i], a = 2, b = i, c = 65536)

def get_result_oberlin_replace(binary, result_dict):
    for i in ['L', 'F']:
        result_dict[i] = {}
        get_result_oberlin(binary, result_dict[i], r = i)

def get_result_ucsd(binary):
    fnames = ['proj1/traces/mcf.trace', 'proj1/traces/swim.trace', 'proj1/traces/gcc.trace']
    print("For file proj1/mcf.trace, we have: ")
    subprocess.run([binary, '-f', fnames[0], '-a', '1', '-b', '16', '-c', '16384', '-t', 'simple', '-s'], cwd=SOURCE_ROOT + '/examples') 
    print("For file proj1/swim.trace, we have: ")
    subprocess.run([binary, '-f', fnames[1], '-a', '8', '-b', '32', '-c', '65536', '-t', 'simple', '-s'], cwd=SOURCE_ROOT + '/examples') 
    print("For file proj1/gcc.trace, we have: ")
    subprocess.run([binary, '-f', fnames[2], '-a', '8', '-b', '32', '-c', '65536', '-t', 'simple', '-s'], cwd=SOURCE_ROOT + '/examples')

def plot(var_x, var_y, var_x_name, var_y_name, title, fname): 
    import numpy as np
    import matplotlib.pyplot as plt
    import re

    workload_list = ['art.trace', 'mcf.trace', 'swim.trace', 'gcc.trace']
    workload_display_dict = {'art.trace': 'art', 'mcf.trace': 'mcf', 'swim.trace': 'swim', 'gcc.trace': 'gcc'}
    marker_dict = ['o', 'x']
    
    plt.rcParams.update({'font.size': 15})
    plt.rcParams.update({'axes.linewidth': 2})
    plt.figure()
    
    plt.plot(var_x, var_y, label=title, marker = marker_dict[0], markersize=12, color='blue', linewidth=3)
    plt.grid()
    plt.legend(ncol=3, labelspacing=0.2, columnspacing=1, fontsize=10)
    ax = plt.gca()
    ax.set_ylim([-0.1, 1.05])
    ax.set_ybound([-0.1, 1.05])
    # ax.set_ybound([0.70, 0.8])
    ax.set_aspect('auto')
    plt.xlabel(var_x_name)
    plt.ylabel(var_y_name)
    plt.xscale("log", base = 2)
    plt.xticks(var_x)
    # plt.yticks([0.7 + 0.01 * n for n in range(10)])
    # plt.yticks([0.0] + var_y + [1.0])
    # plt.tight_layout()
    import pathlib
    if not pathlib.Path('figures/').is_dir():
        pathlib.Path('figures/').mkdir()
    plt.savefig(f'figures/{fname}.png', format='png', bbox_inches='tight', pad_inches=0.1)

if __name__ == "__main__":
    __bin = str(get_main_binary())
    
    rdict = {}
    get_oberlin_trace()
    # res_dict = get_result_oberlin(__bin, rdict)
    get_result_oberlin_assoc(__bin, rdict)

    var_x = []
    var_y = []
    var_x_name = 'associativity'
    var_y_name = 'hit rate'
    title = 'assoc vs. hit rate'
    for num in rdict:
        var_x.append(num)
        var_y.append(float(rdict[num]['oberlin']['HITRATE']))
    plot(var_x, var_y, var_x_name, var_y_name, title, 'assocvhitrate')
   
    rdict = {}
    get_result_oberlin_capacity(__bin, rdict) 
    var_x = []
    var_y = []
    var_x_name = 'cache capacity'
    var_y_name = 'hit rate'
    title = 'cachecapacity vs. hit rate'
    for num in rdict:
        var_x.append(num)
        var_y.append(float(rdict[num]['oberlin']['HITRATE']))
    plot(var_x, var_y, var_x_name, var_y_name, title, 'csizevhitrate')
    
    rdict = {}
    get_result_oberlin_bsize(__bin, rdict) 
    var_x = []
    var_y = []
    var_x_name = 'block size'
    var_y_name = 'hit rate'
    title = 'block size vs. hit rate'
    for num in rdict:
        var_x.append(num)
        var_y.append(float(rdict[num]['oberlin']['HITRATE']))
    plot(var_x, var_y, var_x_name, var_y_name, title, 'bsizevhitrate')
    # print(rdict)
    
    rdict = {}
    get_result_oberlin_replace(__bin, rdict) 
    var_x = []
    var_y = []
    var_x_name = 'replacement policy'
    var_y_name = 'hit rate'
    title = 'replacement vs. hit rate'
    for num in rdict:
        var_x.append(num)
        var_y.append(float(rdict[num]['oberlin']['HITRATE']))
    plot(var_x, var_y, var_x_name, var_y_name, title, 'replacementvhitrate')
