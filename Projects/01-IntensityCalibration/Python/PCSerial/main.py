import os
import tkinter as tk
from tkinter import ttk
from tkinter.scrolledtext import ScrolledText
import serial
import serial.tools.list_ports
import threading
from datetime import datetime

##### 1. GUI INITIALIZATION #####

root = tk.Tk()
root.title("Lumos Serial PC GUI")
root.geometry("700x450")

connected_devices = {}

##### 2. GUI Components #####
serial_frame = tk.Frame(root)
serial_frame.pack(fill=tk.BOTH, expand=True)

serial_text = ScrolledText(serial_frame, wrap=tk.WORD)
serial_text.pack(fill=tk.BOTH, expand=True)

function_frame = tk.Frame(root)
function_frame.pack(fill=tk.X)

port_list_label = ttk.Label(function_frame, text="Select Port")
port_list_label.pack(side=tk.LEFT, padx=5, pady=5)

port_var = tk.StringVar()
port_list = ttk.Combobox(function_frame, state="readonly", textvariable=port_var)
port_list.pack(side=tk.LEFT, padx=5, pady=5)

connect_button = ttk.Button(function_frame, text="Connect this Lumos")
connect_button.pack(side=tk.LEFT, padx=5, pady=5)

clear_button = ttk.Button(function_frame, text="Clear")
clear_button.pack(side=tk.LEFT, padx=5, pady=5)

##### 3. Refresh Ports #####
def refresh_ports():
    ports = [port.device for port in serial.tools.list_ports.comports()]
    current_port = port_var.get()
    port_list['values'] = ports
    if current_port in ports:
        port_var.set(current_port)
    else:
        port_var.set('')
    root.after(1000, refresh_ports)

##### 4. Clear Serial #####
def clear_serial():
    serial_text.delete('1.0', tk.END)

clear_button.config(command=clear_serial)

##### 5. Open data window #####
def open_data_window(port_name, ser):
    window = tk.Toplevel(root)
    window.title(f"Data - {port_name}")
    window.geometry("650x800")

    data_text = ScrolledText(window, wrap=tk.WORD)
    data_text.pack(fill=tk.BOTH, expand=True)

    stop_button = ttk.Button(window, text="Stop Record and Save")
    stop_button.pack(pady=5)

    stop_event = threading.Event()
    data_buffer = []

    def read_serial():
        while not stop_event.is_set():
            try:
                if ser.in_waiting:
                    line = ser.readline().decode('utf-8', errors='replace').strip()
                    timestamp = datetime.now().strftime('%Y-%m-%d-%H:%M:%S.%f')[:-3]
                    formatted_line = f"{timestamp}: {line}"
                    data_buffer.append(formatted_line)
                    data_text.insert(tk.END, formatted_line + "\n")
                    data_text.see(tk.END)
            except Exception as e:
                data_text.insert(tk.END, f"Error: {e}\n")
                break

    def stop_and_save():
        stop_event.set()
        ser.close()
        connected_devices.pop(port_name, None)
        save_data(port_name, data_buffer)
        window.destroy()

    threading.Thread(target=read_serial, daemon=True).start()
    stop_button.config(command=stop_and_save)

##### 6. Save Data to file #####
def save_data(port_name, data_buffer):
    folder = f"Lumos"
    os.makedirs(folder, exist_ok=True)
    filename = datetime.now().strftime('%Y-%m-%d-%H-%M-%S') + ".txt"
    filepath = os.path.join(folder, filename)

    with open(filepath, 'w') as file:
        for line in data_buffer:
            file.write(line + "\n")

    serial_text.insert(tk.END, f"Data saved: {filepath}\n")
    serial_text.see(tk.END)

##### 7. Connect Serial Port #####
def connect_serial():
    port_name = port_var.get()

    if port_name in connected_devices:
        tk.messagebox.showwarning("Already Connected", f"{port_name} is already connected!")
        return

    try:
        ser = serial.Serial(port_name, 115200, timeout=1)
        connected_devices[port_name] = ser
        open_data_window(port_name, ser)
        serial_text.insert(tk.END, f"Connected to {port_name}\n")
        serial_text.see(tk.END)
    except Exception as e:
        serial_text.insert(tk.END, f"Error connecting to {port_name}: {e}\n")
        serial_text.see(tk.END)

connect_button.config(command=connect_serial)

##### START GUI #####

serial_text.insert(tk.END, """Welcome to the Lumos Serial PC App
1. Select your Serial Port
2. Click 'Connect this Lumos' to start data collection
3. Click 'Stop Record and Save' in data window to finish and save data
Enjoy your journey of non-invasive sensing!
""")

refresh_ports()
root.mainloop()
