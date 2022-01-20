import socket
import struct
import sys
import numpy as np
from matplotlib import pyplot as plt

server_address = ('localhost', 5100)
packet_len = 255 + 7
A = np.zeros((128,128), dtype = np.uint32)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect(server_address)
	s.setblocking(1)
	print("Connected to {:s}".format(repr(server_address)))
	print ("connection established")
	j = 0
	X = 0
	Y = 0
	while True:
		print ("\nj: ", j, "\n")
		data = s.recv(packet_len)
		j += 1
		if data:
			msgdata = data.decode('UTF-8')
			msgstart = msgdata.find('START: ')
			if msgstart != -1:
				numXY = ord(msgdata[msgstart + 7]);
				print ('numXY: ', numXY)
				for i in range(msgstart+8, min(msgstart + 8 + 2*numXY, len(data) - 1), 2):
					X = ord(msgdata[i])
					Y = ord(msgdata[i+1])
					if X >=0 & X <= 127 & Y >= 0 & Y <= 127:
						A[X,Y] += 1
		if not data:
			break
	plt.imshow(A)
	plt.show()
