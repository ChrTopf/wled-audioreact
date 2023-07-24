# wled-audioreact

This application generates cool light effects for your [WLED](https://kno.wled.ge/) driven strips based on the music currently playing on your computer. You can choose from multiple effects and audio sources to add another dimension to your music listening experience.

## !Warning!

This application can potentially trigger seizures for people with photosensitive epilepsy.

## License

This software is free for non commercial use only. You are prohibited to copy, modify or sell this software without permission.

For further licensing information, please have a look at LICENSE.txt

## Requirements

- n WLED Clients with m RGB LEDs each. (where "n" and "m" stand for positive non zero integers)

- Reliable IPv4 UDP Communication between the computer and the [WLED](https://kno.wled.ge/) clients on port 21324. For further reference on the interface between this application and WLED have a look at the [WLED Documentation](https://kno.wled.ge/interfaces/udp-realtime/).

- Linux or Windows operating system.

## Installation/Usage

1. Copy the compiled application **and its settings file** into a directory of your choice.

2. Configure everything in the settings file. See [Configuration](#Configuration) section for more information.

3. Start the application (over the command line), choose an audio stream as input and one of the effects.

## Configuration

The configuration file `settings.json` contains the following entries:

| Setting           | Example Value(s)                                      | Description                                                                                                                                                                                                                                                                                                |
| ----------------- | ----------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `addressees`      | `"0":"192.168.178.241.59",`<br/>`"1":"192.168.178.33` | A List of IPv4 Adresses of the WLED Clients. The first key must be 0. Each Client is going to receive the same Information to be displayed.                                                                                                                                                                |
| `audioStream`     | `"spotify"`                                           | The name of the currently chosen audio stream. Can be left empty, as this is modified by the application.                                                                                                                                                                                                  |
| `sampleRate`      | `44100`                                               | The sample rate or sample size, with which the audio stream is red by this application. The higher the sample rate, the higher the effect quality.                                                                                                                                                         |
| `effectIndex`     | `0`                                                   | The index of the currently chosen effect. Can be left empty, as this is modified by the application.                                                                                                                                                                                                       |
| `ledAmount`       | `144`                                                 | The amount of LEDs controlled by each WLED Client. So if you have Clients with 144 LEDs each, this parameter should be 144. Only linear (one dimensional) LED Stripes are supported. No LED matrices or LED cubes. For setups with multiple stripe Lengths, this should be the length of the smallest one. |
| `loglevel`        | `1`                                                   | The level, that determines which log messages are printed. This parameter specifies the lowest level to be printed.<br/>`0` : Debug<br/>`1` : Info<br/>`2` : Warning<br/>`3` : Error                                                                                                                       |
| `streamBlacklist` | `"0": "upmix",`<br/>`"1": "vdownmix"`                 | A list of audio stream names, that should not be selected by this application. This needs to be configured manually.                                                                                                                                                                                       |

## Developer Notes

- In Order to build the application you need to install all necessary dependencies first.

- When implementing a new Effect, do not forget to add it in the Effects.h!

### Dependencies

| Name                                       | Version    | Installation                                                                                                        |
| ------------------------------------------ | ---------- | ------------------------------------------------------------------------------------------------------------------- |
| [nlohmann JSON](https://json.nlohmann.me/) | >= v3.11.2 | Already done in CMakeLists                                                                                          |
| [PortAudio](http://www.portaudio.com/)     | >= v19.7.0 | Linux:<br/>`sudo apt-get install portaudio19-dev`<br/>Windows:<br/>Build the .dll and include it in the CMakeLists. |

## Version History

| Version | Type     | Date       | Changes                                                                            |
| ------- | -------- | ---------- | ---------------------------------------------------------------------------------- |
| 0.1     | snapshot | 09.06.2023 | implemented basic structure; added test effect                                     |
| 0.2     | snapshot | 12.06.2023 | implemented first test effect                                                      |
| 0.3     | snapshot | 18.06.2023 | first usable application; 3 effects included                                       |
| 0.4     | snapshot | 21.06.2023 | fixed wrong audio stream issue; fixed audio stream setting not deleted after crash |
| 0.5     | snapshot | 23.06.2023 | implemented sample rate and led amount setting                                     |
