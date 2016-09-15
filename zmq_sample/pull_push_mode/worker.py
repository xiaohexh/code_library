import sys
import time
import zmq

def worker():
	ctx = zmq.Context()

	receiver = ctx.socket(zmq.PULL)
	receiver.connect("tcp://127.0.0.1:5557")

	sender = ctx.socket(zmq.PUSH)
	sender.connect("tcp://127.0.0.1:5558")

	try:
		while True:

			s = receiver.recv()

			# simple progress indicator for the viewer
			sys.stdout.write(".")
			sys.stdout.flush()

			# Do the work
			time.sleep(int(s)*0.001)

			# Send results to sink
			sender.send('from msg')
	except KeyboardInterrupt:
		print "exiting..."


if __name__ == "__main__":
	worker()
