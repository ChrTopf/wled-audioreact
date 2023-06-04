# wled-audioreact

This project provides an application that reads an audio stream on the computer where it is executed and then sends commands to a [WLED](https://kno.wled.ge/) strip to change its color.

## Requirements

- n WLED Clients with 144 RGB LEDs each. (where "n" stands for a positive non zero integer)

- IPv4 UDP Communication between this computer and the [WLED](https://kno.wled.ge/) clients on port 19446. For further reference on the interface between this application and WLED have a look at the [documentation](https://kno.wled.ge/interfaces/udp-realtime/).

- Linux operating system.

## Installation/Usage

1. Copy the compiled application and its settings file into a directory of your choice.

2. Configure everything in the settings file.

3. Start the application (over the command line) and choose an audio stream as input.

## Configuration

TODO