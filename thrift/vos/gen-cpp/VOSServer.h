/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef VOSServer_H
#define VOSServer_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include "vos_types.h"



#ifdef _WIN32
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class VOSServerIf {
 public:
  virtual ~VOSServerIf() {}
  virtual void SendOrderInfo(const OrderInfo& oi) = 0;
};

class VOSServerIfFactory {
 public:
  typedef VOSServerIf Handler;

  virtual ~VOSServerIfFactory() {}

  virtual VOSServerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(VOSServerIf* /* handler */) = 0;
};

class VOSServerIfSingletonFactory : virtual public VOSServerIfFactory {
 public:
  VOSServerIfSingletonFactory(const boost::shared_ptr<VOSServerIf>& iface) : iface_(iface) {}
  virtual ~VOSServerIfSingletonFactory() {}

  virtual VOSServerIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(VOSServerIf* /* handler */) {}

 protected:
  boost::shared_ptr<VOSServerIf> iface_;
};

class VOSServerNull : virtual public VOSServerIf {
 public:
  virtual ~VOSServerNull() {}
  void SendOrderInfo(const OrderInfo& /* oi */) {
    return;
  }
};

typedef struct _VOSServer_SendOrderInfo_args__isset {
  _VOSServer_SendOrderInfo_args__isset() : oi(false) {}
  bool oi :1;
} _VOSServer_SendOrderInfo_args__isset;

class VOSServer_SendOrderInfo_args {
 public:

  VOSServer_SendOrderInfo_args(const VOSServer_SendOrderInfo_args&);
  VOSServer_SendOrderInfo_args& operator=(const VOSServer_SendOrderInfo_args&);
  VOSServer_SendOrderInfo_args() {
  }

  virtual ~VOSServer_SendOrderInfo_args() throw();
  OrderInfo oi;

  _VOSServer_SendOrderInfo_args__isset __isset;

  void __set_oi(const OrderInfo& val);

  bool operator == (const VOSServer_SendOrderInfo_args & rhs) const
  {
    if (!(oi == rhs.oi))
      return false;
    return true;
  }
  bool operator != (const VOSServer_SendOrderInfo_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const VOSServer_SendOrderInfo_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class VOSServer_SendOrderInfo_pargs {
 public:


  virtual ~VOSServer_SendOrderInfo_pargs() throw();
  const OrderInfo* oi;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

class VOSServerClient : virtual public VOSServerIf {
 public:
  VOSServerClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  VOSServerClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void SendOrderInfo(const OrderInfo& oi);
  void send_SendOrderInfo(const OrderInfo& oi);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class VOSServerProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<VOSServerIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (VOSServerProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_SendOrderInfo(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  VOSServerProcessor(boost::shared_ptr<VOSServerIf> iface) :
    iface_(iface) {
    processMap_["SendOrderInfo"] = &VOSServerProcessor::process_SendOrderInfo;
  }

  virtual ~VOSServerProcessor() {}
};

class VOSServerProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  VOSServerProcessorFactory(const ::boost::shared_ptr< VOSServerIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< VOSServerIfFactory > handlerFactory_;
};

class VOSServerMultiface : virtual public VOSServerIf {
 public:
  VOSServerMultiface(std::vector<boost::shared_ptr<VOSServerIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~VOSServerMultiface() {}
 protected:
  std::vector<boost::shared_ptr<VOSServerIf> > ifaces_;
  VOSServerMultiface() {}
  void add(boost::shared_ptr<VOSServerIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void SendOrderInfo(const OrderInfo& oi) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->SendOrderInfo(oi);
    }
    ifaces_[i]->SendOrderInfo(oi);
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class VOSServerConcurrentClient : virtual public VOSServerIf {
 public:
  VOSServerConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  VOSServerConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void SendOrderInfo(const OrderInfo& oi);
  void send_SendOrderInfo(const OrderInfo& oi);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  ::apache::thrift::async::TConcurrentClientSyncInfo sync_;
};

#ifdef _WIN32
  #pragma warning( pop )
#endif



#endif
