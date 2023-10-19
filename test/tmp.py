import os
import shutil
import sys

DIRNAME = "test"
FILENAME = "test.txt"
FILE_SYMLINK = "test-symlink.txt"
FILE_LINK = "test-link.txt"
NODE_FILE = "test-node"
FIFO_FILE = "test-fifo"


def single_loop(workdir):
    os.chdir(workdir)
    os.mkdir(DIRNAME, 0o777)
    
    os.chdir(DIRNAME)
    
    with open(FILENAME, "w") as f:
        f.write("hello" * 100)
    
    os.symlink(FILENAME, FILE_SYMLINK)
    with open(FILE_SYMLINK, "r") as f:
        f.readlines()
    
    os.link(FILENAME, FILE_LINK)
    with open(FILE_LINK, "r") as f:
        f.readlines()

    os.chmod(FILENAME, 0o644)
    
    os.rename(FILENAME, FILENAME + "new")
    
    os.mknod(NODE_FILE)
    
    os.mkfifo(FIFO_FILE)
    
    os.chdir(workdir)
    os.listdir(DIRNAME)
    
    shutil.rmtree(DIRNAME)


if __name__ == '__main__':
    for i in range(1000):
        print("loop[{}]".format(i))
        single_loop(sys.argv[-1])
