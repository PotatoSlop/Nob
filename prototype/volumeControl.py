import serial # COM Port Access
import time # Time lib

# Gives access to windows audio devices
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

COM_PORT = "COM3"
BAUD_RATE = 9600

devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None) # Get volume control from speaker device
    
volume = cast(interface, POINTER(IAudioEndpointVolume)) # Cast volume control interface to python object
print("Speaker initialized!")

print(f"Connecting to Arduino on {COM_PORT}...") 
arduino = serial.Serial(COM_PORT, BAUD_RATE, timeout=1) # Serial Connection
time.sleep(1)

while True:
    line = arduino.readline().decode('utf-8').strip() # --> Reads from arduino and checks for newline from the serial printlns
    target_scalar = int(line) / 100.0 # Convert % to (0-1) scalar val. for pycaw 
    volume.SetMasterVolumeLevelScalar(target_scalar, None) # Set the master volume to the target_scalar