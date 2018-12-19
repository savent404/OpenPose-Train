from pyserver.network import * # install by "pip install pyserver"
import socket
import random

def createUdpSocket():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return s

def createRandomTcpServer():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', 0)) # return a avaliable random port
    s.listen(1)
    return s
    
def convertStr2Ord(string):
    """ convert a little-edition binary to int """
    data = 0
    cnt = 0
    for i in string:
        data |= ord(i) << cnt
        cnt += 8
    return data

def recvfromTcpSocket(sock, blockSize=4096, accept_addr=None):
    d = ''
    while True:
        conn, addr = sock.accept()
        if accept_addr is None:
            break
        elif accept_addr == addr[0]:
            break
        else :
            conn.close()
            continue
    print ("TCP server got a client: " + str(addr))
    while True:
        block = conn.recv(blockSize)
        d += block
        if len(block) < blockSize:
            print ("TCP recv done, all size: " + str(len(d)))
            break
    conn.close()
    return d

def checkHandshake(addr, data):
    """ check if is handShake msg from client """
    if data == "STA\x01":
        print ("Got a handshake from" + str(addr))
        print ("Callback a handshake ack for him")
        tmpSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        tmpSocket.sendto(str("STA\x02"), (addr[0], 9001))
        tmpSocket.close()
        return True
    return False

def checkFileTransmitStart(addr, data):
    """ check if is File Transmit request msg from client """
    if data[:4] == "STA\x04":
        tmp = data.split(':')
        fType = convertStr2Ord(tmp[1])
        fSize = convertStr2Ord(tmp[2])
        print ("Got a Transmit request from" + str(addr))
        print ("Request file Type: " + str(fType) + " Size: " + str(fSize))
        return fType, fSize
    return None
    

class myUdpHandler(IUdpCallback):
    def __init__(self):
        self.client_list = {}
    def on_started(self, server):
        print ('UDP server is started')
    def on_stoped(self, server):
        print ('UDP server is stoped')
    def on_received(self, server, addr, data):
        print ("Received lenth: " + str(len(data)))
        print ("From addr:" + str(addr))

        dictIndex = str(addr[0])

        # check if is a 'handshake' msg
        if (checkHandshake(addr, data)):
            # use Client's IP as dict ID
            self.client_list[dictIndex] = {'status':"WAIT" }
        # check if is a 'starf transmission' msg
        elif dictIndex in self.client_list and self.client_list[dictIndex]['status'] == "WAIT":
            pair = checkFileTransmitStart(addr, data)
            if pair != None:
                (fType, fSize) = pair

                # Open a tcp port
                tcpServer = createRandomTcpServer()

                # remind client tcp's port
                _, tcpPort = tcpServer.getsockname()
                tmpUdp = createUdpSocket()
                tmpUdp.sendto("STA\x05:" + str(tcpPort), addr)
                tmpUdp.close()
                # Recv file
                data = recvfromTcpSocket(tcpServer)
                # TODO: Handle file according to the fType
                pass
    def on_sent(self, server, status, data):
        print ("Sent with status code (" + str(status) + ")")


if __name__ == "__main__":
    handler = myUdpHandler()
    bind_addr = ""
    server = AsyncUDP(9000, handler, bind_addr)