#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include "VOSServer.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace boost;
int main(int argc, char** argv) {
  shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  VOSServerClient client(protocol);
  try {
    transport->open();
        OrderInfo oi;
        oi.id = "7x98i1";
        oi.flag = 1;
        oi.fee = 3.1245;
        oi.type = RequestType::BIZ_END;
        client.SendOrderInfo(oi);
    transport->close();

  } catch (TException &tx) {

    printf("ERROR: %s\n", tx.what());

  }

  return 0;
}
