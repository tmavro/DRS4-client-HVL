# DRS4-client-HVL

## What? 

This software is meant to run on a GNU/Linux system connected to a single DRS4 Evaluation Board. Eventually, it will enable users to define certain parameters and initate gathering of data from channel one. 

## How? 

Compiling and running the software requires that you've installed libusb-1.0. In Ubuntu this can easily be done in terminal:

``bash
sudo apt-get install libusb-1.0.0-dev`` 

From there, simply clone this repository and run make. 

``bash
git clone https://github.com/tmavro/DRS4-client-HVL
make``

The software will probably require root privileges to recognize the DRS4 EB. 

If you'd like to compile read_binary.cpp, this can be done through passing the following command: 

``gcc -o read_binary read_binary.cpp -lm``

