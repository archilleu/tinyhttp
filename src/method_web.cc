//---------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory>
#include <unistd.h>
#include <cstring>
#include "http_config.h"
#include "method_web.h"
#include "request_message.h"
#include "codec.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
__thread char t_responese_header[1024];
__thread char t_file_buffer[1024*64];
//---------------------------------------------------------------------------
void MethodWeb::GET(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)rcv_time;
    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    req_msg->Dump();

    //get current doc root
    std::string url = MyHTTPConfig.doc_root() + req_msg->url_;
    
    //send file
    ResponeseFile(tcp_conn, url);

    return;
}
//---------------------------------------------------------------------------
void MethodWeb::POST(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)rcv_time;
    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    req_msg->Dump();
    ResponeseHeader(tcp_conn, RequestMessage::kOK, 0);
}
//---------------------------------------------------------------------------
void MethodWeb::HEAD(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)rcv_time;
    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    req_msg->Dump();
    ResponeseHeader(tcp_conn, RequestMessage::kOK, 0);
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
void MethodWeb::ResponeseHeader(const net::TCPConnPtr& tcp_conn, const char* status_code, int body_len)
{
    snprintf(t_responese_header, sizeof(t_responese_header), 
    "HTTP/1.1 %s\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: %d\r\n\r\n",
    status_code, body_len);

    tcp_conn->Send(t_responese_header, strlen(t_responese_header));
    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseNotFound(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "<P>Not Found</P>";
    ResponeseHeader(tcp_conn, RequestMessage::kNotFound, sizeof(reply));
    tcp_conn->Send(reply, sizeof(reply));

    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseForbidden(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "<P>Forbidden</P>";
    ResponeseHeader(tcp_conn, RequestMessage::kForbidden, sizeof(reply));
    tcp_conn->Send(reply, sizeof(reply));

    return;
}

//---------------------------------------------------------------------------
void MethodWeb::ResponeseFile(const net::TCPConnPtr& tcp_conn, const std::string& path)
{
    struct stat st;
    if(-1 == stat(path.c_str(), &st))
    {
        ResponeseNotFound(tcp_conn);
        return;
    }
    else
    {
        //directory?
        if(S_ISDIR(st.st_mode))
            ResponeseForbidden(tcp_conn);

        //responese  header
        ResponeseHeader(tcp_conn, RequestMessage::kOK, static_cast<int>(st.st_size));

        //read file
        int fd = open(path.c_str(), O_RDONLY);
        if(0 > fd)
            ResponeseForbidden(tcp_conn);

        size_t size = st.st_size;
        while(0 < size)
        {
            ssize_t rlen = read(fd, t_file_buffer, sizeof(t_file_buffer));
            if(0 >= rlen)
                break;

            size -= rlen;
            tcp_conn->Send(t_file_buffer, rlen);
        }

        //read file fail
        if(0 != size)
        {
            tcp_conn->ForceClose();
        }

        close(fd);
    }

    return;
}
//---------------------------------------------------------------------------

}//namepace tinyhttp
