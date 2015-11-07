Welcome to use SurreyFBA2.34!

-----------------------
SurreyFBA Installation
-----------------------
The source codes of SurreyFBA2.34 are in the file folder
'SurreyFBA2.34_Source/SurreyFBA2.34' within this package. There are two
versions: one is for glpk solver in directory 'sfba-glpk' and the other is for
Gurobi solver in directory 'sfba- grb'. The software can be run on three OS
platforms (Linux, Mac and Windows). The executable files of 'sfba-glpk' and
'sfba-grb' of 3 platforms are in directory
'SurreyFBA2.34_Source/PlatformBinaries'. For Linux, you can try run the
executable files (Gurobi6.0, 64bit) directly; you can also compile the source
codes under your specific version of Linux; For Mac, you can run the executable
files (Gurobi6.0, 64bit) directly; you can also compile the source codes under
Mac OSX. For Windows, you can just run the executable files (Gurobi6.0, 32bit)
directly. The default executable files within directory 'SurreyFBA2.34' are for
Mac and Windows, you can also replace it with Linux version. The software is
able to apply two LP solvers which are GLPK package (already compiled within
software) and Gurobi package. Note that if you want to run 'sfba-grb', you need
to install Gurobi6.0 solver and apply a Gurobi license (free for academics). In
folder 'SurreyFBA2.34_Source/SurreyFBA2.34/scripts', the perl/R scripts used for
statistical computing for the output of weak GNI prediction are provided.

-----------------------
Install Gurobi library
-----------------------
Go to the Gurobi website www.gurobi.com, set up an account and download Gurobi
of version 6.0. Apply Free Academic License of Gurobi, and download license key
file 'gurobi.lic'. Install Gurobi into default directories: "/opt/gurobi600"
for Linux and "/Library/gurobi600" for Mac, finally put key file into default
directories.

-----------------------
Compile the software
-----------------------
SurreyFBA2.34 was coded as NetBeans project using NetBeans IDE C++, so the
easiest way is to load them and compile them in NetBeans. Or you can compile
them for Linux and Mac by command as follows: go to folder
'SurreyFBA2.34_Source/SurreyFBA2.34/sfba-glpk (or sfba-grb)', change the folder
name 'nbproject_linux' (for Linux) or 'nbproject_mac' (for Mac) to be NetBeans
default directory name 'nbproject' then run command: 'make' you will get release
version of executable file 'sfba-glpk' (or 'sfba-grb') in sub-folder
'dist/Release/GNU-Linux-x86' (for Linux) or 'dist/Release/ GNU-MacOSX' (for
Mac).

The default Gurobi version used by SurreyFBA is 6.0.0 for Mac and Linux, if you
want use different version of Gurobi, you can set them in NetBeans as follows:
go to 'Properties' of project and setting them in categories of 'Build/C++
compiler' and 'Build/Linker'. Or you can directly change this information in
file 'nbproject/configurations.xml' in which only need to look into xml blocks
<compileType> and <linkerTool>.

-----------------------
JyMet 
----------------------- 
JyMet is a graphical user interface to the program SurreyFBA. If you compiled
new executable files of SurreyFBA2.34, just copy them into local folder
'SurreyFBA2.34' and making sure the name is 'sfba-glpk' (or 'sfba-grb') for
Linux or Mac and 'sfba-glpk.exe' (or 'sfba-grb.exe') for Windows so that JyMet
can identify the executable files for the specific platform. To open the JyMet
you just double-click the file 'JyMet.jar'.

-----------------------
QSSPN software 
----------------------- 
In SurreyFBA2.34 we add new tool called QSSPN (Quasi-Steady State Petri Nets), see
website at http://sysbio3.fhms.surrey.ac.uk/qsspn/index.html . The source codes
are located in directory 'SurreyFBA2.34_Source/QSSPN/src'. The default executable file
within directory 'SurreyFBA2.34' is for Mac.

-----------------------
Documents
-----------------------
In folder 'SurreyFBA2.34/docs', you can find JyMet2.34 manuals (file
'Manual.html') and In folder 'SurreyFBA2.34_Docs' you can find QSSPN manuals and
more help files. File 'omics' contains the detailed usage and output expiations
for GSMN related problems including Shlomi method, Fast Shlomi method, Linear
Shlomi method, GIMME, SGNI, WGNI, DPAplot and DPAsig.
