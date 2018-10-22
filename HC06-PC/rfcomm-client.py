from bluetooth import *
import sys

if sys.version < '3':
    input = raw_input

addr = '98:D3:31:B0:8F:D5'

# search for the SampleServer service
uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"
service_matches = find_service( uuid = uuid, address = addr )

if len(service_matches) == 0:
    print("couldn't find the SampleServer service =(")
    sys.exit(0)

first_match = service_matches[0]
port = first_match["port"]
name = first_match["name"]
host = first_match["host"]
print("connecting to \"%s\" on %s" % (name, addr))

# Create the client socket
sock=BluetoothSocket( RFCOMM )
sock.connect((addr, port))

print("connected.  type stuff")
va1 = 'teste'
sock.send(va1.encode())

try:
    while True:
        data = sock.recv(1024)
        if len(data) == 0: break
        print("received [%s]" % data)
except IOError:
    pass

sock.close()
