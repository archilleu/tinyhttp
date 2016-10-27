//---------------------------------------------------------------------------
#ifndef TINYHTTP_CODEC_H_
#define TINYHTTP_CODEC_H_
//---------------------------------------------------------------------------
#include <functional>
#include "../depend/net/include/callback.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

class Codec
{
public:
    using CallbackRequestMessage = std::function<void (const net::TCPConnPtr&, uint64_t)>;

    void set_callback_req_msg(CallbackRequestMessage&& callback) { callback_req_msg_ = std::move(callback); }

private:
    friend class HTTPServer;
    void OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t mic_secs);

    bool GetRequestHeader(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);
    bool GetRequestLines(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);

    bool CheckRequestMessage(const char* begin, int len);

    const char* FindCRLF(const char* start, size_t len) const;

    void SkipSP(const char* start, int len, int& idx)
    {
        if((isspace(start[idx]) && (idx<len)))
            idx++;
    }

private:
    CallbackRequestMessage callback_req_msg_;
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_CODEC_H_
