import json
import bleak
import serial
import time
import os
import wavio
import asyncio
import sounddevice as sd
from bleak import BleakScanner, BleakClient
from concurrent.futures import ThreadPoolExecutor
from ollama import chat, ChatResponse
from faster_whisper import WhisperModel
from TTS.api import TTS

#BLE UUIDS
SERVICE_UUID = "27d3bd94-4270-4c76-847f-b56fe601d169"
CHAR_UUID = "24a3cded-5ed9-469e-ab63-a69d97aacccc"

#BLE connection
async def connection():
    #esp32 device
    esp32_device = None
    print("Scannig for BLE devices...")
    devices = await BleakScanner.discover(timeout=5.0)

    for device in devices:
        if "esp32-car" in device.name:
            esp32_device = device
            break

    if(esp32_device is None):
        print("Could not find esp32 device")
        return None

    print(f"Connecting with {esp32_device.address} device...")
    client = BleakClient(esp32_device.address)
    await client.connect()
    print("Connect to ESP32")
    return client


async def send_response_to_esp32(client, command):
    #Sends command over over BLE to the ESP32.
    if not client or not client.is_connected:
        print("ESP32 is not connect")
        return
    data = command.encode()
    await client.write_gatt_char(CHAR_UUID,data)
    print(f"Sent command: {command}")


#Load WhisperModel
model = WhisperModel("medium.en",device="cpu")

#inserting the inceptions into the llm
with open("inception.txt","r") as file:
    inception = file.read()
with open("inception_specs.txt","r") as file2:
    inception += file2.read()

async def listen_for_command(output_file="command.wav", duration=10, fs=16000):
    #listen for wake word (hey liberty)
    print(f"Recording audio for {duration} seconds...")
    recording = sd.rec(int(duration * fs), samplerate=fs, channels=1, dtype='int16')
    sd.wait()
    
    wavio.write(output_file, recording, fs, sampwidth=2)
    print(f"Recording save to {output_file}")

       #Initialize Batched Infernce
    print("Transcribing audio...")
    segments, info = model.transcribe(output_file, beam_size=5)

    transcript = ""
    for segment in segments:
        print(f"[{segment.start:.2f}s -> {segment.end:.2f}s] {segment.text}")
        transcript += segment.text

    return transcript

def llm(transcript): 
   
    #setting up ollama with an llm 
    response :ChatResponse = chat(model='qwen2.5-coder', messages=[
        {
            'role':'system',
                'content': inception,
        },
        {
             "role": "user",
            "content": transcript,

        }
    ])
    print("llm response")
    print(response['message']['content'])
    print("end response")
    return response['message']['content']

def procesing_llm_response(llm_response):
    x = llm_response
    clean = x.replace("```json","").replace("```", "")
    z = json.loads(clean)
    print("Type of x:", type(x))  
    print("Type of z:", type(z))   

    
    print("llm json output")
    print(z)
    movements = z["Robot_Actions"]["action_1"]["movements"]
    for movementss in movements:
       # print(f"{movementss['direction']} {movementss['speed']} {movementss['duration_ms']}")
        print(movementss)
    return movements
    


async def main():
    #connect to ESP32 
    client = await connection()
    if not client:
        return
    transcript = await listen_for_command()
    commands = procesing_llm_response(llm(transcript))
    for command in commands:
        action = f"{command['direction']},{command['speed']},{command['duration_ms']}"
    
        await send_response_to_esp32(client,action)
    #listen_for_command()
if __name__ == "__main__":
    asyncio.run(main())


