//---------------------------------------------------------------------------
#include <iostream>
#include "http_server.h"
#include "../depend/net/include/event_loop.h"
#include "../depend/net/include/tcp_server.h"
#include "../depend/net/include/inet_address.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

HTTPServer::HTTPServer()
{
}
//---------------------------------------------------------------------------
HTTPServer::~HTTPServer()
{
}
//---------------------------------------------------------------------------
void HTTPServer::Start()
{
    main_loop_ = std::make_shared<net::EventLoop>();
    main_loop_->set_sig_usr1_callback(std::bind(&HTTPServer::SignalUsr1, this));
    main_loop_->SetAsSignalHandleEventLoop();

    net::TCPServer tcp_server(main_loop_.get(), 9981);
    tcp_server.set_callback_connection(std::bind(&HTTPServer::OnConnection, this, std::placeholders::_1));
    tcp_server.set_callback_disconnection(std::bind(&HTTPServer::OnDisconnection, this, std::placeholders::_1));
    tcp_server.set_callback_read(std::bind(&Codec::OnRead, &codec_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    tcp_server.set_callback_write_complete(std::bind(&HTTPServer::OnWriteComplete, this, std::placeholders::_1));
    tcp_server.set_callback_high_water_mark(std::bind(&HTTPServer::OnHightWaterMark, this, std::placeholders::_1, std::placeholders::_2), 100);
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
void HTTPServer::OnRequestMessage(net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)rcv_time;
    codec_.DumpReq(tcp_conn);
}
//---------------------------------------------------------------------------
void HTTPServer::OnWriteComplete(const net::TCPConnPtr& tcp_conn)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort();
    std::cout << "complete" << std::endl;

    return;
}
//---------------------------------------------------------------------------
void HTTPServer::OnHightWaterMark(const net::TCPConnPtr& tcp_conn, size_t mark)
{
    std::cout << "name:" << tcp_conn->name() <<
        " local: " << tcp_conn->local_addr().IPPort() <<
        " peer: " << tcp_conn->peer_addr().IPPort();
    std::cout << "mark:" << mark << std::endl;

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
