# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

# https://github.com/adafruit/Adafruit_CircuitPython_MAX31865

import time
import board
import digitalio
import adafruit_max31865

# Create sensor object, communicating over the board's default SPI bus
spi = board.SPI()
cs = digitalio.DigitalInOut(board.D5)  # Chip select of the MAX31865 board.
sensor = adafruit_max31865.MAX31865(spi, cs)

# Read temperature.
temp = sensor.temperature
# Print the value.
print(temp)
