//---------------------------------------------------------------------------
#include <cstring>
#include <iostream>
#include "codec.h"
#include "../depend/net/include/buffer.h"
#include "../depend/net/include/tcp_connection.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
//request header
const char* Codec::kMethod  = "Method";
const char* Codec::kURL     = "URL";
const char* Codec::kReqVersion = "version";
//---------------------------------------------------------------------------
//common header

const char* Codec::kConnection         = "Connection";              // close
const char* Codec::kDate               = "Date";                    // Tue, 15 Nov 2010 08:12:31 GMT
const char* Codec::kTrailer            = "Trailer";                 // Max-Forwards
const char* Codec::kTransferEncoding   = "Transfer-Encoding";       // chunked
const char* Codec::kVia                = "Via";                     // 1.0 fred, 1.1 nowhere.com (Apache/1.1)
const char* Codec::kCacheControl       = "Cache-Control";           // no-cache
//---------------------------------------------------------------------------
//request header line
const char* Codec::kAccept             = "Accept";                  // text/plain, text/html
const char* Codec::kAcceptCharset      = "Accept-Charset";          // iso-8859-5
const char* Codec::kAcceptEncoding     = "Accept-Encoding";         // compress, gzip
const char* Codec::kAcceptLanguage     = "Accept-Language";         // en,zh
const char* Codec::kAcceptRanges       = "Accept-Ranges";           // bytes
const char* Codec::kAuthorization      = "Authorization";           // Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
const char* Codec::kCookie             = "Cookie";                  // $Version=1; Skin=new;
const char* Codec::kContentLength      = "Content-Length";          // 348
const char* Codec::kContentType        = "Content-Type";            // application/x-www-form-urlencoded
const char* Codec::kExpect             = "Expect";                  // 100-continue
const char* Codec::kFrom               = "From";                    // user@email.com
const char* Codec::kHost               = "Host";                    // www.zcmhi.com
const char* Codec::kIfMatch            = "If-Match";                // “737060cd8c284d8af7ad3082f209582d”
const char* Codec::kIfModifiedSince    = "If-Modified-Since";       // Sat, 29 Oct 2010 19:43:31 GMT
const char* Codec::kIfNoneMatch        = "If-None-Match";           // “737060cd8c284d8af7ad3082f209582d”
const char* Codec::kIfRange            = "If-Range";                // “737060cd8c284d8af7ad3082f209582d”
const char* Codec::kIfUnmodifiedSince  = "If-Unmodified-Since";     // Sat, 29 Oct 2010 19:43:31 GMT
const char* Codec::kMaxForwards        = "Max-Forwards";            // 10
const char* Codec::kPragma             = "Pragma";                  // no-cache
const char* Codec::kProxyAuthorization = "Proxy-Authorization";     // Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
const char* Codec::kRange              = "Range";                   // bytes=500-999
const char* Codec::kReferer            = "Referer";                 // http://www.zcmhi.com/archives/71.html
const char* Codec::kTE                 = "TE";                      // trailers,deflate;q=0.5
const char* Codec::kUpgrade            = "Upgrade";                 // HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11
const char* Codec::kUserAgent          = "User-Agent";              // Mozilla/5.0 (Linux; X11)
const char* Codec::kWarning            = "Warn";                    // 199 Miscellaneous warning
//---------------------------------------------------------------------------
//responses header
const char* Codec::kAge                = "Age";                 // 12
const char* Codec::kAllow              = "Allow";               // GET, HEAD
const char* Codec::kContentEncoding    = "Content-Encoding";    // gzip
const char* Codec::kContentLanguage    = "Content-Language";    // en,zh
const char* Codec::kContentLocation    = "Content-Location";    // /index.htm
const char* Codec::kContentMD5         = "Content-MD5";         // Q2hlY2sgSW50ZWdyaXR5IQ==
const char* Codec::kContentRange       = "Content-Range";       // bytes 21010-47021/47022
const char* Codec::kETag               = "ETag";                // “737060cd8c284d8af7ad3082f209582d”
const char* Codec::kExpires            = "Expires";             // Thu, 01 Dec 2010 16:00:00 GMT
const char* Codec::kLastModified       = "Last-Modified";       // Tue, 15 Nov 2010 12:45";//26 GMT
const char* Codec::kLocation           = "Location";            // http://www.zcmhi.com/archives/94.html
const char* Codec::kProxyAuthenticate  = "Proxy-Authenticate";  // Basic
const char* Codec::krefresh            = "Refresh";             // 5; url=http://www.zcmhi.com/archives/94.html
const char* Codec::kRetryAfter         = "Retry-After";         // 120
const char* Codec::kServer             = "Server";              // Apache/1.3.27 (Unix) (Red-Hat/Linux)
const char* Codec::kSetCookie          = "Set-Cookie";          // UserID=JohnDoe; Max-Age=3600; Version=1
const char* Codec::kVary               = "Vary";                // *
const char* Codec::kWWWAuthenticate    = "WWW-Authenticate";    // Basic
//---------------------------------------------------------------------------
//responeses header 
const char* Codec::kResponeseVersion= "ResponeseVersion";
const char* Codec::kResponeseStatus = "ResponeseStatus";
const char* Codec::kCurVer          = "HTTP/1.1";
//---------------------------------------------------------------------------
//status code
//1xx
//2xx
const char* Codec::kOK = "200 OK";
//3xx
//4xx
const char* Codec::kBadRequest = "400 Bad Request";
//5xx
const char* Codec::kNotImplemented = "501 Not Implemented";

//\R\N
const char Codec::kCRLF[] = {'\r', '\n'};
//---------------------------------------------------------------------------
void Codec::DumpReq(const net::TCPConnPtr& tcp_conn)
{
    std::cout << "--------------------------------->" << std::endl;;
    for(auto v : *std::static_pointer_cast<RequestLines>(tcp_conn->any_))
    {
        std::cout << v.first << ":" << v.second << std::endl;
    }
    std::cout << "--------------------------------->" << std::endl;;

    return;
}
//---------------------------------------------------------------------------
void Codec::OnRead(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer, uint64_t rcv_time)
{
    //is new conn
    if(!tcp_conn->any_)
        tcp_conn->any_ = std::make_shared<RequestLines>();

    //read request header
    if((*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kMethod].empty())
    {
        if(false == GetRequestHeader(tcp_conn, buffer))
        {
            callback_req_msg_(tcp_conn, rcv_time);
            return;
        }
    }

    //read request line
    if(0 < buffer.ReadableBytes())
    {
        if(true == GetRequestLines(tcp_conn, buffer))
            callback_req_msg_(tcp_conn, rcv_time);
    }

    return;
}
//---------------------------------------------------------------------------
bool Codec::GetRequestHeader(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kMethod].resize(32);
    const char* crlf = FindCRLF(buffer);
    if(0 == crlf)
        return true;

    const char* begin = buffer.Peek();
    int len = static_cast<int>(crlf - begin);

    ///|method|blank|URL|blank|ver|\r\n

    int idx = 0;

    //method
    for(; idx<len; idx++)
    {
        if(!isspace(begin[idx]))
        {
            (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kMethod].push_back(begin[idx]);
            continue;
        }

        break;
    }

    //URL
    SkipSP(begin, len, idx);
    for(; idx<len; idx++)
    {
        if(!isspace(begin[idx]))
        {
            (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kURL].push_back(begin[idx]);
            continue;
        }

        break;
    }

    //ver
    SkipSP(begin, len, idx);
    (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kReqVersion] = std::string(begin+idx, crlf-(begin+idx));

    buffer.Retrieve(len+sizeof(kCRLF));

    if( (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kMethod].empty() ||
        (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kURL].empty() ||
        (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kReqVersion].empty())
    {
        (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kResponeseStatus] = kBadRequest;
        return false;
    }
    
    return true;;
}
//---------------------------------------------------------------------------
bool Codec::GetRequestLines(const net::TCPConnPtr& tcp_conn, net::Buffer& buffer)
{
    if((*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kMethod].empty())
        return false;

    while(true)
    {
        const char* crlf = FindCRLF(buffer);
        if(0 == crlf)
            return false;

        const char* begin = buffer.Peek();
        int len = static_cast<int>(crlf - begin);

        //\r\n
        if(0 == len)
        {
            buffer.Retrieve(sizeof(kCRLF));
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
            (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[kResponeseStatus] = kBadRequest;
            buffer.Retrieve(len+sizeof(kCRLF));
            return true;
        }

        auto& val = (*std::static_pointer_cast<RequestLines>(tcp_conn->any_))[std::string(begin, idx)];

        //skip ':'
        idx++;

        //val
        SkipSP(begin, len , idx);
        val = std::string(begin+idx, crlf-(begin+idx));
        buffer.Retrieve(len+sizeof(kCRLF));
    }
}
//---------------------------------------------------------------------------
const char* Codec::FindCRLF(const net::Buffer& buffer) const
{
    return static_cast<const char*>(memmem(buffer.Peek(), buffer.ReadableBytes(), kCRLF, sizeof(kCRLF)));
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
