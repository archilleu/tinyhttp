//---------------------------------------------------------------------------
#ifndef TINYHTTP_METHOD_WEB_H_
#define TINYHTTP_METHOD_WEB_H_
//---------------------------------------------------------------------------
#include "method.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

class MethodWeb : public Method
{
public:
    virtual void GET        (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void POST       (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void HEAD       (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void OPTIONS    (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void PUT        (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void TRANCE     (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void DELETE     (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    virtual void CONNECT    (const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_METHOD_WEB_H_
