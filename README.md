# GPS rescue device system

## Description

This project is a GPS rescue device system that allows the user to send a current signal to a rescue team. The system consists of a GPS module, a microcontroller, and a battery. The GPS module is used to get the current location of the user. The microcontroller is used to control the system. The battery is used to power the system.

## Table of Contents

- [GPS rescue device system](#gps-rescue-device-system)
  - [Description](#description)
  - [Table of Contents](#table-of-contents)
  - [Materials](#materials)
  - [Installation](#installation)
    - [Microcontroller: Esp32 or Esp8266](#microcontroller-esp32-or-esp8266)
    - [Server: Next.js](#server-nextjs)
  - [Usage](#usage)
  - [License](#license)

## Materials

- Microcontroller: Esp32 or Esp8266
- GPS module: NEO-M8M (u-blox), Can be replaced by other GPS modules.
- Server: Executable Node.js Server

## Installation

There are two groups of system: the user system and the monitor system.

### Microcontroller: Esp32 or Esp8266

The microcontroller is used to PlatformIO to build and upload the code to the microcontroller. The code is written in C++.

1. Install [PlatformIO](https://platformio.org/install/ide?install=vscode) on VSCode.
2. Open the project folder in VSCode.
3. Adjustment the code in `src/main.cpp` to fit your needs.
   - network name, password, etc. (Line 50)
4. Build and upload the code to the microcontroller.
5. Open the serial monitor to see the output of the code.

### Server: Next.js

The server is used to Next.js to build the code to the server. The code is written in TypeScript.

1. Install [Node.js](https://nodejs.org/en/download/).
2. Build the code to the server.

   ```bash
   cd server
   npm run build
   ```

## Usage

This project is used for GPS rescue device system.There are two groups of system: the user system and the monitor system.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
