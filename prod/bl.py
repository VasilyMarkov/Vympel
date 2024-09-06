import asyncio
from bleak import BleakClient
from bleak import BleakScanner

address = "54:F8:2A:32:6D:92"
RX_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"

async def find():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)

async def main(address):
    async with BleakClient(address) as client:
        model_number = await client.read_gatt_char(RX_SERVICE_UUID)
        print("Model Number: {0}".format("".join(map(chr, model_number))))




asyncio.run(find())
# asyncio.run(main(address))