# zmq subscribe end in pub-sub mode
# python sub.py tcp://127.0.0.1:2345  // tcp
# python sub.py ipc://pub_sub		  // ipc

import sys 
import time
import zmq

def main():
	if len(sys.argv) < 2:
		print "usage: subscriber <connect_to> [topic topic ...]"
		sys.exit(1)

	connect_to = sys.argv[1]
	topics = sys.argv[2:]

	ctx = zmq.Context()
	s = ctx.socket(zmq.SUB)
	s.connect(connect_to)

	# manage subscriptions
	if  not topics:
		print "Receiving message on All topics..."
		s.setsockopt(zmq.SUBSCRIBE, '')
	else:
		for t in topics:
			s.setsockopt(zmq.SUBSCRIBE, t) # filter topic, only recv spec topic
	print
	try:
		while True:
			topic, msg = s.recv_pyobj()
			print "Topic: %s, msg: %s" % (topic, msg)
	except KeyboardInterrupt:
		pass

	s.close()


if __name__ == "__main__":
	main()
