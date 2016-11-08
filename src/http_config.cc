//---------------------------------------------------------------------------
#include <unistd.h>
#include "http_config.h"
#include "../depend/json/include/json_reader.h"
#include "../depend/base/include/function.h"
//---------------------------------------------------------------------------
namespace tinyhttp 
{

//---------------------------------------------------------------------------
namespace
{
    const char* kDocRoot        = "doc root";
    const char* kThreadNums     = "thread nums";
    const char* kMaxHeaderSize  = "max header size";
    const char* kMaxBodySize    = "max body size";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HTTPConfig MyHTTPConfig;
//---------------------------------------------------------------------------
HTTPConfig::HTTPConfig()
{
    DefaultHTTPConfig();
}
//---------------------------------------------------------------------------
bool HTTPConfig::LoadCofig()
{
    //load config file
    config_ = json::Value(json::Value::TYPE::OBJECT);
    json::JsonReader reader;
    reader.ParseFile(path_, &config_);

    LoadWebRoot();
    LoadThreadNums();
    LoadMaxHeaderSize();
    LoadMaxBodySize();

    return true;
}
//---------------------------------------------------------------------------
bool HTTPConfig::SaveCofig()
{
    std::string cfg = config_.ToString(true);
    std::string temp_name = path_ + "-temp";

    if(false == base::SaveFile(temp_name, cfg.data(), cfg.length()))
        return false;

    //rename
    if(-1 == rename(temp_name.c_str(), path_.c_str()))
    {
        unlink(temp_name.c_str());
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void HTTPConfig::DefaultHTTPConfig()
{
    doc_root_ = ".";
    thread_nums_ = 0;
    max_header_size_ = 1024*2;
    max_body_size_ = 1024*8;

    return;
}
//---------------------------------------------------------------------------
void HTTPConfig::LoadWebRoot()
{
    json::Value jroot;
    if(false == config_.PairGet(kDocRoot, &jroot))
        config_.PairAdd(kDocRoot, doc_root_);
    else
        doc_root_ = jroot.val();

    return;
}
//---------------------------------------------------------------------------
void HTTPConfig::LoadThreadNums()
{
    json::Value jnums;
    if(false == config_.PairGet(kThreadNums, &jnums))
        config_.PairAdd(kThreadNums, thread_nums_);
    else
        thread_nums_ = static_cast<int>(jnums.get_uint());

    return;
}
//---------------------------------------------------------------------------
void HTTPConfig::LoadMaxHeaderSize()
{
    json::Value jsize;
    if(false == config_.PairGet(kMaxHeaderSize, &jsize))
        config_.PairAdd(kMaxHeaderSize, max_header_size_);
    else
        max_header_size_ = static_cast<int>(jsize.get_uint());

    return;
}
//---------------------------------------------------------------------------
void HTTPConfig::LoadMaxBodySize()
{
    json::Value jsize;
    if(false == config_.PairGet(kMaxBodySize, &jsize))
        config_.PairAdd(kMaxBodySize, max_body_size_);
    else
        max_body_size_ = static_cast<int>(jsize.get_uint());

    return;
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
