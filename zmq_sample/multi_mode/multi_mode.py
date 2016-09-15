import zmq

ctx = zmq.Context()

receiver = ctx.socket(zmq.PULL)
receiver.connect("tcp://127.0.0.1:5558")

subscriber = ctx.socket(zmq.SUB)
subscriber.connect("tcp://127.0.0.1:5556")
subscriber.setsockopt(zmq.SUBSCRIBE, "")
#subscriber.setsockopt(zmq.SUBSCRIBE, "sports.football")

poller = zmq.Poller()
poller.register(receiver, zmq.POLLIN)
poller.register(subscriber, zmq.POLLIN)

while True:
	socks = dict(poller.poll())

	if receiver in socks and socks[receiver] == zmq.POLLIN:
		message = receiver.recv()
		print "recv msg: %s" % message

	if subscriber in socks and socks[subscriber] == zmq.POLLIN:
		topic, message = subscriber.recv_pyobj()
		#print "%s: %s" % (topic, message)

