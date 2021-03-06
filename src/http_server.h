//---------------------------------------------------------------------------
#ifndef TINYHTTP_HTTPSERVER_H_
#define TINYHTTP_HTTPSERVER_H_
//---------------------------------------------------------------------------
#include <memory>
#include "codec.h"
//---------------------------------------------------------------------------
namespace net
{
    class EventLoop;
    class TCPServer;
};
//---------------------------------------------------------------------------
namespace tinyhttp
{

class Method;
class RequestMessage;

class HTTPServer
{
public:
    HTTPServer(Method* method);
    ~HTTPServer();

    void Start();
    void Stop();

private:
    void OnConnection(const net::TCPConnPtr& tcp_conn);
    void OnDisconnection(const net::TCPConnPtr& tcp_conn);
    void OnRequestMessage(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time);
    void OnWriteComplete(const net::TCPConnPtr& tcp_conn);
    void OnHightWaterMark(const net::TCPConnPtr& tcp_conn, size_t mark);

    void OnCodecError(const net::TCPConnPtr& tcp_conn, const std::string& msg);

private:
    void SignalUsr1();

private:
    std::shared_ptr<net::EventLoop> main_loop_;
    std::shared_ptr<net::TCPServer> tcp_server_;

    Codec codec_;
    Method* method_;
};

}//namepace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_HTTPSERVER_H_
