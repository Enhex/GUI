import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-r", "--release", help="Generate release build (debug by default).", action="store_true")
args = parser.parse_args()

build_type = 'Release' if args.release else 'Debug'

os.chdir('../build/mingw-' + build_type)

os.environ['CHOST'] = 'x86_64-w64-mingw32'
os.environ['AR'] = 'x86_64-w64-mingw32-ar'
os.environ['AR'] = 'x86_64-w64-mingw32-ar'
os.environ['AS'] = 'x86_64-w64-mingw32-as'
os.environ['RANLIB'] = 'x86_64-w64-mingw32-ranlib'
os.environ['CC'] = 'x86_64-w64-mingw32-gcc'
os.environ['CXX'] = 'x86_64-w64-mingw32-g++'
os.environ['STRIP'] = 'x86_64-w64-mingw32-strip'
os.environ['RC'] = 'x86_64-w64-mingw32-windres'

os.system('make -r -j$(nproc)')