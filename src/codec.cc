//---------------------------------------------------------------------------
#include "request_message.h"
#include "codec.h"
#include "../depend/net/include/buffer.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
//\r\n
namespace
{
    const char kCRLF[] = {'\r', '\n'};
}
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
void Codec::OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t rcv_time)
{
    //is new conn
    if(!tcp_conn->any_)
        tcp_conn->any_ = std::make_shared<RequestMessage>();

    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();

    //read request header
    if(false == GetRequestHeader(tcp_conn, buffer))
    {
        callback_req_msg_(tcp_conn, rcv_time);
        buffer.Retrieve(req_msg->rq_size_);
        req_msg->reset();
        return;
    }

    //read request line
    if(false == GetRequestLines(tcp_conn, buffer))
        return;

    //check has body?
    auto iter = req_msg->req_lines_.find(RequestMessage::kContentLength);
    if(req_msg->req_lines_.end() != iter)
    {
        req_msg->body_      = const_cast<char*>(buffer.Peek() + req_msg->rq_size_);
        req_msg->body_len_  = atoi(iter->second);
        if(buffer.ReadableBytes() < static_cast<size_t>((req_msg->rq_size_+req_msg->body_len_)))
            return;

        req_msg->rq_size_ += req_msg->body_len_;
    }

    callback_req_msg_(tcp_conn, rcv_time);
    buffer.Retrieve(req_msg->rq_size_);
    req_msg->reset();
    
    return;
}
//---------------------------------------------------------------------------
bool Codec::GetRequestHeader(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    //already have method
    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    if(0 != req_msg->method_)
        return true;

    char* begin = const_cast<char*>(buffer.Peek());
    const char* crlf = FindCRLF(begin, buffer.ReadableBytes());
    if(0 == crlf)
        return true;

    int idx = 0;
    int len = static_cast<int>(crlf - begin);

    ///|method|blank|URL|blank|ver|\r\n

    //method
    if(false == CheckRequestMessage(begin, len)) return false;
    for(; idx<len; idx++) if(isspace(begin[idx]))break;
    req_msg->method_ = begin;
    begin[idx++] = 0;

    //URL
    SkipSP(begin, len, idx);
    req_msg->url_ = begin + idx;
    for(; idx<len; idx++) if(isspace(begin[idx]))break;
    begin[idx++] = 0;

    //ver
    SkipSP(begin, len, idx);
    req_msg->version_ = begin + idx;
    begin[len] = 0;

    req_msg->rq_size_ += static_cast<int>(len + sizeof(kCRLF));

    if((0==req_msg->method_) || (0==req_msg->url_) || (0==req_msg->version_))
    {
        req_msg->method_ = 0;
        return false;
    }
    
    return true;;
}
//---------------------------------------------------------------------------
bool Codec::GetRequestLines(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();

    //head method have nto parse
    if(0 == req_msg->method_)
        return false;

    while(true)
    {
        //not enought data
        int readable = static_cast<int>(buffer.ReadableBytes() - req_msg->rq_size_);
        char* begin = const_cast<char*>(buffer.Peek()) + req_msg->rq_size_;
        const char* crlf = FindCRLF(begin, readable);
        if(0 == crlf)
            return false;

        int len = static_cast<int>(crlf - begin);

        //\r\n\r\n
        if(0 == len)
        {
            req_msg->rq_size_ += static_cast<int>(sizeof(kCRLF));
            return true;
        }

        ///|head name|:| val|\r\n

        int idx = 0;

        //head name
        for(; idx<len; idx++)
        {
            if(':' == (begin[idx]))
                break;
        }
        
        //can't find ':'
        if(idx == len)
        {
            req_msg->method_ = 0;
            req_msg->rq_size_ += static_cast<int>(len + sizeof(kCRLF));
            return true;
        }

        begin[idx] = 0;
        const char*& val = req_msg->req_lines_[begin];

        //skip ':'
        idx++;

        //val
        SkipSP(begin, len , idx);
        val = begin + idx;
        begin[len] = 0;
        req_msg->rq_size_ += static_cast<int>(len + sizeof(kCRLF));
    }
}
//---------------------------------------------------------------------------
bool Codec::CheckRequestMessage(const char* begin, int len)
{
    //max method len
    if(8 > len) return false;
    if(0 == memcmp(begin, RequestMessage::kGET,     strlen(RequestMessage::kGET)))      return true;
    if(0 == memcmp(begin, RequestMessage::kPOST,    strlen(RequestMessage::kPOST)))     return true;
    if(0 == memcmp(begin, RequestMessage::kHEAD,    strlen(RequestMessage::kHEAD)))     return true;
    if(0 == memcmp(begin, RequestMessage::kPUT,     strlen(RequestMessage::kPUT)))      return true;
    if(0 == memcmp(begin, RequestMessage::kOPTIONS, strlen(RequestMessage::kOPTIONS)))  return true;
    if(0 == memcmp(begin, RequestMessage::kTRACE,   strlen(RequestMessage::kTRACE)))    return true;
    if(0 == memcmp(begin, RequestMessage::kDELETE,  strlen(RequestMessage::kDELETE)))   return true;
    if(0 == memcmp(begin, RequestMessage::kCONNECT, strlen(RequestMessage::kCONNECT)))  return true;
    
    return false;
}
//---------------------------------------------------------------------------
const char* Codec::FindCRLF(const char* begin, size_t len) const
{
    return static_cast<const char*>(memmem(begin, len, kCRLF, sizeof(kCRLF)));
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
