# BlackJack
Program intended to provide data and an interface for machine learning or other strategies to beat the game of black jack

# Cross Compile for Raspberry Pi
http://amgaera.github.io/blog/2014/04/10/cross-compiling-for-raspberry-pi-on-64-bit-linux/

The CMAKE scripts expect a tools folder to be in this directory

```git clone https://github.com/raspberrypi/tools.git```

You will also need to copy some libraries off the Raspberry Pi

```mkdir -p rootfs/opt
cp /run/media/amgaera/root/lib/ rootfs/ -rv
cp /run/media/amgaera/root/usr/ rootfs/ -rv
cp /run/media/amgaera/root/opt/vc rootfs/opt/ -rv
```
or you can copy the entire /lib and /usr libraries from the raspberry pi