//---------------------------------------------------------------------------
#include <unistd.h>
#include "config.h"
#include "../depend/json/include/json_reader.h"
#include "../depend/base/include/function.h"
//---------------------------------------------------------------------------
namespace tinyhttp 
{

//---------------------------------------------------------------------------
namespace
{
    const char* kDocRoot    = "doc root";
    const char* kThreadNums = "thread nums";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Config MyConfig;
//---------------------------------------------------------------------------
Config::Config()
{
    DefaultConfig();
}
//---------------------------------------------------------------------------
bool Config::LoadCofig()
{
    //load config file
    config_ = json::Value(json::Value::TYPE::OBJECT);
    json::JsonReader reader;
    reader.ParseFile(path_, &config_);

    //load web root
    LoadWebRoot();

    //load thread num
    LoadThreadNums();

    return true;
}
//---------------------------------------------------------------------------
bool Config::SaveCofig()
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
void Config::DefaultConfig()
{
    //web root
    doc_root_ = ".";

    //thread numbers
    thread_nums_ = 0;

    return;
}
//---------------------------------------------------------------------------
void Config::LoadWebRoot()
{
    json::Value jroot;
    if(false == config_.PairGet(kDocRoot, &jroot))
        config_.PairAdd(kDocRoot, doc_root_);
    else
        doc_root_ = jroot.val();

    return;
}
//---------------------------------------------------------------------------
void Config::LoadThreadNums()
{
    json::Value jnums;
    if(false == config_.PairGet(kThreadNums, &jnums))
        config_.PairAdd(kThreadNums, thread_nums_);
    else
        thread_nums_ = static_cast<int>(jnums.get_uint());

    return;
}
//---------------------------------------------------------------------------

}//namespace tinyhttp
