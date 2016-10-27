//---------------------------------------------------------------------------
#ifndef TINYHTTP_REQUEST_MESSAGE_H_
#define TINYHTTP_REQUEST_MESSAGE_H_
//---------------------------------------------------------------------------
#include <map>
//---------------------------------------------------------------------------
/*
 * attention
 *  everything in RequestMessage depend on Buffer's data
 */
//---------------------------------------------------------------------------
namespace tinyhttp
{

class RequestMessage
{
public:
    RequestMessage()
    :   method_(0),
        version_(0),
        url_(0),
        rq_size_(0),
        body_(0),
        body_len_(0)
    {}
    RequestMessage(RequestMessage&& o)
    :   method_(o.method_),
        version_(o.version_),
        url_(o.url_),
        req_lines_(std::move(o.req_lines_)),
        rq_size_(o.rq_size_),
        body_(o.body_),
        body_len_(o.body_len_)
    {
        o.method_   = 0;
        o.version_  = 0;
        o.url_      = 0;
        o.rq_size_  = 0;
        o.body_     = 0;
        o.body_len_ = 0;
    }
    RequestMessage& operator=(RequestMessage&& o)
    {
        method_     = o.method_;
        version_    = o.version_;
        url_        = o.url_;
        req_lines_  = std::move(o.req_lines_);
        rq_size_    = o.rq_size_;
        body_       = o.body_;
        body_len_   = o.body_len_;

        o.method_   = 0;
        o.version_  = 0;
        o.url_      = 0;
        o.rq_size_  = 0;
        o.body_     = 0;
        o.body_len_ = 0;

        return *this;
    }
    void reset()
    {
        method_   = 0;
        version_  = 0;
        url_      = 0;
        req_lines_.clear();
        rq_size_  = 0;
        body_     = 0;
        body_len_ = 0;
    }

    //for debug
    void Dump() const;

public:
    char* method_;
    char* version_;
    char* url_;

    std::map<char*, char*> req_lines_;

    int rq_size_;   //current parse request message size

    char* body_;
    int body_len_;

public:
    //method
    static const char* kOPTIONS;
    static const char* kGET;
    static const char* kHEAD;
    static const char* kPOST;
    static const char* kPUT;
    static const char* kDELETE;
    static const char* kTRACE;
    static const char* kCONNECT;

    //status code
    //1xx
    //2xx
    static const char* kOK;
    //3xx
    //4xx
    static const char* kBadRequest;
    static const char* kNotFound;
    static const char* kForbidden;
    //5xx
    static const char* kNotImplemented;

    //common header
    static const char* kConnection;
    static const char* kDate;
    static const char* kTrailer;
    static const char* kTransferEncoding;
    static const char* kVia;

    //request line header
    static const char* kAccept;
    static const char* kAcceptCharset;
    static const char* kAcceptEncoding;
    static const char* kAcceptLanguage;
    static const char* kAcceptRanges;
    static const char* kAuthorization;
    static const char* kCacheControl;
    static const char* kCookie;
    static const char* kContentLength;
    static const char* kContentType;
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
    static const char* kVary;
    static const char* kWWWAuthenticate;
};

}//namespace tinyhttp
//---------------------------------------------------------------------------
#endif //TINYHTTP_REQUEST_MESSAGE_H_

