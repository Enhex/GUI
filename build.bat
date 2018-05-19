set source="%~dp0\"

cd ..
mkdir build
cd build

REM conan install %source% --build=outdated
conan install %source% --build=outdated -s build_type=Debug

cd %source%
premake5 vs2017 --location=../build/