import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def generate_ppg_report(file_path):
    red_data = []
    ir_data = []
    
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or ':' not in line: continue
            
            # Split metadata from CSV payload
            parts = line.split(': ', 1)
            if len(parts) < 2: continue
            payload = parts[1].replace(';', '')
            cols = payload.split(',')
            if len(cols) < 12: continue
            
            timestamp_str = cols[0]
            led_idx = int(cols[11].strip('[]'))
            
            # Convert timestamp to seconds
            t = datetime.strptime(timestamp_str, "%H-%M-%S.%f")
            seconds = t.hour * 3600 + t.minute * 60 + t.second + t.microsecond / 1e6
            
            # Filter for Red (LED 0) and IR (LED 8) [cite: 31, 33]
            # PD 7 is col 8, PD 8 is col 9 
            if led_idx == 0:
                red_data.append({'time': seconds, 'val': int(cols[8])})
            elif led_idx == 8:
                ir_data.append({'time': seconds, 'val': int(cols[9])})

    df_red = pd.DataFrame(red_data)
    df_ir = pd.DataFrame(ir_data)

    if df_red.empty or df_ir.empty:
        print("Error: Missing Red or IR data cycles.")
        return

    # Calculate AC and DC components
    def get_metrics(values):
        dc = np.mean(values)
        ac = np.max(values) - np.min(values)
        return ac, dc

    ac_red, dc_red = get_metrics(df_red['val'])
    ac_ir, dc_ir = get_metrics(df_ir['val'])

    # Ratio of Ratios (R) formula
    R = (ac_red / dc_red) / (ac_ir / dc_ir)
    
    # Estimated SpO2 using standard calibration curve
    spo2 = 110 - 25 * R

    # Create the PPG Graph
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)
    
    time_red = df_red['time'] - df_red['time'].iloc[0]
    time_ir = df_ir['time'] - df_ir['time'].iloc[0]

    ax1.plot(time_red, df_red['val'], color='red', marker='o', label='Red Channel (660nm)')
    ax1.set_ylabel('Intensity (Raw Counts)')
    ax1.set_title(f'PPG Signal Analysis\nRatio of Ratios (R): {R:.3f} | Estimated SpO2: {spo2:.1f}%')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    ax2.plot(time_ir, df_ir['val'], color='black', marker='o', label='IR Channel (940nm)')
    ax2.set_ylabel('Intensity (Raw Counts)')
    ax2.set_xlabel('Time (seconds)')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

# Run analysis
generate_ppg_report('Lumos/2026-03-24-13-09-51.txt')