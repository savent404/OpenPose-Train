from pyserver.network import * # install by "pip install pyserver"
import socket
import numpy as np
import random
import json

def createJson(correct=False, done=False, msg="unknow"):
    ''' return a Json Obj as string that used to ack client'''
    doc = {}
    doc['correct'] = correct
    doc['done'] = done
    doc['msg'] = msg
    jsonString = json.dumps(doc)
    return jsonString

def createUdpSocket():
    ''' generate a UDP socket with random port, it's only used to transmit'''
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return s

def createRandomTcpServer():
    ''' generate a TCP socket with random port, it used to rece image file'''
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
    ''' a function to lisen TCP socket,
    and rece bytes till buffer has no more. '''

    d = ''
    while True:
        conn, addr = sock.accept()
        print ("Got connection: " + str(addr))
        if accept_addr is None:
            break
        elif accept_addr == addr[0]:
            break
        else :
            conn.close()
            continue
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
    ''' used to rece/send CTL msg with client.
    Attribute : client_list used to storage client's infomation'''

    def __init__(self):
        self.client_list = {}

    def on_started(self, server):
        print ('UDP server is started')

    def on_received(self, server, addr, data):
        ''' handle client's message, identify which client with their IP addr'''
        print ("Received lenth: " + str(len(data)))
        print ("From addr:" + str(addr))

        # convert IP addr to string, as the identfy ID.
        dictIndex = str(addr[0])

        # check if is a 'handshake' msg
        if (checkHandshake(addr, data)):
            # use Client's IP as dict ID
            self.client_list[dictIndex] = {'status':"WAIT" }
            return

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
                tmpUdp.sendto("STA\x05:" + str(tcpPort), (addr[0], 9001))
                tmpUdp.close()
                print ("tell client tcp port:" + str(tcpPort))
                # Recv file
                data = recvfromTcpSocket(tcpServer)
                if len(data) != fSize:
                    print("recv file size is not the same")
                    return
                
                # handle rece data with different fType
                # fType=1 is image(jpg)
                # fType=2 is the configuration Json file
                if fType == 1:
                    print('Reading image')
                    # read image from bytes
                    img = cv2.imdecode(np.fromstring(data, dtype=np.uint8), -1)
                    # to test rece a right imge: cv2.imwrite('/home/savent/1.jpg', img)

                    # TODO: Handle image information

                    # return a json for client
                    jsonStr = createJson()
                    
                    # transmit to Client with UDP port: 9001
                    tmpUdp = createUdpSocket()
                    tmpUdp.sendto("STA\x06:"+jsonStr, (addr[0], 9001))
                    tmpUdp.close()
                elif fType == 2:
                    print("Reading json")
                    config = json.loads(data)
                    print(config)
                    self.client_list[dictIndex]['width'] = config['width']
                    self.client_list[dictIndex]['height'] = config['height']
                    self.client_list[dictIndex]['type'] = config['type']
                    self.client_list[dictIndex]['interval'] = config['interval']
                    if self.client_list[dictIndex]['type'] == 3:
                        self.client_list[dictIndex]['angle'] = config['angle']
                        self.client_list[dictIndex]['stay'] = config['stay']
                else:
                    print("Unsupported file type")

    def on_sent(self, server, status, data):
        print ("Sent with status code (" + str(status) + ")")


if __name__ == "__main__":
    handler = myUdpHandler()
    bind_addr = ""
    server = AsyncUDP(9000, handler, bind_addr)