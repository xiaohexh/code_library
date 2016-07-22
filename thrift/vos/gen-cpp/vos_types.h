/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef vos_TYPES_H
#define vos_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




struct RequestType {
  enum type {
    MARKET_END = 1,
    BIZ_END = 2
  };
};

extern const std::map<int, const char*> _RequestType_VALUES_TO_NAMES;

class OrderInfo;

typedef struct _OrderInfo__isset {
  _OrderInfo__isset() : id(false), flag(false), fee(false), key(false), type(false) {}
  bool id :1;
  bool flag :1;
  bool fee :1;
  bool key :1;
  bool type :1;
} _OrderInfo__isset;

class OrderInfo {
 public:

  OrderInfo(const OrderInfo&);
  OrderInfo& operator=(const OrderInfo&);
  OrderInfo() : id(), flag(0), fee(0), key(), type((RequestType::type)0) {
  }

  virtual ~OrderInfo() throw();
  std::string id;
  int32_t flag;
  double fee;
  std::string key;
  RequestType::type type;

  _OrderInfo__isset __isset;

  void __set_id(const std::string& val);

  void __set_flag(const int32_t val);

  void __set_fee(const double val);

  void __set_key(const std::string& val);

  void __set_type(const RequestType::type val);

  bool operator == (const OrderInfo & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(flag == rhs.flag))
      return false;
    if (!(fee == rhs.fee))
      return false;
    if (!(key == rhs.key))
      return false;
    if (!(type == rhs.type))
      return false;
    return true;
  }
  bool operator != (const OrderInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const OrderInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(OrderInfo &a, OrderInfo &b);

inline std::ostream& operator<<(std::ostream& out, const OrderInfo& obj)
{
  obj.printTo(out);
  return out;
}



#endif
