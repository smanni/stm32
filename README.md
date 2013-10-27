STM32F103Z
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
  mkdir -p $HOME/stm32/devTools/toolchain
  mkdir -p $HOME/stm32/devTools/openOCD
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
OpenOCD needs some dependencies: libtool libftdi-devel libusb1-devel. 
Install them with the package manager of your host linux distribution (aptitude, yum).

Download OpenOCD sources from http://freefr.dl.sourceforge.net/project/openocd/openocd

Extract the archive into ${HOME}/stm32/devTools/openOCD

Build it:
  ```
  cd ${HOME}/stm32/devTools/openOCD
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

### Eclipse
Download Eclipse for C/C++ developers from http://www.eclipse.org/downloads/

Extract the archive into ${HOME}/stm32/devTools/eclipse

Launch the IDE and install the Zylinn plugin using the Plugins Manager with this URL http://opensource.zylin.com/zylincdt

### Workspace
Clone the git repository:
  ```
  cd $HOME/stm32
  git clone git@github.com:smanni/stm32.git github 
  ```
  
Launch Eclipse and switch workspace to HOME/stm32/github.

Import all the projects into the workspace using the menu File->Import->General->Existing Projects into Workspace


Create a new project from template
----------------------------------
* Select the Template project
* Right-click and select copy (Ctrl+C)
* Paste (Ctrl+V) renaming the project as you want


Build
-----
* Select the project you want to build
* Select the build configuration:
  * Right-click on the project
  * Select 'Build-configuration' and choose between Debug and Release
* Build: 
  * Right-click on the project
  * Select 'Buil project'
* Check for errors in the console

Debug
-----
* Add a new debug configuration:
  * Select the project you want to debug
  * Select the menu 'Run->Debug Configurations...' 
  * Select the item 'Zylinn Emebedded Debug (native)'
  * Right-click and then select 'New'
    * In the tab 'Debugger' tab select 'Emebedded debugger' and type 'arm-none-eabi-gdb' in the 'GDB debugger' field
    * In the tab 'Commands' in the 'Initialize commands' field type :

    ```
    target remote localhost:3333
    mon reset halt 
    mon flash probe 0
    mon flash write_image erase XXXX/Debug/XXXX.elf   # NOTE: replace XXXX with the project name
    mon reset halt
    ```

    * __NOTE__: replace XXXX with the project name
    
* Launch the debug session
  * Launch openOCD with:
  
  ```
  cd $HOME/stm32/github
  cd workspace_metal
  openocd -f OpenOCD/stm_board.cfg 
  ```
  
  * Launch Debug configuration in Eclipse
  * Enjoy your debug session


