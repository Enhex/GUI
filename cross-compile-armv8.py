import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-r", "--release", help="Generate release build (debug by default).", action="store_true")
args = parser.parse_args()

build_type = 'Release' if args.release else 'Debug'

os.chdir('../build/armv8-' + build_type)

os.environ['AR'] = 'aarch64-linux-gnu-ar'
os.environ['AS'] = 'aarch64-linux-gnu-as'
os.environ['RANLIB'] = 'aarch64-linux-gnu-ranlib'
os.environ['CC'] = 'aarch64-linux-gnu-gcc'
os.environ['CXX'] = 'aarch64-linux-gnu-g++'
os.environ['LD'] = 'aarch64-linux-gnu-ld'
os.environ['STRIP'] = 'aarch64-linux-gnu-strip'

os.system('make -r -j$(nproc)')