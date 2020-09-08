# BlackJack
Program intended to provide data and an interface for machine learning or other strategies to beat the game of black jack

# Dependency sqlite3
## Ubuntu
` sudo apt-get install sqlite3 `
` sudo apt-get install libsqlite3-dev `

* note you should install these on the raspberry pi as well before copying over the rootfs

# Build
* In the script directory there are two scripts for setting up the build folder. One for linux one for raspberry-pi these can be ran from the repo folder
## Linux
``` ./scripts/linuxBuild.sh```

## Raspberry Pi
``` ./scripts/rpi3+Build.sh```

These will create a build folder in the main directory that you can run make in


# Cross Compile for Raspberry Pi

I followed this post, which is using an old version of gcc the toolchain is currently setup to only compile for a 32 bit os

http://amgaera.github.io/blog/2014/04/10/cross-compiling-for-raspberry-pi-on-64-bit-linux/

The CMAKE scripts expect a tools folder to be in this directory

```git clone https://github.com/raspberrypi/tools.git```

You will also need to copy some libraries off the Raspberry Pi

The below are commands to use ssh to a raspbery pi to copy them over. This takes a while and it would be faster to directly mount the raspberypi sd card and copy the files over, but I suggest using rsync that way if you forget to install a library or need one later from the raspberrypi you can just resync

From the main repo directory
```
cd tools
mkdir -p rootfs/opt
```
## scp
```
scp -r <username>@<rpi-ip-address>:/lib rootfs/
scp -r <username>@<rpi-ip-address>:/usr rootfs/
scp -r <username>@<rpi-ip-address>:/opt/vc rootfs/
```

## rsync
```
rsync -rv -e 'ssh' <username>@<rpi-ip-address>:/lib/ rootfs/lib
rsync -rv -e 'ssh' <username>@<rpi-ip-address>:/usr/ rootfs/usr
rsync -rv -e 'ssh' <username>@<rpi-ip-address>:/opt/vc rootfs/opt/vc
```

* note you can pass the -n flag to rsync to perform a dry run which will only list the files to be synced


## Sqlite3 dependency
In order to build you must have sqlite3 installed on your local machine to build for linux
and the rootfs must be copied from a rapberry pi with sqlite3 in order to cross compile

## TODO's
* Move to newer version of GCC for cross compiling
  * https://github.com/Pro/raspi-toolchain
* Implement Insurance Logic for game. Currently it just sets a flag in the player but nothing is done.
* Look into ways to not need to copy rootfs from raspberrypi