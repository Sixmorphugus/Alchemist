@echo off


echo ********** 1 / 3 PREPARE BUILDING **********


echo *** Checking for Emscripten...

if not exist "C:\emsdk" (
	echo Error: Emscripten is not installed at C:\emsdk.
	pause & exit
)

echo *** Preparing...

call C:\emsdk\emsdk activate latest

echo *** Locating Visual Studio 2019...

if not exist "C:\Program Files (x86)\Microsoft Visual Studio\Installer" (
	echo Error: Visual Studio is not installed.
	pause & exit
)

pushd "C:\Program Files (x86)\Microsoft Visual Studio\Installer"
for /f "usebackq tokens=* delims=" %%i in (`vswhere.exe -version "[16.0,17.0)" -products Microsoft.VisualStudio.Product.Community -property installationPath -nologo`) do (
	set VisualStudioLocation=%%i
)
popd

if not exist "%VisualStudioLocation%" (
	echo Error: Visual Studio is not installed.
	pause & exit
)

echo *** Found at %VisualStudioLocation%.
echo *** Preparing...

call "%VisualStudioLocation%\VC\Auxiliary\Build\vcvars64.bat" || goto :error

echo *** Prepared.


echo ********** 2 / 3 RUN CMAKE **********


mkdir Build_Web
cd Build_Web
call emcmake cmake -DCMAKE_TOOLCHAIN_FILE=C:/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. -G "NMake Makefiles" || goto :error


echo ********** 3 / 3 BUILD **********


nmake || goto :error
cd ..


echo ********** PROCESS COMPLETED **********


exit /b

:error
echo Aborting script due to error.
exit /b