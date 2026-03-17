#!/bin/bash

arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840
arduino-cli upload -p /dev/cu.usbmodem2101 --fqbn Seeeduino:nrf52:xiaonRF52840   