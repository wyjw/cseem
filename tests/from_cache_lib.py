# script that cmake runs to generate the right tests
import subprocess
import pathlib

# binaries = ['FindHao']
binaries = ['multi2sim']
tmp_dir = '/tmp/'

def generate_file(tmp_dir = tmp_dir):
    pass

# download instructions
from enum import Enum
class HowDL(Enum):
    GITLINK = 1
    DOCKER = 2

how_to_dl = {'FindHao': HowDL.GITLINK, 'multi2sim': HowDL.DOCKER}

# build instructions
def buildFindHao(dirname):
    if not pathlib.Path(dirname + '/build').is_dir():
        subprocess.run(['rm', '-rf', 'build'], cwd=dirname + '/build')

    subprocess.run(['mkdir', 'build'], cwd=dirname)
    subprocess.run(['cmake', '..'], cwd=dirname + '/build')
    subprocess.run(['make', '-j' + str(build_threads)], cwd=dirname + '/build')

how_to_build = {'FindHao': buildFindHao, 'multi2sim': None}

# run instructions
def runFindHao(dirname):
    # get binary
    args = []
    args.append('--input')
    subprocess.run(['./CacheSim', *args], cwd=dirname + '/build')

def runMultiSim(dirname):
    args = ['docker', 'run', '-it', dirname + '/' + dirname, 'bash']
    subprocess.run(args)

how_to_run = {'FindHao': runFindHao, 'multi2sim': runMultiSim}

build_threads = 4

def read_file(filename):
    """
    read a file of git links of binaries to compare

    @param filename: filename of different git links
    """
    with open(filename) as f:
        for line in f:
            if 'git' in line:
                user = line.split('/')[-2].strip()
            elif 'docker' in line:
                user = line.split('docker/')[1].strip()
            fp = pathlib.Path(__file__).parent.resolve()

            if user in binaries and not pathlib.Path(user).is_dir():
                if how_to_dl[user] == HowDL.GITLINK:
                    get_from_git_link(line.strip(), user)
                elif how_to_dl[user] == HowDL.DOCKER:
                    get_from_docker(line.strip(), user)
            
            if user in binaries:
                build_from_dir(user)
                run_from_dir(user)

def get_from_git_link(link, dirname):
    """
    actually do the download and setup from git link

    @param link: git links of binaries 
    """
    cmd = ['git', 'clone', link, dirname]
    subprocess.run(cmd)

def get_from_docker(link, dirname):
    try:
        subprocess.run(['docker', 'pull', dirname + '/' + dirname])
    except FileNotFoundError as e:
        if e.filename == 'docker':
            print("Got here")
            subprocess.run('curl -fsSL https://get.docker.com -o /tmp/get-docker.sh', shell=True)
            subprocess.run('sudo sh /tmp/get-docker.sh', shell=True)
            subprocess.run('dockerd-rootless-setuptool.sh install', shell=True)
            subprocess.run(['docker', 'pull', dirname + '/' + dirname])

def build_from_dir(dirname):
    f = how_to_build[dirname]
    f(dirname)
    # print(dirname)

def run_from_dir(dirname):
    f = how_to_run[dirname]
    f(dirname)
    # print(dirname)

def parse_result():
    pass

if __name__ == "__main__":
    # read_file('cache_lib.txt') 
    pass
