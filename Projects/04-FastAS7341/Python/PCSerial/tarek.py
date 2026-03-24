import os
import tkinter as tk
from tkinter import ttk, messagebox
from tkinter.scrolledtext import ScrolledText
import serial
import serial.tools.list_ports
import threading
import struct
from datetime import datetime

##### 1. CONSTANTS & CONFIGURATION #####
BAUD_RATE = 1000000
PACKET_SIZE = 13  # 1 byte (LED Index) + 12 bytes (6 uint16_t readings)
CMD_START = b'\xAA'
CMD_READY = b'\xA5'
CMD_COMPLETE = b'\xBB'

root = tk.Tk()
root.title("Lumos Binary Data GUI")
root.geometry("700x500")

connected_devices = {}

##### 2. GUI Components #####
serial_frame = tk.Frame(root)
serial_frame.pack(fill=tk.BOTH, expand=True)

serial_text = ScrolledText(serial_frame, wrap=tk.WORD, bg="#1e1e1e", fg="#ffffff")
serial_text.pack(fill=tk.BOTH, expand=True)

function_frame = tk.Frame(root)
function_frame.pack(fill=tk.X)

port_list = ttk.Combobox(function_frame, state="readonly")
port_list.pack(side=tk.LEFT, padx=5, pady=5)

connect_button = ttk.Button(function_frame, text="Connect & Start")
connect_button.pack(side=tk.LEFT, padx=5, pady=5)

##### 3. Logic Functions #####
def refresh_ports():
    ports = [port.device for port in serial.tools.list_ports.comports()]
    port_list['values'] = ports
    root.after(2000, refresh_ports)

def open_data_window(port_name, ser):
    window = tk.Toplevel(root)
    window.title(f"Live Binary Stream - {port_name}")
    window.geometry("600x600")

    data_text = ScrolledText(window, wrap=tk.WORD)
    data_text.pack(fill=tk.BOTH, expand=True)
    
    status_label = ttk.Label(window, text="Waiting for Handshake (0xA5)...")
    status_label.pack(pady=5)

    stop_button = ttk.Button(window, text="Stop and Save")
    stop_button.pack(pady=5)

    stop_event = threading.Event()
    data_buffer = []

    def read_serial():
        nonlocal data_buffer
        try:
            while not stop_event.is_set():
                if ser.in_waiting > 0:
                    # Check for handshake if we haven't started
                    first_byte = ser.read(1)
                    
                    if first_byte == CMD_READY:
                        status_label.config(text="Hardware Ready. Sending Start Signal...")
                        ser.write(CMD_START)
                        continue
                    
                    if first_byte == CMD_COMPLETE:
                        status_label.config(text="Cycle Complete (0xBB).")
                        break

                    # If it's a valid ledIndex (0-10), read the remaining 12 bytes of data
                    led_idx = int.from_bytes(first_byte, "little")
                    if 0 <= led_idx <= 10:
                        payload = ser.read(12)
                        if len(payload) == 12:
                            # Unpack 6 unsigned shorts (uint16_t)
                            readings = struct.unpack('<6H', payload)
                            
                            timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]
                            formatted_data = f"{timestamp}, LED:[{led_idx}], Chans:{readings}"
                            
                            data_buffer.append(formatted_data)
                            data_text.insert(tk.END, formatted_data + "\n")
                            data_text.see(tk.END)
        except Exception as e:
            data_text.insert(tk.END, f"\nSerial Error: {e}\n")

    def stop_and_save():
        stop_event.set()
        ser.close()
        connected_devices.pop(port_name, None)
        save_to_lumos_folder(data_buffer)
        window.destroy()

    threading.Thread(target=read_serial, daemon=True).start()
    stop_button.config(command=stop_and_save)

def save_to_lumos_folder(buffer):
    os.makedirs("Lumos", exist_ok=True)
    fname = datetime.now().strftime('%Y-%m-%d-%H-%M-%S') + ".txt"
    path = os.path.join("Lumos", fname)
    with open(path, 'w') as f:
        f.write("\n".join(buffer))
    serial_text.insert(tk.END, f"Saved {len(buffer)} samples to {path}\n")

def connect_serial():
    port_name = port_list.get()
    if not port_name: return
    
    try:
        ser = serial.Serial(port_name, BAUD_RATE, timeout=0.1)
        connected_devices[port_name] = ser
        open_data_window(port_name, ser)
        serial_text.insert(tk.END, f"Connected to {port_name} at {BAUD_RATE} baud.\n")
    except Exception as e:
        messagebox.showerror("Connection Error", str(e))

##### INITIALIZE #####
connect_button.config(command=connect_serial)
refresh_ports()
serial_text.insert(tk.END, "Lumos GUI Ready.\nSelect port and click Connect.\n")
root.mainloop()