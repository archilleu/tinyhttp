//---------------------------------------------------------------------------
#include <iostream>
#include "request_message.h"
//---------------------------------------------------------------------------
namespace tinyhttp
{

//---------------------------------------------------------------------------
void RequestMessage::Dump() const
{
    std::cout << "--------------------------------->" << std::endl;;
    std::cout << "method:" << method_ << std::endl;
    std::cout << "url:" << url_ << std::endl;
    std::cout << "version:" << version_ << std::endl;
    for(auto v : req_lines_)
    {
        std::cout << v.first << ":" << v.second << std::endl;
    }
    if(0 != body_)
        std::cout << "body len:" << body_len_ << " body:" << body_ << std::endl;
    std::cout << "--------------------------------->" << std::endl;;

    return;
}
//---------------------------------------------------------------------------
//method
const char* RequestMessage::kOPTIONS = "OPTIONS";
const char* RequestMessage::kGET     = "GET";
const char* RequestMessage::kHEAD    = "HEAD";
const char* RequestMessage::kPOST    = "POST";
const char* RequestMessage::kPUT     = "PUT";
const char* RequestMessage::kDELETE  = "DELETE";
const char* RequestMessage::kTRACE   = "TRACE";
const char* RequestMessage::kCONNECT = "CONNECT";
//---------------------------------------------------------------------------
//status code
//1xx
//2xx
const char* RequestMessage::kOK = "200 OK";
//3xx
//4xx
const char* RequestMessage::kBadRequest = "400 Bad Request";
const char* RequestMessage::kNotFound   = "404 Not Found";
const char* RequestMessage::kForbidden  = "403 Forbidden";
//5xx
const char* RequestMessage::kNotImplemented = "501 Not Implemented";
//---------------------------------------------------------------------------
//common header
const char* RequestMessage::kConnection         = "Connection";              // close, by default in 1.1 keep alive is ON
const char* RequestMessage::kDate               = "Date";                    // Tue, 15 Nov 2010 08:12:31 GMT
const char* RequestMessage::kTrailer            = "Trailer";                 // Max-Forwards
const char* RequestMessage::kTransferEncoding   = "Transfer-Encoding";       // chunked
const char* RequestMessage::kVia                = "Via";                     // 1.0 fred, 1.1 nowhere.com (Apache/1.1)
const char* RequestMessage::kCacheControl       = "Cache-Control";           // no-cache
//---------------------------------------------------------------------------
//request header line
const char* RequestMessage::kAccept             = "Accept";                  // text/plain, text/html
const char* RequestMessage::kAcceptCharset      = "Accept-Charset";          // iso-8859-5
const char* RequestMessage::kAcceptEncoding     = "Accept-Encoding";         // compress, gzip
const char* RequestMessage::kAcceptLanguage     = "Accept-Language";         // en,zh
const char* RequestMessage::kAcceptRanges       = "Accept-Ranges";           // bytes
const char* RequestMessage::kAuthorization      = "Authorization";           // Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
const char* RequestMessage::kCookie             = "Cookie";                  // $Version=1; Skin=new;
const char* RequestMessage::kContentLength      = "Content-Length";          // 348
const char* RequestMessage::kContentType        = "Content-Type";            // application/x-www-form-urlencoded
const char* RequestMessage::kExpect             = "Expect";                  // 100-continue
const char* RequestMessage::kFrom               = "From";                    // user@email.com
const char* RequestMessage::kHost               = "Host";                    // www.zcmhi.com
const char* RequestMessage::kIfMatch            = "If-Match";                // “737060cd8c284d8af7ad3082f209582d”
const char* RequestMessage::kIfModifiedSince    = "If-Modified-Since";       // Sat, 29 Oct 2010 19:43:31 GMT
const char* RequestMessage::kIfNoneMatch        = "If-None-Match";           // “737060cd8c284d8af7ad3082f209582d”
const char* RequestMessage::kIfRange            = "If-Range";                // “737060cd8c284d8af7ad3082f209582d”
const char* RequestMessage::kIfUnmodifiedSince  = "If-Unmodified-Since";     // Sat, 29 Oct 2010 19:43:31 GMT
const char* RequestMessage::kMaxForwards        = "Max-Forwards";            // 10
const char* RequestMessage::kPragma             = "Pragma";                  // no-cache
const char* RequestMessage::kProxyAuthorization = "Proxy-Authorization";     // Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==
const char* RequestMessage::kRange              = "Range";                   // bytes=500-999
const char* RequestMessage::kReferer            = "Referer";                 // http://www.zcmhi.com/archives/71.html
const char* RequestMessage::kTE                 = "TE";                      // trailers,deflate;q=0.5
const char* RequestMessage::kUpgrade            = "Upgrade";                 // HTTP/2.0, SHTTP/1.3, IRC/6.9, RTA/x11
const char* RequestMessage::kUserAgent          = "User-Agent";              // Mozilla/5.0 (Linux; X11)
const char* RequestMessage::kWarning            = "Warn";                    // 199 Miscellaneous warning
//---------------------------------------------------------------------------
//responses header
const char* RequestMessage::kAge                = "Age";                 // 12
const char* RequestMessage::kAllow              = "Allow";               // GET, HEAD
const char* RequestMessage::kContentEncoding    = "Content-Encoding";    // gzip
const char* RequestMessage::kContentLanguage    = "Content-Language";    // en,zh
const char* RequestMessage::kContentLocation    = "Content-Location";    // /index.htm
const char* RequestMessage::kContentMD5         = "Content-MD5";         // Q2hlY2sgSW50ZWdyaXR5IQ==
const char* RequestMessage::kContentRange       = "Content-Range";       // bytes 21010-47021/47022
const char* RequestMessage::kETag               = "ETag";                // “737060cd8c284d8af7ad3082f209582d”
const char* RequestMessage::kExpires            = "Expires";             // Thu, 01 Dec 2010 16:00:00 GMT
const char* RequestMessage::kLastModified       = "Last-Modified";       // Tue, 15 Nov 2010 12:45";//26 GMT
const char* RequestMessage::kLocation           = "Location";            // http://www.zcmhi.com/archives/94.html
const char* RequestMessage::kProxyAuthenticate  = "Proxy-Authenticate";  // Basic
const char* RequestMessage::krefresh            = "Refresh";             // 5; url=http://www.zcmhi.com/archives/94.html
const char* RequestMessage::kRetryAfter         = "Retry-After";         // 120
const char* RequestMessage::kServer             = "Server";              // Apache/1.3.27 (Unix) (Red-Hat/Linux)
const char* RequestMessage::kSetCookie          = "Set-Cookie";          // UserID=JohnDoe; Max-Age=3600; Version=1
const char* RequestMessage::kVary               = "Vary";                // *
const char* RequestMessage::kWWWAuthenticate    = "WWW-Authenticate";    // Basic
//---------------------------------------------------------------------------
//responeses header 
const char* RequestMessage::kResponeseVersion   = "ResponeseVersion";
const char* RequestMessage::kResponeseStatus    = "ResponeseStatus";
const char* RequestMessage::kCurVer             = "HTTP/1.1";
//---------------------------------------------------------------------------

}//namespace tinyhttp
