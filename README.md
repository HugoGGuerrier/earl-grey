# Earl Grey : The full functional compiler JS

Authors : Hugo GUERRIER | Emilie SIAU
Licence : MIT

This is the main repository for the Earl Grey language. It contains source code for the compiler and the virtual machine.
The compiler is written in C lang and use flex and bison for the syntaxical analysis.
The virtual machine is also in C for performance issues.

## Requirements :

* flex
* bison
* make
* gcc (or equivalents)

## How to build :

* Run `$> make`
* The executables are in the `./bin/` folder

## How to install :

* Run `$> sudo make install`
* You can now use `$> egcc` and `$> egvm` from every working directory

## How to run the compiler :

* Run `$> egcc my_file.eg` to compile the file
* Run `$> egcc -h` to display the help menu

## How to run the virtual machine :

* Run `$> egvm my_file.egb` to execute the file
* Run `$> egvm -h` to display the help menu

## TODOS :

* Virtual machine : Verify the endianess of the CPU
* Virtual machine : Read a char in Windows
* Virtual machine : Read a char in Mac OS
* Compiler : Finish!!
