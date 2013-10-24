SMT32F103Z
==========

My repository for learning STM32F103Z microcontroller using a Linux based development envronment.

Table of contents:

* [Setup development environment](#setup-development-environment)
* [Create a new project](#create-a-new-project-from-template)
* [Build, burn and debug](#build-burn-and-debug)


Setup development environment
-----------------------------

Here are some instructions to have on a Linux host a complete development environment based on this toolset:

* Eclipse
* GCC ARM Embedded Toolchain
* OpenOCD

for this target device:
* STM32F103Z based board (bought on [WaveShare](http://www.wvshare.com/column/STM32_DevelopmentBoard.htm#Open103Z))
* ST-LINK/V2 debugger and programmer 

### Working directory
Setup the working directory that will contain all the necessary (development tools and workspace).

  ```
  mkdir -p ${HOME}/stm32/devTools/toolchain
  mkdir -p ${HOME}/stm32/devTools/openOCD
  mkdir -p ${HOME}/stm32/workspace
  ```

### Toolchain
Download executables from https://launchpad.net/gcc-arm-embedded.
At this moment only the 32-bit version is available, so if you are working on a 64-bit host operating system you need 
to installsome dependencies: 32-bit version of glibc and ncurses libraries.

Extract the archive into ${HOME}/stm32/devTools/toolchain

Update you PATH editing the file $HOME/.bashrc (ubuntu, debian, etc.) or $HOME/.bash_profile (fedora) adding this line:

  ```
  export PATH=$PATH:$HOME/stm32/devTools/toolchain/bin
  ```
  
Reload it and test it:

  ```
  source $HOME/.bashrc
  arm-none-eabi-gcc --version
  ```

### OpenOCD
TODO

### Eclipse
TODO

### Workspace
TODO


Create a new project from template
----------------------------------
TODO


Build, burn and debug
---------------------
TODO
