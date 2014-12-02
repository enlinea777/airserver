import socket
import struct

HOST = '10.1.1.182'
PORT = 4404

NUM = 40
CNT = 48000

s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST,PORT))

temp = ""
for i in range(NUM):
    temp += "s"

for i in range(CNT):
    length = 20 + len(temp)
    seq = 1
    cmd = 101
    ret = 0
    uid = 545155399

    format = "iiiii%ds" % len(temp)
    #print "format", format


    send_str = struct.pack(format, length, seq, cmd, ret, uid, temp)

    #print 'send_str:', len(send_str), send_str

    s.send(send_str)
    data = s.recv(1024)

    ##print 'length res:', len(data)
    #length,seq,cmd,ret,uid = struct.unpack_from("iiiii", data)

    #body = data[20:len(data)]

    #print body
    #print i

s.close()
