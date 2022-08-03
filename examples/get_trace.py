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

def get_result_oberlin(binary):
    print("For file art.trace, we have: ")
    subprocess.run([binary, '-f', 'traces/art.trace', '-a', '1', '-b', '16', '-c', '16384', '-s'], cwd=SOURCE_ROOT + '/examples') 
    print("For file mcf.trace, we have: ")
    subprocess.run([binary, '-f', 'traces/mcf.trace', '-a', '8', '-b', '32', '-c', '65536', '-s'], cwd=SOURCE_ROOT + '/examples') 

def get_result_ucsd(binary):
    fnames = ['proj1/traces/mcf.trace', 'proj1/traces/swim.trace', 'proj1/traces/gcc.trace']
    print("For file proj1/gcc.trace, we have: ")
    subprocess.run([binary, '-f', fnames[2], '-a', '4', '-b', '16', '-c', '16384', '-t', 'simple', '-r', 'F', '-s'], cwd=SOURCE_ROOT + '/examples') 

def get_jhu_trace():
    subprocess.run(['wget', 'https://jhucsf.github.io/spring2020/assign/assign03_traces/gcc.trace'])

def get_result_oberlin_cust(binary, result_dict, trace = 'art', a = 1, b = 64, c = 16384, r = 'L', w = 'B'):
    result_dict['oberlin'] = {}
    if trace == 'art':
        # print("For file art.trace, we have: ")
        out = subprocess.check_output([binary, '-f', 'traces/art.trace', '-a', str(a), '-b', str(b), '-c', str(c), '-r', str(r), '-w', str(w),'-s'], cwd=SOURCE_ROOT + '/examples')
        dct = parse(out, result_dict['oberlin'])
    elif trace == 'mcf':
        # print("For file mcf.trace, we have: ")
        out = subprocess.check_output([binary, '-f', 'traces/mcf.trace', '-a', a, '-b', b, '-c', c, '-r', str(r), '-w', str(w), '-s'], cwd=SOURCE_ROOT + '/examples') 
    return result_dict['oberlin']

def parse(string, result_dict):
    for line in string.split():
        line = line.decode("utf-8")
        if ':' in line:
            name = line.split(':')[0]
            value = line.split(':')[1]
            result_dict[name] = value

if __name__ == "__main__":
    __bin = str(get_main_binary())
    get_oberlin_trace()
    get_result_oberlin(__bin)
    
    get_ucsd_trace()
    get_result_ucsd(__bin)
    
    rdict = {}
    highest_hit = 0.0
    tup = None
    _c = 16384
    for trace in ['art', 'mcf']: 
        for _a in [1,2,4,8,16]:
            # for _b in [1,2,4,8,16,32,64,128]:
            for _b in [32]:
                for _r in ['L', 'F']:
                    r = get_result_oberlin_cust(__bin, rdict, a=_a, b=_b, c=_c, r=_r)
                    if float(r['HITRATE']) > highest_hit:
                        highest_hit = float(r['HITRATE'])
                        tup = (_a, _r, _c)

        print(f"Best Config for {trace} @ {_c}:", tup)
    
    _c = 4096
    for trace in ['art', 'mcf']: 
        for _a in [1,2,4]:
            for _b in [1,2,4,8,16,32,64,128, 256,512,1024]:
            # for _b in [32]:
                for _r in ['L', 'F']:
                    r = get_result_oberlin_cust(__bin, rdict, a=_a, b=_b, c=_c, r=_r)
                    if float(r['HITRATE']) > highest_hit:
                        highest_hit = float(r['HITRATE'])
                        tup = (_a, _b, _r, _c)
        print(f"Best Config for {trace} @ {_c}:", tup)
