STM32F103Z - Workspace libopencm3
============================

Some programs for learning STM32F103Z microcontroller using a Linux based development envronment relying on libopencm3.


TOC
---

Table of contents:
* [Setup development environment](#setup-development-environment)
* [Make your project](#make-your-project)


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
  mkdir -p $HOME/stm32/devTools/toolchain
  mkdir -p $HOME/stm32/devTools/openOCD
  ```

### Toolchain
Download executables from https://launchpad.net/gcc-arm-embedded.
At this moment only the 32-bit version is available, so if you are working on a 64-bit host operating system you need 
to installsome dependencies: 32-bit version of glibc and ncurses libraries.

Extract the archive into $HOME/stm32/devTools/toolchain

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
OpenOCD needs some dependencies: libtool libftdi-devel libusb1-devel. 
Install them with the package manager of your host linux distribution (aptitude, yum).

Download OpenOCD sources from http://freefr.dl.sourceforge.net/project/openocd/openocd

Extract the archive into $HOME/stm32/devTools/openOCD

Build it:
  ```
  cd $HOME/stm32/devTools/openOCD
  ./configure --enable-stlink
  make
  ```

Update you PATH editing the file $HOME/.bashrc (ubuntu, debian, etc.) or $HOME/.bash_profile (fedora) adding this line:

  ```
  export PATH=$PATH:$HOME/stm32/devTools/openOCD/src
  ```
  
Reload it and test it:

  ```
  source $HOME/.bashrc
  openocd --version
  ```

### Workspace
Clone the git repository:
  ```
  cd $HOME/stm32
  git clone https://github.com/smanni/stm32.git github
  ```
  
Make your project
-----------------

Here some instructions to have a working project.

### Create a new project from template
cd $HOME/stm32/github/workspace_libopencm3
cp -R template <your_project>
cd <your_project>

Use your favorite editor to edit your sources and consistently modify the Makefile

### Build and debug 
make

TODO
