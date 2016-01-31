'''
	Name: Who are lying?
	Idea: Alice Zhang
	Author: Legend.X
	E-mail: w93163red@163.com
	Thanks: Teacher Shi Kai
	Instruction: This is just a prototype. This program CAN NOT deal with any illegal input and failure.  
'''

import socket  

HOST = '192.168.199.150'    
PORT = 10000              
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
poker = ""

while 1:
	data = s.recv(512)
	opt = data.split()[0]

	if (opt == 'BROADCAST'):
		print "User %s has claimed his poker(s) is %s" % (data.split()[1], data.split()[2])
		
	elif (opt == 'FORCE'):
		if (poker == ''):
			poker = data.split()[1]
		else:
			poker = poker + data.split()[1]
		poker = ''.join(sorted(poker))
		print "Current Pokers: " + poker + '\n'

	elif (opt == 'TURN'):
		user_order = raw_input("Usage: PUT [poker]\n")
		s.send(user_order)
		poker_delete = user_order.split()[1]
		list_poker_delete = list(poker_delete)
		list_poker = list(poker)
		for i in list_poker_delete:
			list_poker.remove(i)
		poker = ''.join(list_poker)
		print "Current Pokers: " + poker + '\n'
		user_order = raw_input("Usage: CLAIM [poker]\n")
		s.send(user_order)
	elif (opt == 'JUDGE'):
		user_order = raw_input("JUDGE: Is it true? Type F for False, Type T for True\n")
		if (user_order == "F"):
			s.send("SAY FALSE")
		else:
			s.send("SAY TRUE")
	elif (poker == ""):
		s.send("OVER")
		break

s.close()
