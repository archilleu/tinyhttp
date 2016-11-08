//---------------------------------------------------------------------------
#include <iostream>
#include "../src/http_config.h"
#include "../src/codec.h"
#include "../src/request_message.h"
#include "../depend/base/include/function.h"
#include "../depend/base/include/memory_block.h"
#include "../depend/net/include/inet_address.h"
#include "../depend/net/include/buffer.h"
#include "../depend/net/include/event_loop.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace
{
    bool g_success = false;
    
    void RequestMessage(const net::TCPConnPtr& conn, uint64_t)
    {
        tinyhttp::RequestMessage* req_msg = std::static_pointer_cast<tinyhttp::RequestMessage>(conn->any_).get();
        req_msg->Dump();
        g_success = true;
        return;
    }

    void CodecError(const net::TCPConnPtr& , const std::string& msg)
    {
        std::cout << "msg:" << msg << std::endl;
        g_success = false;
        return;
    }

}
//---------------------------------------------------------------------------
int main(int, char**)
{
    tinyhttp::MyHTTPConfig.set_path("./cfg");
    tinyhttp::MyHTTPConfig.LoadCofig();
    tinyhttp::MyHTTPConfig.SaveCofig();

    //failed 1
    g_success = true;
    base::MemoryBlock mb1;
    if(false == base::LoadFile("../../test/test_file/get.head.f1", &mb1))
    {
        assert(0);
        return -1;
    }

    tinyhttp::Codec codec;
    codec.set_callback_req_msg(RequestMessage);
    codec.set_callback_err_msg(CodecError);

    net::Buffer buffer;
    char* begin = mb1.dat();
    net::TCPConnPtr conn = std::make_shared<net::TCPConn>(reinterpret_cast<net::EventLoop*>(0), "", 0, net::InetAddress(), net::InetAddress());
    for(size_t i=0; i<mb1.len(); i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }
    if(true == g_success)
    {
        assert(0);
        return -1;
    }
    g_success = true;

    //failed 2
    base::MemoryBlock mb2;
    if(false == base::LoadFile("../../test/test_file/get.head.f2", &mb2))
    {
        assert(0);
        return -1;
    }
    begin = mb2.dat();
    for(size_t i=0; i<mb2.len(); i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }
    if(true == g_success)
    {
        assert(0);
        return -1;
    }
    g_success = true;

    //failed 3
    base::MemoryBlock mb3;
    if(false == base::LoadFile("../../test/test_file/get.reqline.f", &mb3))
    {
        assert(0);
        return -1;
    }
    begin = mb3.dat();
    for(size_t i=0; i<mb3.len(); i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }
    if(true == g_success)
    {
        assert(0);
        return -1;
    }
    g_success = false;

    //success
    base::MemoryBlock mb;
    if(false == base::LoadFile("../../test/test_file/get", &mb))
    {
        assert(0);
        return -1;
    }
    begin = mb.dat();
    for(size_t i=0; i<mb.len(); i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }
    if(false == g_success)
    {
        assert(0);
        return -1;
    }
    g_success = false;

    //post failed 
    base::MemoryBlock pmbf;
    if(false == base::LoadFile("../../test/test_file/post.failed", &pmbf))
    {
        assert(0);
        return -1;
    }
    begin = pmbf.dat();
    for(size_t i=0; i<pmbf.len(); i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }

    //post success
    g_success = false;
    base::MemoryBlock pmb;
    if(false == base::LoadFile("../../test/test_file/post", &pmb))
    {
        assert(0);
        return -1;
    }
    begin = pmb.dat();
    for(size_t i=0; i<pmb.len()-2; i++)
    {
        //buffer.ReadFd(static_cast<int>(mb.len()), reinterpret_cast<int*>(mb.dat()));
        buffer.ReadFd(1, reinterpret_cast<int*>(begin+i));
        codec.OnRead(conn, buffer, 0);
    }
    if(false == g_success)
    {
        assert(0);
        return -1;
    }

    return 0;
}
//---------------------------------------------------------------------------
