import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import re

def generate_ppg_report(file_path, target_channel_index=0):
    red_data = []
    ir_data = []
    
    # Format: "13:50:12.345, LED:[2], Chans:(100, 200, 300, 400, 500, 600)"
    line_regex = re.compile(r"([^,]+),\s*LED:\[(\d+)\],\s*Chans:\(([^)]+)\)")

    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            match = line_regex.search(line)
            if not match:
                continue
            
            timestamp_str = match.group(1)
            led_idx = int(match.group(2))
            channels = [int(v.strip()) for v in match.group(3).split(',')]
            
            try:
                t = datetime.strptime(timestamp_str, "%H:%M:%S.%f")
                seconds = t.hour * 3600 + t.minute * 60 + t.second + t.microsecond / 1e6
            except ValueError:
                continue
            
            # LED 2 = Deep Red (660nm), LED 0 = IR (940nm)
            if led_idx == 2:
                red_data.append({'time': seconds, 'val': channels[target_channel_index]})
            elif led_idx == 0:
                ir_data.append({'time': seconds, 'val': channels[target_channel_index]})

    df_red = pd.DataFrame(red_data)
    df_ir = pd.DataFrame(ir_data)

    if df_red.empty or df_ir.empty:
        print(f"Error: Missing data. Found {len(df_red)} Red and {len(df_ir)} IR samples.")
        return

    # Count samples
    num_red_samples = len(df_red)
    num_ir_samples = len(df_ir)

    # Create the Plot
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)
    
    time_red = df_red['time'] - df_red['time'].iloc[0]
    time_ir = df_ir['time'] - df_ir['time'].iloc[0]

    # Red Plot
    ax1.plot(time_red, df_red['val'], color='red', marker='o', markersize=3, label='Red (660nm)')
    ax1.set_ylabel('Intensity (Raw Counts)')
    ax1.set_title('PPG Raw Intensity Analysis')
    ax1.annotate(f"Samples: {num_red_samples}", xy=(0.02, 0.90), xycoords='axes fraction', 
                 bbox=dict(boxstyle="round", fc="white", ec="red", alpha=0.8))
    ax1.legend(loc='upper right')
    ax1.grid(True, alpha=0.3)
    
    # IR Plot
    #ax2.plot(time_ir, df_ir['val'], color='black', marker='o', markersize=3, label='IR (940nm)')
    #ax2.set_ylabel('Intensity (Raw Counts)')
    #ax2.set_xlabel('Time (seconds since start)')
    #ax2.annotate(f"Samples: {num_ir_samples}", xy=(0.02, 0.90), xycoords='axes fraction', 
    #             bbox=dict(boxstyle="round", fc="white", ec="black", alpha=0.8))
    #ax2.legend(loc='upper right')
    #ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

# Update this path to your most recent saved file
generate_ppg_report('Lumos/2026-03-24-14-38-28.txt')