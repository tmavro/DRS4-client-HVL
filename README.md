# drs4client

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

## About

**drs4client** enables the user to define a set of parameters to initiate data acqusition through the Domino Ring Sampler 4 Evaluation Board. It is different from the stock DRS4 applications in that it allows the user to gather data over a predefined period of time, and it includes timestamps for captured events as well as temperature measurements. 

The software is made to be non-interactive: there's no GUI and it does not require any further user input after starting the program. This allows the user to simply initiate data acquisition for long periods of time without having to further engage with the software. 

Data is stored in a CSV-like format. Event number, temperature [°C] and timestamps [ms] are followed by the waveform data.  

```Example
Event #1, Temp: 23.130, Time: 99.0 ms
t[ns],u[mV]
0.000,-2.0
0.622,0.1
1.047,-0.3
1.613,-0.6
2.077,-1.0
2.649,-0.7
3.089,-0.9
...
```

The software is meant to run on a Debian-based GNU/Linux system, such as Raspberry Pi OS or Ubuntu, connected to a single DRS4 Evaluation Board. 

## Installation 

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

At this point your system will require a restart. Do that now. 

```bash
sudo reboot
```

The software requires root privileges to recognize the DRS4 EB's USB connection. 

```bash
sudo ./drs4client --help
```
