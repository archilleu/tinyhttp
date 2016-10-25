//---------------------------------------------------------------------------
#ifndef TINYHTTP_CODEC_H_
#define TINYHTTP_CODEC_H_
//---------------------------------------------------------------------------
#include <string>
#include <cctype>
#include <map>
#include <functional>
#include "../depend/net/include/callback.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

class Codec
{
public:
    using RequestLines = std::map<std::string, std::string>;
    using CallbackRequestMessage = std::function<void (const net::TCPConnPtr&, uint64_t)>;

    void set_callback_req_msg(CallbackRequestMessage&& callback) { callback_req_msg_ = std::move(callback); }

    //for debug
    void DumpReq(const net::TCPConnPtr& tcp_conn);

public:
    //status code
    //1xx
    //2xx
    static const char* kOK;
    //3xx
    //4xx
    static const char* kBadRequest;
    //5xx
    static const char* kNotImplemented;

    //header
    static const char* kMethod;
    static const char* kURL;
    static const char* kReqVersion;

    //request line header
    static const char* kAccept;
    static const char* kAcceptCharset;
    static const char* kAcceptEncoding;
    static const char* kAcceptLanguage;
    static const char* kAcceptRanges;
    static const char* kAuthorization;
    static const char* kCacheControl;
    static const char* kConnection;
    static const char* kCookie;
    static const char* kContentLength;
    static const char* kContentType;
    static const char* kDate;
    static const char* kExpect;
    static const char* kFrom;
    static const char* kHost;
    static const char* kIfMatch;
    static const char* kIfModifiedSince;
    static const char* kIfNoneMatch;
    static const char* kIfRange;
    static const char* kIfUnmodifiedSince;
    static const char* kMaxForwards;
    static const char* kPragma;
    static const char* kProxyAuthorization;
    static const char* kRange;
    static const char* kReferer;
    static const char* kTE;
    static const char* kUpgrade;
    static const char* kUserAgent;
    static const char* kVia;
    static const char* kWarning;

    //responeses header 
    static const char* kResponeseVersion;
    static const char* kResponeseStatus;
    static const char* kCurVer;

    //responses header
    static const char* kAge;
    static const char* kAllow;
    static const char* kContentEncoding;
    static const char* kContentLanguage;
    static const char* kContentLocation;
    static const char* kContentMD5;
    static const char* kContentRange;
    static const char* kETag;
    static const char* kExpires;
    static const char* kLastModified;
    static const char* kLocation;
    static const char* kProxyAuthenticate;
    static const char* krefresh;
    static const char* kRetryAfter;
    static const char* kServer;
    static const char* kSetCookie;
    static const char* kTrailer;
    static const char* kTransferEncoding;
    static const char* kVary;
    static const char* kWWWAuthenticate;

private:
    friend class HTTPServer;
    void OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t mic_secs);

    bool GetRequestHeader   (const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);
    bool GetRequestLines    (const net::TCPConnPtr& tcp_conn, net::Buffer& buffer);

    const char* FindCRLF(const net::Buffer& buffer) const;

    void SkipSP(const char* start, int len, int& idx)
    {
        if((isspace(start[idx]) && (idx<len)))
            idx++;
    }

private:
    enum
    {
        SUCEESS = 1,
        CONTINUE,
        ERROR
    };

private:
    CallbackRequestMessage callback_req_msg_;

private:
    static const char kCRLF[];
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_CODEC_H_
