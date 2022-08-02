# This is to get some traces found on the internet
import subprocess
from pathlib import Path

# TODO: this is busted
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
    subprocess.run(['unzip', 'traces.zip'])
    subprocess.run(['rm', '-rf', '__MACOSX'])
    subprocess.run(['gunzip', 'traces/art.trace.gz'])
    subprocess.run(['gunzip', 'traces/mcf.trace.gz'])

def get_ucsd_trace():
    trace_dir = SOURCE_ROOT + "/traces"
    if not Path(trace_dir).is_dir():
        Path(trace_dir).mkdir()
    if not Path("./proj1-traces.tar.gz").is_file():
        subprocess.run(['wget', 'http://cseweb.ucsd.edu/classes/fa07/cse240a/proj1-traces.tar.gz'])
    if not Path('./proj1').is_dir():
        Path('./proj1').mkdir()
    subprocess.run(['tar', '-xf', 'proj1-traces.tar.gz', '--directory', 'proj1'])

def get_result_oberlin(binary):
    print("For file art.trace, we have: ")
    subprocess.run([binary, '-f', 'traces/art.trace', '-a', '1', '-b', '16', '-c', '16384', '-s'], cwd=SOURCE_ROOT + '/examples') 
    print("For file mcf.trace, we have: ")
    subprocess.run([binary, '-f', 'traces/mcf.trace', '-a', '8', '-b', '32', '-c', '65536', '-s'], cwd=SOURCE_ROOT + '/examples') 

def get_result_ucsd(binary):
    print("For file art.trace, we have: ")
    subprocess.run([binary, '-f', 'proj1/traces/mcf.trace', '-a', '1', '-b', '16', '-c', '16384', '-s'], cwd=SOURCE_ROOT + '/examples') 
    print("For file mcf.trace, we have: ")
    subprocess.run([binary, '-f', 'proj1/traces/swim.trace', '-a', '8', '-b', '32', '-c', '65536', '-s'], cwd=SOURCE_ROOT + '/examples') 

if __name__ == "__main__":
    __bin = str(get_main_binary())
    # get_oberlin_trace()
    # get_result_oberlin(__bin)
    get_ucsd_trace()
    get_result_ucsd(__bin)
