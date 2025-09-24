# measure_serial_latency.py
import serial, threading, time, collections, csv, sys

# -------- CONFIG --------
TX_PORT = 'COM25'       # port that SENDS (your transmitter)
RX_PORT = 'COM3'        # port that RECEIVES
BAUD = 115200
FRAME_TIMEOUT = 0.005   # seconds of idle to consider a frame ended (5 ms)
MATCH_WINDOW = 5.0      # seconds to keep past TX events for matching
OUT_CSV = 'serial_latency.csv'
# If you have exact framing set one of these:
DELIMITER = None        # e.g. b'\n'  or None
MSG_LEN = None          # e.g. 12 or None
# ------------------------

def open_serial(port):
    try:
        return serial.Serial(port, BAUD, timeout=0)
    except Exception as e:
        print(f"Failed to open {port}: {e}")
        sys.exit(1)

def reader(port_name, ser, q):
    buf = bytearray()
    last_recv = time.perf_counter()
    while True:
        try:
            data = ser.read(512)
        except Exception as e:
            print(f"Read error {port_name}: {e}")
            break
        now = time.perf_counter()
        if data:
            buf.extend(data)
            last_recv = now

            # framing by delimiter
            if DELIMITER:
                while True:
                    idx = buf.find(DELIMITER)
                    if idx == -1:
                        break
                    msg = bytes(buf[:idx])         # exclude delimiter
                    del buf[:idx+len(DELIMITER)]
                    q.append((now, port_name, msg))
            # framing by fixed length
            elif MSG_LEN:
                while len(buf) >= MSG_LEN:
                    msg = bytes(buf[:MSG_LEN])
                    del buf[:MSG_LEN]
                    q.append((now, port_name, msg))
            # else use idle timeout framing (handled below)
        else:
            # no data this iteration -> check IDLE framing
            if buf:
                idle = now - last_recv
                if idle >= FRAME_TIMEOUT:
                    msg = bytes(buf)
                    buf.clear()
                    q.append((last_recv, port_name, msg))
            time.sleep(0.0005)

def match_loop(q):
    # recent TX events to match against
    recent_tx = collections.deque()  # elements: (ts, msg)
    with open(OUT_CSV, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['tx_ts_s', 'rx_ts_s', 'msg_hex', 'latency_s'])
        print("Listening... Ctrl-C to stop")
        try:
            while True:
                if not q:
                    time.sleep(0.001); continue
                ts, port, msg = q.popleft()
                # keep recent_tx window
                now = time.perf_counter()
                while recent_tx and (now - recent_tx[0][0] > MATCH_WINDOW):
                    recent_tx.popleft()

                if port == TX_PORT:
                    recent_tx.append((ts, msg))
                elif port == RX_PORT:
                    # find best matching TX (same content) - newest first
                    match = None
                    for i in range(len(recent_tx)-1, -1, -1):
                        ttx, tmsg = recent_tx[i]
                        if tmsg[:4] == msg[:4]:
                            match = (ttx, tmsg, i)
                            break
                    if match:
                        ttx, tmsg, idx = match
                        latency = ts - ttx   # seconds (can be negative if clocks scheduling weirdness)
                        hexmsg = msg.hex(' ')
                        writer.writerow([f"{ttx:.9f}", f"{ts:.9f}", hexmsg, f"{latency:.9f}"])
                        f.flush()
                        print(f"Matched msg len={len(msg)} bytes  latency = {latency*1e6:.3f} µs")
                        # remove matched tx to avoid duplicate matching
                        del recent_tx[idx]
                    else:
                        # no matching tx found
                        print(f"RX-only msg (no TX match found): len {len(msg)}   hex: {msg.hex(' ')}")
                else:
                    # other port (should not happen)
                    pass
        except KeyboardInterrupt:
            print("\nStopped by user.")

def main():
    q = collections.deque()
    tx_ser = open_serial(TX_PORT)
    rx_ser = open_serial(RX_PORT)

    t1 = threading.Thread(target=reader, args=(TX_PORT, tx_ser, q), daemon=True)
    t2 = threading.Thread(target=reader, args=(RX_PORT, rx_ser, q), daemon=True)
    t1.start(); t2.start()

    match_loop(q)

if __name__ == '__main__':
    main()
