
# ESP32 BLE-Controlled Robot Car

This project implements a **Bluetooth Low Energy (BLE) controlled robot car** using an **ESP32 microcontroller** and a **Python script for voice command processing**.  
The ESP32 controls a robot car with two motors, responding to voice commands processed through a Python application that uses **speech-to-text, a language model, and BLE communication**.

---

## Table of Contents
- [Overview](#overview)  
- [Hardware Requirements](#hardware-requirements)  
- [Software Requirements](#software-requirements)  
- [Setup Instructions](#setup-instructions)  
- [Project Structure](#project-structure)  
- [Usage](#usage)  
- [How It Works](#how-it-works)  
- [License](#license)  

---

## Overview

The system consists of two main components:

- **ESP32 Firmware**: Controls the robot car's motors based on commands received via BLE.  
  It interprets commands like `forward`, `reverse`, `left`, `right`, or `stop` with specified speed and duration.

- **Python Application**: Records voice commands, transcribes them using the Whisper model, processes them with a language model (**Qwen2.5-coder**), and sends the resulting commands to the ESP32 over BLE.

---

## Hardware Requirements

- ESP32 development board (e.g., ESP32-WROOM-32)  
- L298N motor driver  
- Two DC motors  
- Robot chassis  
- Power supply (e.g., battery pack)  
- Jumper wires  
- Computer with BLE support for running the Python application  

---

## Software Requirements

### ESP32 Firmware
- Arduino IDE or PlatformIO  
- ESP32 board support package  
- Libraries:  
  - `BLEDevice`  
  - `BLEServer`  
  - `BLEUtils`  
  - `BLE2902`  

### Python Application
- Python 3.8+  
- Libraries:  
  ```bash
  pip install bleak sounddevice wavio faster-whisper tts ollama


* Whisper model (`medium.en`)
* Qwen2.5-coder model for Ollama framework

**Optional**: Serial monitor for debugging (e.g., Arduino Serial Monitor)

---

## Setup Instructions

### ESP32 Setup

1. Install **Arduino IDE** or **PlatformIO**.
2. Add **ESP32 board support** via Boards Manager.
3. Install required libraries (`BLEDevice`, etc.) via Library Manager.
4. Connect ESP32 to L298N motor driver:

   * ENA (PWM) → GPIO 4
   * IN1 → GPIO 5
   * IN2 → GPIO 6
   * IN3 → GPIO 7
   * IN4 → GPIO 15
   * ENB (PWM) → GPIO 16
5. Upload the provided Arduino code (`esp32_car.ino`) to the ESP32.

### Python Application Setup

1. Install Python 3.8+.
2. Install dependencies:

   ```bash
   pip install bleak sounddevice wavio faster-whisper tts ollama
   ```
3. Download the **Whisper medium.en model**.
4. Ensure **Qwen2.5-coder** model is available for Ollama.
5. Create `inception.txt` and `inception_specs.txt` files with prompts for the language model.
6. Verify your computer has a microphone and BLE support.

### Wiring

* Connect L298N motor driver to the two DC motors.
* Power both **L298N** and **ESP32** with an appropriate supply.
* Verify all connections before powering on.

---

## Project Structure

```
.
├── esp32_car.cpp          # ESP32 firmware (Arduino sketch)
├── main.py                # Python script for voice command processing + BLE
├── inception.txt          # System prompt for LLM
├── inception_specs.txt    # Additional LLM specifications
└── command.wav            # Temporary file for recorded audio
```

---

## Usage

### Run the ESP32

1. Power on the ESP32 (it will advertise as **`esp32-car`** over BLE).
2. Open a serial monitor at **9600 baud** to view logs.

### Run the Python Application

1. Execute the script:

   ```bash
   python main.py
   ```
2. Speak a command starting with **"Hey Liberty"** (e.g., *"Hey Liberty, move forward at medium speed for 2 seconds"*).
3. The script will:

   * Record audio for 10 seconds
   * Transcribe using **Whisper**
   * Process transcript with **Qwen2.5-coder**
   * Send parsed command to ESP32 over BLE

### Example Commands

* `"Move forward at fast speed for 1000 milliseconds"`
* `"Turn left at slow speed for 500 milliseconds"`
* `"Stop"`

---

## How It Works

### ESP32

* Initializes a **BLE server** with UUIDs.
* Listens for write operations on a characteristic.
* Parses received commands like:

  ```
  forward,255,1000
  ```
* Controls **motor direction + speed via PWM**.
* Stops motors after the given duration.

### Python Application

* Scans and connects to **`esp32-car`** over BLE.
* Records audio → saves as `command.wav`.
* Transcribes with **Whisper (medium.en)**.
* Processes transcript with **Qwen2.5-coder** → outputs JSON commands.
* Sends commands (direction, speed, duration) via BLE to ESP32.

---

## License

This project is licensed under the **MIT License**.
See the [LICENSE](LICENSE) file for details.

