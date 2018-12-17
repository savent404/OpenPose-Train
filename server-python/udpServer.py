from pyserver.network import * # install by "pip install pyserver"
import socket
import random

def convertStr2Ord(string):
    """ convert a little-edition binary to int """
    data = 0
    cnt = 0
    for i in string:
        data |= ord(i) << cnt
        cnt += 1
    return data

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

class FileTransmitHandler(IUdpCallback):
    """ a async class to handle a file transmit sequence """
    def __init__(self, FileSize, FileType, callback=None):
        self.size = FileSize
        self.type = FileType
        self.count = 0
        self.frame = -1
        self.data = ""
        self.callback = callback
    def on_received(self, server, addr, data):
        str_frame = data[:2]
        data = data[2:]
        frame_num = ord(str_frame[0]) + ord(str_frame[1]) << 8
        if frame_num - self.frame != 1:
            print("Sequence error frame: " + str(frame_num) + " except: " + str(self.frame) + ", closing...")
        else:
            self.frame = frame_num
            self.data += data
            self.count += len(data)
            if self.count >= self.size:
                print("Recv file done")
                if self.callback != None:
                    self.callback(addr, self.type, self.data)

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
        if (checkHandshake(addr, data)):
            self.client_list[dictIndex] = {'status':"WAIT" }
        elif dictIndex in self.client_list and self.client_list[dictIndex]['status'] == "WAIT":
            pair = checkFileTransmitStart(addr, data)
            if pair != None:
                (fType, fSize) = pair
                handle = FileTransmitHandler(fSize, fType, callback=self.recv_callback)
                server = AsyncUDP(random.randint(9002, 9500), handle, addr[0])
                print ("Generated a recv udp, port: " + str(server.port))
                self.client_list[dictIndex]['server'] = server
                self.client_list[dictIndex]['status'] = "RECV"

                # Send a response to client notice the listen Port:server.port
                server.send(addr[0], 9001, "STA\x05")

    def on_sent(self, server, status, data):
        print ("Sent with status code (" + str(status) + ")")
    def recv_callback(self, addr, fType, fData):
        dictIndex = str(addr[0])
        if dictIndex in self.client_list and self.client_list[dictIndex]['status'] == "RECV":
            print ("Recv done")
            self.client_list[dictIndex]['server'].close()
            self.client_list[dictIndex]['status'] = "WAIT"

            if fType == 1:
                ''' json file '''
                pass
            elif fType == 2:
                ''' img file '''
                pass


if __name__ == "__main__":
    handler = myUdpHandler()
    bind_addr = ""
    server = AsyncUDP(9000, handler, bind_addr)