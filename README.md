# ROBSEN ACTIVITY RECOGNITION PROJECT

## REQUIREMENTS

- Intel Real Sense 2.0 SDK (also OpenCV 3.4.1 is required but it is already present in Real Sense SDK)
- Cubemos SDK

### For Running in Visual Studio

- **Visual Studio Community Version 2019** with C++ and Python Packages for console applications already loaded.

### For Running with Compilers

- **Clang**, can be accessed from `https://clang.llvm.org/`
- **GNU Make**, can be accessed from `http://gnuwin32.sourceforge.net/packages/make.htm`
- **Python 3.7** (maybe 3.8 is also supported but did not try yet)

## ENVIRONMENT
- This project is run on Visual Studio Community version 2019 or .
- Project is supported currently only on Windows 10. For linux versions, some changes on the code has to be applied.

## WARNINGS

- In order to run the program properly, both SDK's should be installed to the default  locations proposed during the installation phases. If this is not the case, then the paths specified in the **.props** files should be changed.

## INSTRUCTIONS

### On Visual Studio

- Open Visual Studio and create a new empty C++ console project.
- Copy all the files inside the header document and also the main.cpp file to the location: `project folder` (where the .sln file is present) / `folder with the same name as project folder` / in my case, my project name is RS_cubemos, then: `RS_cubemos/RS_cubemos/` is the location where I copy all these files.
- Inside the VS, right click on header files folder in solution explorer and select: `add > existing item`. With this way, add all **the files in the header folder** (the new copies which you created on step 2).
- Inside the VS, right click on source files folder in solution explorer and select: `add > existing item`. With this way, add the **main.cpp** file (the new copy which you created on step 2).
- Open: `View > Other Windows > Property Manager`. On the property manager, right click on the  project and select add existing property sheet. With this way, select and add all the **.props** files in the Properties folder one by one. 
- Open **main.cpp** file. The line where Skeleton_Tracker object is declared, if there is a second parameter, then a **.bag video file** will be processed insted of real-time. If there is no second parameter, then real-time processing will be applied. 
- For the first parameter in the object construction, my original path is given, where my license information is placed. If the installation paths are set by the installer by default, then your path should be similar to mine. Please change this line accordingly. 
- Lastly, I am not sure if this is necessary but on the left side of the button Local Windows Debugger, there is a dropdown list and from that list please select `x64` option.

### With Compilers

- First of all, check if clang is added to the path by running `clang --version` on the command prompt. If not, then add **<clang_installation_folder>/bin** folder to the **PATH**.
- Check if GNU Make is added to the path by running `make --version` on the command prompt. If not, then add **<gnu_make_installation_folder>/bin** folder to the **PATH**.
- Open **Makefile** file and check if the paths for **PY_PATH**, **IRS_PATH** and **CM_PATH** are set correctly. If default locations are selected during the installation of Intel Real Sense SDK 2.0 and Cubemos SDK, then **IRS_PATH** and **CM_PATH** are probably correct. However, **PY_PATH** is probably wrong. Therefore, it should be set in the correct path.

## LAST ADJUSTMENTS AND RUNNING

In the **main.cpp** file, some parameters are declared by default. Please take a look at these and change them according to your processes. Keep in mind that calling a python function from C++ still does not work in Visual Studio 2019. Therefore, activity recognition modules for this environment is not present currently. About activity recognition module, right now only a static python function from C++ is called and outputs are printed. Actual activity recognition is not implemented and put into the repository yet.

### Running on Visual Studio

Try to run the project by pressing `Ctrl+F5` or pressing `Local Windows Debugger` button.

### Running with Compilers

From the command propmt in the project directory, please execute `make -i`.