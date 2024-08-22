# CodeStarter
## Making projects easy

Code starter is a simple piece of software designed to help you get your projects started.

Code Starter
Allows people to create projects in a few simple steps
has built in templetaes for multiple languages,c,java,py,c++,rust,etc
creates a makefile or bash/bat build script for supported projects
or setup.py file for python
for languges like c,c++,rust, it creates a libs folder, and provides a command line abilty to install packages in a smiler way to pip. Adding them to a .json file and makefile for building
Creates a simple main file for each lanuage, with a output stament that says "Hello from Code Starter"

It will also add a coustem header file(or quivlent) with the version, and compile_number, whtch will be inbcrmented by the make file automaticlly with each compile.

For each project, it will create a folder with the name of the project, and inside will be the source code, and a build folder, where the build files will be placed.

For the main file, it will create a header of comments with the auther name, licnnse copyright info, and description. It will also create a main function, and a main file.

It will also create a.gitignore file, and a README.md file.

It will also detect if the correct cimpiler is installed, and if not, give a url to the download

Users can create coutem templates and licenses, as well as add more libies, each libury is sotred on github in a json file, pointing the the json file for that project, along with the nessary files, the files will then be downlaed into the libs dir at libs/src/lib_name. and then built into a libray.a or windows version of .a file, with there .h files put into lib/src/lib_name. This part might change at a later date

