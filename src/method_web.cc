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
__thread char t_responese_header[1024*2];
__thread char t_file_buffer[1024*64];
__thread char t_time_format[64];
//---------------------------------------------------------------------------
int TYPE_TEXT_HTML      = 1 << 0;
int TYPE_CHARSET_UTF8   = 1 << 1;
//---------------------------------------------------------------------------
const char* kType[sizeof(int)]=
{
    "","text/html;","charset=UTF-8;"
};
//---------------------------------------------------------------------------
const char* DateNowHTTPFormat()
{
    time_t now = time(0);
    struct tm tm;
    gmtime_r(&now, &tm);
    strftime(t_time_format, sizeof(t_time_format), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return t_time_format;
}
//---------------------------------------------------------------------------
std::string ContentType(int type)
{
    std::string ret;

    if(0 == type)
        return ret;

    if(type & TYPE_TEXT_HTML)
        ret += kType[TYPE_TEXT_HTML];
    if(type & TYPE_CHARSET_UTF8)
        ret += kType[TYPE_CHARSET_UTF8];

    ret.pop_back();
    return ret;
}
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
    ResponeseCommonHeader(tcp_conn, RequestMessage::kOK, TYPE_TEXT_HTML|TYPE_CHARSET_UTF8, 0);
}
//---------------------------------------------------------------------------
void MethodWeb::HEAD(const net::TCPConnPtr& tcp_conn, uint64_t rcv_time)
{
    (void)rcv_time;
    const RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    req_msg->Dump();

    //get current doc root
    std::string url = MyHTTPConfig.doc_root() + req_msg->url_;

    //send head
    ResponeseHead(tcp_conn, url);
    return;
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
void MethodWeb::ResponeseCommonHeader(const net::TCPConnPtr& tcp_conn, const char* status_code, int type,  int body_len)
{
    snprintf(t_responese_header, sizeof(t_responese_header), 
    "HTTP/1.1 %s\r\n"
    "Server:tinyhttp\r\n"
    "Date:%s\r\n"
    "Content-Type:%s\r\n"
    "Content-Length:%d\r\n\r\n",
    status_code, DateNowHTTPFormat(), ContentType(type).c_str(), body_len);

    tcp_conn->Send(t_responese_header, strlen(t_responese_header));
    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseNotFound(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "<P>Not Found</P>";
    ResponeseCommonHeader(tcp_conn, RequestMessage::kNotFound, TYPE_TEXT_HTML|TYPE_CHARSET_UTF8, sizeof(reply));
    tcp_conn->Send(reply, sizeof(reply));

    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseForbidden(const net::TCPConnPtr& tcp_conn)
{
    static const char reply[] = "<P>Forbidden</P>";
    ResponeseCommonHeader(tcp_conn, RequestMessage::kForbidden, TYPE_TEXT_HTML|TYPE_CHARSET_UTF8, sizeof(reply));
    tcp_conn->Send(reply, sizeof(reply));

    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseHead(const net::TCPConnPtr& tcp_conn, const std::string& path)
{
    int file_size = CheckURL(tcp_conn, path);
    if(-1 == file_size)
        return;

    //responese head
    ResponeseCommonHeader(tcp_conn, RequestMessage::kOK, TYPE_TEXT_HTML|TYPE_CHARSET_UTF8, 0);
    return;
}
//---------------------------------------------------------------------------
void MethodWeb::ResponeseFile(const net::TCPConnPtr& tcp_conn, const std::string& path)
{
    int file_size = CheckURL(tcp_conn, path);
    if(-1 == file_size)
        return;

    //responese  header
    ResponeseCommonHeader(tcp_conn, RequestMessage::kOK, TYPE_TEXT_HTML|TYPE_CHARSET_UTF8, file_size);

    //read file
    int fd = open(path.c_str(), O_RDONLY);
    if(0 > fd)
        ResponeseForbidden(tcp_conn);

    size_t size = file_size;
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

    return;
}
//---------------------------------------------------------------------------
int MethodWeb::CheckURL(const net::TCPConnPtr& tcp_conn, const std::string& path)
{
    struct stat st;
    if(-1 == stat(path.c_str(), &st))
    {
        ResponeseNotFound(tcp_conn);
        return -1;
    }

    //directory?
    if(S_ISDIR(st.st_mode))
    {
        ResponeseForbidden(tcp_conn);
        return -1;
    }

    return static_cast<int>(st.st_size);
}
//---------------------------------------------------------------------------

}//namepace tinyhttp
