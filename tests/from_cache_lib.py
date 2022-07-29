# script that cmake runs to generate the right tests
import subprocess
import pathlib

binaries = ['FindHao']

# build instructions
def buildFindHao(dirname):
    if not pathlib.Path(dirname + '/build').is_dir():
        subprocess.run(['mkdir', 'build'])
        subprocess.run(['cmake', dirname + '/.'])
        subprocess.run(['make', '-j' + build_threads])

how_to_build = {'FindHao': buildFindHao}
build_threads = 4

def read_file(filename):
    """
    read a file of git links of binaries to compare

    @param filename: filename of different git links
    """
    with open(filename) as f:
        for line in f:
            user = line.split('/')[-2]
            fp = pathlib.Path(__file__).parent.resolve()

            if user in binaries and not pathlib.Path(user).is_dir():
                get_from_git_link(line.strip(), user)

def get_from_git_link(link, dirname):
    """
    actually do the download and setup from git link

    @param link: git links of binaries 
    """
    print("DIR", dirname, "LINK", link)
    cmd = ['git', 'clone', link, dirname]
    subprocess.run(cmd)

    how_to_build[dirname]()

def parse_result():
    pass

if __name__ == "__main__":
    read_file('cache_lib.txt') 
