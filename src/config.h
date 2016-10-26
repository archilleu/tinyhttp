//---------------------------------------------------------------------------
#ifndef TINYHTTP_CONFIG_H_
#define TINYHTTP_CONFIG_H_
//---------------------------------------------------------------------------
#include <string>
#include <map>
#include "../depend/json/include/value.h"
//---------------------------------------------------------------------------
namespace tinyhttp 
{

class Config 
{
public:
    Config();

    bool LoadCofig();
    bool SaveCofig();

    const std::string& web_root() const { return web_root_; }
    int thread_nums() const { return thread_nums_; }

    void set_path(const std::string& p) { path_ = p; }
    const std::string& path() const { return path_; }

private:
    void DefaultConfig();

    void LoadWebRoot();
    void LoadThreadNums();

private:
    //config item
    std::string web_root_;
    int thread_nums_; 

private:
    std::string path_;
    json::Value config_;

private:
    const static int kSecond = 1;
    const static int kMinute = kSecond * 60;
    const static int kHour = kMinute * 60;
};
//---------------------------------------------------------------------------
extern Config MyConfig;
//---------------------------------------------------------------------------
}//namespace tinyhttp 
//---------------------------------------------------------------------------
#endif //TINYHTTP_CONFIG_H_

