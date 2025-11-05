import asyncio, serial_asyncio
PORT="COM8"; BAUD=115200; DELAY=0
frames=[b"+200+200\n", b"+200+100\n", b"+100+100\n", b"+140-230\n"]

class Echo(asyncio.Protocol):
    def connection_made(self, t):
        self.t=t; self.buf=bytearray()
        print(f"[OPEN] {PORT} @ {BAUD} baud")
        asyncio.create_task(self.tx())

    async def tx(self):
        i=0
        while True:
            self.t.write(frames[i % len(frames)])
            i+=1
            await asyncio.sleep(DELAY)   # keep small but nonzero

    def data_received(self, data: bytes):
        self.buf.extend(data)
        while True:
            nl = self.buf.find(b"\n")
            if nl < 0: break
            line = bytes(self.buf[:nl]); del self.buf[:nl+1]
            print("Echo:", line)

async def main():
    loop=asyncio.get_running_loop()
    await serial_asyncio.create_serial_connection(loop, Echo, PORT, baudrate=BAUD)
    await asyncio.sleep(3600)

asyncio.run(main())
