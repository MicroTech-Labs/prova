import serial
from xmodem import XMODEM

def getc(size, timeout=1):
    return ser.read(size) or None

def putc(data, timeout=1):
    return ser.write(data)

# Update this with your serial port
SERIAL_PORT = 'COM3'  # or '/dev/ttyUSB0' on Linux/Mac
BAUDRATE = 9600
FILE_PATH = 'file_to_send.txt'

# Open serial connection
ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)

# Initialize XMODEM with getc and putc functions
modem = XMODEM(getc, putc)

# Open the file you want to send
with open(FILE_PATH, 'rb') as stream:
    print("Starting file transfer...")
    success = modem.send(stream)
    if success:
        print("File sent successfully!")
    else:
        print("File transfer failed.")

# Close the serial connection
ser.close()
