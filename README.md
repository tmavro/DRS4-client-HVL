# DRS4-client-HVL

## What? 

This software is meant to run on a GNU/Linux system connected to a single DRS4 Evaluation Board. Eventually, it will enable users to define certain parameters and initate gathering of data from channel one. 

## How? 

Compiling and running the software requires that you've installed libusb-1.0. It's also recommended that you use a screen multiplexer, such as tmux. On debian-based systems this can easily be done in terminal:

```bash
sudo apt update && sudo apt -y upgrade && sudo apt-get install libusb-1.0.0-dev tmux
```

To capture temperature data with the MAX31865 pcb, you'll need some python libraries: 

```bash
sudo pip3 install --upgrade setuptools adafruit-python-shell

wget https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/raspi-blinka.py -O- | sed '124d' - > raspi-blinka.py

sudo python3 raspi-blinka.py

sudo pip3 install adafruit-circuitpython-max31865
```

From there, simply clone this repository and run make. 

```bash
git clone https://github.com/tmavro/DRS4-client-HVL

cd DRS4-client-HVL

make
```

The software will probably require root privileges to recognize the DRS4 EB. 

```bash
sudo ./drs4client -h
```

```
  Usage: DRS4-client [OPTION]Usage: DRS4-client [OPTION]
  Command-line application for timed data acqusition with DRS4 EB.
  
  -f, --sampling-frequency Set sampling frequency. Default: 5 [GHz] 
  -i, --input-range        input range, ex. input 0 means -0.5 to 0.5. Default: 0 
  -l, --trigger-level      Set trigger level in volts. Default: 0.02 [V] 
  -d, --trigger-delay      Set trigger delay in ns. Default: 250 [ns] 
  -n, --numbered           Run for given number of events. Default: 0 
  -t, --timed              Run for given amount of seconds. Default: 0 
  -w, --ignore-wave        Do not save waveforms, only time and temp. Default: False 
  -h, --help               Shows this help message 
  
          Examples:
          DRS4-client -t259200      Records events for 72 hours.
          DRS4-client -n50 -w       Records 50 events without waveform data.
  
  ```
