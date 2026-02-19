import argparse
import os
import time
from datetime import datetime

import serial


def pc_timestamp():
    # matches your old format: YYYY-MM-DD-HH:MM:SS.mmm
    return datetime.now().strftime("%Y-%m-%d-%H:%M:%S.%f")[:-3]


def make_output_path(folder="Lumos"):
    os.makedirs(folder, exist_ok=True)
    filename = datetime.now().strftime("%Y-%m-%d-%H-%M-%S") + ".txt"
    return os.path.join(folder, filename)


def pulse_dtr_reset(ser, low_s=0.1, boot_wait_s=0.8):
    # Optional: reset board so you catch one-shot output like readback()
    try:
        ser.dtr = False
        time.sleep(low_s)
        ser.dtr = True
        time.sleep(boot_wait_s)
    except Exception:
        pass


def main():
    ap = argparse.ArgumentParser(description="Minimal Lumos serial logger (same timestamp format as your app).")
    ap.add_argument("--port", required=True, help="COM port (e.g., COM6) or /dev/ttyACM0")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--reset", action="store_true", help="Pulse DTR after opening (helps catch one-shot readback output)")
    ap.add_argument("--folder", default="Lumos", help="Output folder (default: Lumos)")
    args = ap.parse_args()

    out_path = make_output_path(args.folder)
    print(f"Logging to: {out_path}")
    print("Press Ctrl+C to stop.\n")

    ser = serial.Serial(args.port, args.baud, timeout=0.2)

    # Keep DTR/RTS asserted (often helps keep the port stable)
    try:
        ser.dtr = True
        ser.rts = True
    except Exception:
        pass

    if args.reset:
        pulse_dtr_reset(ser)

    buf = bytearray()

    try:
        with open(out_path, "w", encoding="utf-8", buffering=1) as f:
            while True:
                chunk = ser.read(8192)
                if not chunk:
                    continue

                buf.extend(chunk)

                # Parse lines like Arduino Serial Monitor: split on '\n'
                while b"\n" in buf:
                    line_bytes, _, rest = buf.partition(b"\n")
                    buf = bytearray(rest)

                    line = line_bytes.decode("utf-8", errors="replace").rstrip("\r")
                    formatted_line = f"{pc_timestamp()}: {line}"

                    print(formatted_line, flush=True)
                    f.write(formatted_line + "\n")

    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        try:
            ser.close()
        except Exception:
            pass


if __name__ == "__main__":
    main()
