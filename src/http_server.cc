//---------------------------------------------------------------------------
#include <iostream>
#include <memory>
#include "method.h"
#include "http_server.h"
#include "request_message.h"
#include "../depend/net/include/event_loop.h"
#include "../depend/net/include/tcp_server.h"
#include "../depend/net/include/inet_address.h"
#include "../depend/net/include/tcp_connection.h"
#include "../depend/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
namespace
{

//---------------------------------------------------------------------------
void ReplyBadRequest(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "HTTP/1.1 400 Bad Request\r\n"
        "Content-type: text/html\r\n"
        "Content-Length: 18\r\n\r\n"
        "<P>Bad Request</p>";

    tcp_conn->Send(reply, sizeof(reply));
    tcp_conn->ForceClose();
    return;
}
//---------------------------------------------------------------------------
void ReplyNotImplemented(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "HTTP/1.1 501 Not Implemented\r\n"
        "Content-type: text/html\r\n"
        "Content-Length: 21\r\n\r\n"
        "<P>Not Implemented<P>";

    tcp_conn->Send(reply, sizeof(reply));
    tcp_conn->ForceClose();
    return;
}
//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
HTTPServer::HTTPServer(Method* method)
:   method_(method)
{
}
//---------------------------------------------------------------------------
HTTPServer::~HTTPServer()
{
}
//---------------------------------------------------------------------------
void HTTPServer::Start()
{
    assert(method_);
    net::EventLoop::SetLogger("/tmp/logger", net::EventLoop::TRACE);

    main_loop_ = std::make_shared<net::EventLoop>();
    main_loop_->set_sig_usr1_callback(std::bind(&HTTPServer::SignalUsr1, this));
    main_loop_->SetAsSignalHandleEventLoop();

    net::TCPServer tcp_server(main_loop_.get(), 9981);
    tcp_server.set_callback_connection(std::bind(&HTTPServer::OnConnection, this, std::placeholders::_1));
    tcp_server.set_callback_disconnection(std::bind(&HTTPServer::OnDisconnection, this, std::placeholders::_1));
    tcp_server.set_callback_read(std::bind(&Codec::OnRead, &codec_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    tcp_server.set_callback_write_complete(std::bind(&HTTPServer::OnWriteComplete, this, std::placeholders::_1));
    tcp_server.set_callback_high_water_mark(std::bind(&HTTPServer::OnHightWaterMark, this, std::placeholders::_1, std::placeholders::_2), 100);

    codec_.set_callback_req_msg(std::bind(&HTTPServer::OnRequestMessage, this, std::placeholders::_1, std::placeholders::_2));

    tcp_server.Start();
    main_loop_->Loop();

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::Stop()
{
}
//---------------------------------------------------------------------------
void HTTPServer::OnConnection(const net::TCPConnPtr& tcp_conn)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort() << std::endl;

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::OnDisconnection(const net::TCPConnPtr& tcp_conn)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort() << std::endl;

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::OnRequestMessage(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    std::cout << "recv time:" << base::Timestamp(rcv_time).Datetime(true) << std::endl;

    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();

    if(0 == req_msg->method_)
    {
        ReplyBadRequest(tcp_conn);
        return;
    }

    if(!strcmp(RequestMessage::kGET, req_msg->method_))
    {
        method_->GET(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kPOST, req_msg->method_))
    {
        method_->POST(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kHEAD, req_msg->method_))
    {
        method_->HEAD(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kPUT, req_msg->method_))
    {
        method_->PUT(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kDELETE, req_msg->method_))
    {
        method_->DELETE(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kTRACE, req_msg->method_))
    {
        method_->TRANCE(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kCONNECT, req_msg->method_))
    {
        method_->CONNECT(tcp_conn, rcv_time);
        return;
    }

    if(!strcmp(RequestMessage::kOPTIONS, req_msg->method_))
    {
        method_->OPTIONS(tcp_conn, rcv_time);
        return;
    }

    ReplyNotImplemented(tcp_conn);

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::OnWriteComplete(const net::TCPConnPtr& tcp_conn)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort();
    std::cout << " complete" << std::endl;

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::OnHightWaterMark(const net::TCPConnPtr& tcp_conn, size_t mark)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort();
    std::cout << " mark:" << mark << std::endl;

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::SignalUsr1()
{
    main_loop_->Quit();
    return;
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
