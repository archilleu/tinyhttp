//---------------------------------------------------------------------------
#include "request_message.h"
#include "codec.h"
#include "http_config.h"
#include "../depend/net/include/buffer.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
const char Codec::kCRLF[2] = {'\r', '\n'};
//---------------------------------------------------------------------------
void Codec::OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t rcv_time)
{
    //recv too much data
    if(MyHTTPConfig.max_header_size() < buffer.ReadableBytes())
    {
        if(callback_err_msg_)
            callback_err_msg_(tcp_conn, "connection recv too much data");

        tcp_conn->ForceClose();
        return;
    }

    //is new conn
    if(!tcp_conn->any_)
        tcp_conn->any_ = std::make_shared<RequestMessage>();

    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();

    int ostatus;
    do
    {
        ostatus = req_msg->status_;
        switch(req_msg->status_)
        {
            case RequestMessage::HEADER:
                GetRequestHeader(tcp_conn, buffer);
                break;

            case RequestMessage::LINES:
                GetRequestLines(tcp_conn, buffer);
                break;

            case RequestMessage::BODY:
                ReadBody(buffer, req_msg);
                break;

            case RequestMessage::OK:
                callback_req_msg_(tcp_conn, rcv_time);

                buffer.Retrieve(req_msg->rq_size_);
                req_msg->reset();
                return;

            case RequestMessage::FAILED:
                if(callback_err_msg_)
                    callback_err_msg_(tcp_conn, "request header or lines invaild");

                buffer.Retrieve(req_msg->rq_size_);
                req_msg->reset();

                return;

            default:
                assert(0);
                break;
        }

    }while(ostatus != req_msg->status_);

    return;
}
//---------------------------------------------------------------------------
void Codec::GetRequestHeader(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    //already have method
    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();
    if(RequestMessage::HEADER != req_msg->status_)
        return;

    char* begin = const_cast<char*>(buffer.Peek());
    const char* crlf = FindCRLF(begin, buffer.ReadableBytes());
    if(0 == crlf)
        return;

    int idx = 0;
    int len = static_cast<int>(crlf - begin);
    req_msg->rq_size_ += static_cast<int>(len + sizeof(kCRLF));

    ///|method|blank|URL|blank|ver|\r\n

    //method
    if(false == CheckRequestHeader(begin, len))
    {
        req_msg->status_ = RequestMessage::FAILED;
        return;
    }

    for(; idx<len; idx++) if(isspace(begin[idx]))break;
    req_msg->method_ = begin;
    begin[idx++] = 0;
    if(idx >= len)
    {
        req_msg->status_ = RequestMessage::FAILED;
        return;
    }

    //URL
    SkipSP(begin, len, idx);
    req_msg->url_ = begin + idx;
    for(; idx<len; idx++) if(isspace(begin[idx]))break;
    begin[idx++] = 0;
    if(idx >= len)
    {
        req_msg->status_ = RequestMessage::FAILED;
        return;
    }

    //ver
    SkipSP(begin, len, idx);
    req_msg->version_ = begin + idx;
    begin[len] = 0;
    
    req_msg->status_ = RequestMessage::LINES;
    return;
}
//---------------------------------------------------------------------------
void Codec::GetRequestLines(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    RequestMessage* req_msg = std::static_pointer_cast<RequestMessage>(tcp_conn->any_).get();

    //head method have nto parse
    if(RequestMessage::LINES != req_msg->status_)
        return;

    while(true)
    {
        //not enought data
        int readable = static_cast<int>(buffer.ReadableBytes() - req_msg->rq_size_);
        char* begin = const_cast<char*>(buffer.Peek()) + req_msg->rq_size_;
        const char* crlf = FindCRLF(begin, readable);
        if(0 == crlf)
            return;

        int len = static_cast<int>(crlf - begin);
        req_msg->rq_size_ += static_cast<int>(len + sizeof(kCRLF));

        //\r\n\r\n
        if(0 == len)
        {
            req_msg->status_ = RequestMessage::BODY;
            return;
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
            req_msg->status_ = RequestMessage::FAILED;
            return;
        }

        begin[idx] = 0;
        const char*& val = req_msg->req_lines_[begin];

        //skip ':'
        idx++;

        //val
        SkipSP(begin, len , idx);
        val = begin + idx;
        begin[len] = 0;

        continue;
    }
}
//---------------------------------------------------------------------------
bool Codec::CheckRequestHeader(const char* begin, int len)
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
void Codec::ReadBody(net::Buffer& buffer, RequestMessage* req_msg)
{
    if(RequestMessage::BODY != req_msg->status_)
        return;

    if(0 == req_msg->body_)
    {
        auto iter = req_msg->req_lines_.find(tinyhttp::RequestMessage::kContentLength);
        if(req_msg->req_lines_.end() != iter)
        {
            req_msg->body_ = const_cast<char*>(buffer.Peek() + req_msg->rq_size_);
            req_msg->body_len_ = atoi(iter->second);
            if(MyHTTPConfig.max_body_size() < req_msg->body_len_)
            {
                req_msg->status_ = RequestMessage::FAILED;
                return;
            }
        }
        else
        {
            req_msg->status_ = RequestMessage::OK;
            return;
        }
    }

    if(buffer.ReadableBytes() < static_cast<size_t>((req_msg->rq_size_+req_msg->body_len_)))
        return;

    req_msg->rq_size_ += req_msg->body_len_;
    req_msg->status_ = RequestMessage::OK;
    return;
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
