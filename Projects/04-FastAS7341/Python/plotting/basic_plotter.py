import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

def plot_spectral_data(file_path, target_led_index=1, target_pd_index=6):
    data = []
    
    # Wavelength maps from the 9G.ino context [cite: 154, 155, 168-170]
    led_wavelengths = [660, 633, 599, 567, 530, 470, 450, 415, 940]
    pd_wavelengths = ["415", "445", "480", "515", "555", "590", "630", "680", "NIR", "Clear"]

    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or ':' not in line:
                continue
            
            # Using split(': ', 1) robustly ignores metadata tags like [cite: 174-177]
            parts = line.split(': ', 1)
            if len(parts) < 2: continue
            
            # Clean up trailing semicolon and split CSV columns [cite: 182]
            payload = parts[1].replace(';', '')
            cols = payload.split(',')
            if len(cols) < 12: continue
            
            # Extract timestamp, readings, and LED index [cite: 178-181]
            timestamp_str = cols[0]
            pd_readings = cols[1:11]
            led_idx_str = cols[11].strip('[]')
            
            # Filter for the requested LED
            if int(led_idx_str) == target_led_index:
                t = datetime.strptime(timestamp_str, "%H-%M-%S.%f")
                seconds = t.hour * 3600 + t.minute * 60 + t.second + t.microsecond / 1e6
                data.append({
                    'time_sec': seconds,
                    'pd_value': int(pd_readings[target_pd_index])
                })

    df = pd.DataFrame(data)
    
    # Calculate the number of samples
    num_samples = len(df)

    # Plotting
    plt.figure(figsize=(10, 6))
    time_relative = df['time_sec'] - df['time_sec'].iloc[0]
    plt.plot(time_relative, df['pd_value'], marker='o', linestyle='-', color='tab:blue')
    
    led_wave = led_wavelengths[target_led_index]
    pd_wave = pd_wavelengths[target_pd_index]
    
    plt.title(f"Spectral Analysis: PD {pd_wave}nm response to LED {target_led_index} ({led_wave}nm)")
    plt.xlabel("Time (seconds since start)")
    plt.ylabel("Intensity (Raw Counts)")
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # Annotate number of samples instead of sampling rate
    plt.annotate(f"Number of samples: {num_samples}", 
                 xy=(0.02, 0.95), xycoords='axes fraction', 
                 bbox=dict(boxstyle="round", fc="white", ec="gray", alpha=0.9))

    plt.tight_layout()
    plt.show()

# Update the path to your specific file
plot_spectral_data('Lumos/2026-03-24-13-09-51.txt')