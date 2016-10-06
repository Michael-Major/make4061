CSci4061 F2016 Assignment 1

login: major072

date: 10/5/2016

name: Michael Major, Shannyn Telander, Chi-Cheng Chang

id: 4995102 / major072, 4978484 / telan024, 5327023 / chan1434

group number: 61 

1. HOW TO COMPILE OUR PROGRAM: To compile our program, make sure you have all of the source files (Makefile, main.c, util.c, util.h) in one folder. Open up a linux terminal in that folder. Type 'make' and then press enter. This will create an executable file called make4061. 

2. GROUP CONTRIBUTIONS:
  Jason: 
    Jason wrote many of the loops and recursive functions that we used to cycle through our targets, dependencies, etc. He also handled most of the adjustments in the logic of our program when we found bugs. Jason also added comments to the parts of the file that he wrote.
   Mike:
    Mike wrote our fork(), exec(), and wait() function. He added comments to the parts of the file that he wrote. He also wrote half of this README file and found a bug related to our -B flag.
   Shannyn:
    Shannyn attended TA office hours and asked questions about what we were required to print as error messages. She also found an extra tricky bug that wasn't working for a case when a -n flag was used. Shannyn created test cases and compared our make4061's output with the output of the provided solution to find bugs.

3. How to use the program from the shell (syntax).

    After you have compiled our program using the 'make' command, the executable file, 'make4061' can then be used to compile your own makefiles.
    The command:
    
      $ ./make4061
      
    will execute the 'Makefile' that is in the same directory as the make4061 object file. This command will execute the commands within the makefile and build the object files detailed within the makefile. If a parent file is dependent on another file to be built, the parent file will only be recreated while the file it is dependent on is unaltered and 'up to date.'
    
    To compile a specific Makefile:
    
      $ ./make4061 -f filename
      
    If you do not specify a filename, 'makefile' will be assumed. This command will execute the commands within the specified makefile. If a parent file is dependent on another file to be built, the parent file will only be recreated while the file it is dependent on is unaltered and 'up to date.'
    
    To build a specific target within the Makefile:
    
      $ ./make4061 specified_target
      
    This command will cause make4061 build only the specified target and any dependent files necessary. 
    
    The command:
    
      $ ./make4061 -n 
      
    will print all of the commands detailed within the designated makefile. This flag will not execute any of the commands that it is printing. 
    
    The command:
    
      $ ./make4061 -B
      
    will excute all of the commands within makefile, without any consideration if the file exists in the directory. Therefore, when this flag is used, the files are always recompiled. 
    
    These flags and additional specifications ('-f filename', '-n', '-B', 'specified_target') can be used together, in any order after './make4061' on the command line.

