
Here is the problem.

My dir structure is following:

..
  |-linux       (here resides the source code for linux kernel 5.14.0-09718-g4b93c544e90e, 
  |              built with GCC Ubuntu 9.3.0-17ubuntu1~20.04 + binutils 2.34)
  |-linux-cross (here resides the source code for linux kernel 5.14.0-09718-g4b93c544e90e, 
  |              built with x86_64-linux-gcc 8.5.0 + binutils 2.36.1 
  |              from https://mirrors.edge.kernel.org/pub/tools/crosstool/).
  |              Tried also versions 9.4 and 11.1 from there.
  |-extmodules - This repo.         
         
        
     
I am trying to get my module working with kpatch (github.com/dynup/kpatch) and get stuck debugging the compiler.
To spot the place when everything gets wrong, I compile this module independently four times, with host or cross compiler, and patched or unpatched.

`make KCFLAGS="-ffunction-sections -fdata-sections"`


for host, and 

`CROSS_COMPILE=x86_64-linux- suffix=-cross make KCFLAGS="-ffunction-sections -fdata-sections"`


for cross-compiler. 
Then I dump symbols with `objdump -t cdevmod.o` and full text with `objdump -s cdevmod.o | grep PATCHED -C5`. And notice that for a host compiler, both sections `.rodata.cdev_read.str1.1` and `.rodata.cdev_read.str1.8` (watch files p_host_sections and p_host_syms) are present in symbol table of patched version, while for a cross compiler section `.rodata.cdev_read.str1.1` is present as a section, but absent in the symbol table (watch files p_cross_sections and p_cross_syms). This causes kpatch utilities not to work properly and crash.

I want the cross compiler to behave like host one.



