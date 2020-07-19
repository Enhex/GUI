import os
from sys import platform

def create_symlink(src, dst):
    # create all parent directories of the symlink one
    parent_dir = os.path.dirname(dst)
    os.makedirs(parent_dir, exist_ok=True)
    
    try:
        os.symlink(src, dst)
    except:
        pass

def build(source, build_type, symlinks = [], symlink_pairs = []):
    build_dir = '../build/' + build_type + '/'

    # create build directory
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    # symlinks
    for path in symlinks:
        create_symlink(source + '/' + path, './' + path)

    for src_path, dst_path in symlink_pairs:
        create_symlink(source + '/' + src_path, './' + dst_path)

    # conan
    os.system('conan install "' + source + '/" --build=outdated -s arch=x86_64 -s build_type=' + build_type)

    # choose premake generator based on OS
    os.chdir(source)

    def premake_generate(generator):
        os.system('premake5 ' + generator + ' --location="' + build_dir + '"')
        
    if platform == 'win32':
        premake_generate('vs2019')
    else:
        premake_generate('gmake2')
        premake_generate('vscode')
