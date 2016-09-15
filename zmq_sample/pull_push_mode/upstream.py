#
import zmq
import random 
import time

def push():

	ctx = zmq.Context()

	sender = ctx.socket(zmq.PUSH)
	sender.bind("tcp://*:5557")

	print "Press enter when the workers are ready: "
	_ = raw_input()
	print "Sending tasks to workers..."

	# first message is "0" and signals start of batch
	sender.send('0')

	random.seed()

	total_msec = 0
	for task_nbr in range(100):
		workload = random.randint(1, 100)
		total_msec += workload
		sender.send(str(workload))

	print "Total expected cost: %s msec" % total_msec


if __name__ == "__main__":
	push()
