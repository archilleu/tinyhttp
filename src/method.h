//---------------------------------------------------------------------------
#ifndef TINYHTTP_METHOD_H_
#define TINYHTTP_METHOD_H_
//---------------------------------------------------------------------------
#include <cinttypes>
//---------------------------------------------------------------------------
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace tinyhttp 
{

class RequestMessage;

class Method
{
public:
    virtual void GET        (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void POST       (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void HEAD       (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void OPTIONS    (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void PUT        (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void TRANCE     (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void DELETE     (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
    virtual void CONNECT    (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time) = 0;
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //define TINYHTTP_CALLBACK_H_
