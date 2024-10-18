import asyncio
from bleak import BleakScanner
from bleak import BleakClient

from pymodbus.client.serial import ModbusSerialClient as ModbusClient
from pymodbus.constants import Endian
from pymodbus.payload import BinaryPayloadBuilder, BinaryPayloadDecoder

UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
UART_TX_CHAR_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

data_to_write = bytearray([0x0A, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0xdc, 0x81, 0x0D, 0x01])


modbus_slave_id = 1
holding_register_address = 0x0001
number_of_registers = 2

def notification_handler(sender, data):
    print(f"Notification from {sender}: {data}")

async def run():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
    address = "54:F8:2A:32:6D:92"
    
    async with BleakClient(address) as client:
        print("Connected device: ", address)

        services = await client.get_services()
        print("Discovered services:")
        for service in services:
            print(f"Service UUID: {service.uuid}, Handle: {service.handle}")
            for characteristic in service.characteristics:
                print(f"  Characteristic UUID: {characteristic.uuid}, Handle: {characteristic.handle}, Properties: {characteristic.properties}")
        

        # Start notifications on the response characteristic
        await client.start_notify(UART_TX_CHAR_UUID, notification_handler)
        print(f"Started notifications on characteristic {UART_TX_CHAR_UUID}")

        # request = ModbusClient(port='/dev/ttyUSB0', baudrate=38400).read_holding_registers(
        #     holding_register_address, number_of_registers, unit=modbus_slave_id
        # )
        # request_payload = request.encode()
        # print(request_payload)
        try:
            data = await client.read_gatt_char(UART_RX_CHAR_UUID)
            print(f"Read data from characteristic {UART_RX_CHAR_UUID}: {data}")
        except Exception as e:
            print(f"Failed to read from characteristic {UART_RX_CHAR_UUID}: {e}")


        # try:
        #     await client.write_gatt_char(UART_RX_CHAR_UUID, request_payload)
        #     print(f"Sent Modbus request to characteristic {UART_RX_CHAR_UUID}")
        # except Exception as e:
        #     print(f"Failed to write to characteristic {UART_RX_CHAR_UUID}: {e}")

loop = asyncio.get_event_loop()
loop.run_until_complete(run())