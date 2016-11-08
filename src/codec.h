//---------------------------------------------------------------------------
#ifndef TINYHTTP_CODEC_H_
#define TINYHTTP_CODEC_H_
//---------------------------------------------------------------------------
#include <functional>
#include <cstring>
#include "../depend/net/include/callback.h"
//---------------------------------------------------------------------------
namespace net
{
    class Buffer;
}
//---------------------------------------------------------------------------
namespace tinyhttp
{

class RequestMessage;

class Codec
{
public:
    using CallbackRequestMessage = std::function<void (const net::TCPConnPtr&, uint64_t)>;
    using CallbackCodecError = std::function<void (const net::TCPConnPtr&, const std::string& msg)>;

    void set_callback_req_msg(CallbackRequestMessage&& callback) { callback_req_msg_ = std::move(callback); }
    void set_callback_err_msg(CallbackCodecError&& callback) { callback_err_msg_ = std::move(callback); }

    void OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t mic_secs);//only for HTTPServer bind

private:
    void GetRequestHeader(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);
    void GetRequestLines(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);

    bool CheckRequestHeader(const char* begin, int len);
    void ReadBody(net::Buffer& buffer, RequestMessage* req_msg);

    const char* FindCRLF(const char* start, size_t len) const
    {
        return static_cast<const char*>(memmem(start, len, kCRLF, sizeof(kCRLF)));
    }

    void SkipSP(const char* start, int len, int& idx)
    {
        if((isspace(start[idx]) && (idx<len)))
            idx++;
    }

private:
    CallbackRequestMessage callback_req_msg_;
    CallbackCodecError callback_err_msg_;

private:
    static const char kCRLF[2];
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_CODEC_H_
