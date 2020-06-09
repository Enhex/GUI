set source="%~dp0\"

cd ../..
mkdir example-build
cd example-build

REM conan install %source% --build=outdated
REM conan install %source% --build=outdated -s build_type=Debug

cd %source%
premake5 vs2019 --location=../../example-build/