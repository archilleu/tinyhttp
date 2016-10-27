//---------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory>
#include <unistd.h>
#include <cstring>
#include "config.h"
#include "method_web.h"
#include "request_message.h"
#include "codec.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace
{
    __thread char g_responese_header[1024];
    void ResponeseHeader(const net::TCPConnPtr& tcp_conn, const char* status_code, int body_len)
    {
        snprintf(g_responese_header, sizeof(g_responese_header), 
        "HTTP/1.1 %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n\r\n"
        "<P>Not Found</P>", status_code, body_len);

        tcp_conn->Send(g_responese_header, strlen(g_responese_header));
        return;
    }

    void ReplayNotFound(const net::TCPConnPtr& tcp_conn)
    {
        static const char reply[] = "<P>Not Found</P>";
        ResponeseHeader(tcp_conn, RequestMessage::kNotFound, sizeof(reply));
        tcp_conn->Send(reply, sizeof(reply));

        return;
    }

    void ReplayForbidden(const net::TCPConnPtr& tcp_conn)
    {
        static const char reply[] = "HTTP/1.1 403 FORBIDDEN\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 16\r\n\r\n"
            "<P>Forbidden</P>";

        tcp_conn->Send(reply, sizeof(reply));
        return;
    }

    __thread char g_buffer[1024*64];
    void ReplyFile(const net::TCPConnPtr& tcp_conn, const std::string& path)
    {
        struct stat st;
        if(-1 == stat(path.c_str(), &st))
        {
            ReplayNotFound(tcp_conn);
            return;
        }
        else
        {
            if(S_ISDIR(st.st_mode))
                ReplayForbidden(tcp_conn);

            int fd = open(path.c_str(), O_RDONLY);
            if(0 > fd)
                ReplayForbidden(tcp_conn);

            size_t size = st.st_size;
            while(0 < size)
            {
                ssize_t rlen = read(fd, g_buffer, sizeof(g_buffer));
                if(0 >= rlen)
                    break;

                size -= rlen;
                tcp_conn->Send(g_buffer, rlen);
            }

            close(fd);
        }
    }
}

namespace tinyhttp
{

//---------------------------------------------------------------------------
void MethodWeb::GET(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;

    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    //get current doc root
    std::string url = MyConfig.doc_root() + req_msg->url_;
    
    //send file
    ReplyFile(tcp_conn, url);

    return;
}
//---------------------------------------------------------------------------
void MethodWeb::POST(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::HEAD(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::OPTIONS(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::PUT(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::TRANCE(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::DELETE(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------
void MethodWeb::CONNECT(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)tcp_conn;
    (void)rcv_time;
}
//---------------------------------------------------------------------------

}//namepace tinyhttp
