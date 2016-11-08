//---------------------------------------------------------------------------
#ifndef TINYHTTP_CONFIG_H_
#define TINYHTTP_CONFIG_H_
//---------------------------------------------------------------------------
#include <string>
#include "../depend/json/include/value.h"
//---------------------------------------------------------------------------
namespace tinyhttp 
{

class HTTPConfig 
{
public:
    HTTPConfig();

    bool LoadCofig();
    bool SaveCofig();

    //cfg file path
    void set_path(const std::string& p) { path_ = p; }
    const std::string& path() const { return path_; }

    const std::string& doc_root() const { return doc_root_; }
    size_t thread_nums() const { return thread_nums_; }
    size_t max_header_size() const { return max_header_size_; }
    size_t max_body_size() const { return max_body_size_; }

private:
    void DefaultHTTPConfig();

    void LoadWebRoot();
    void LoadThreadNums();
    void LoadMaxHeaderSize();
    void LoadMaxBodySize();

private:
    //config item
    std::string doc_root_;
    size_t thread_nums_; 
    size_t max_header_size_;
    size_t max_body_size_;

private:
    std::string path_;
    json::Value config_;

private:
    const static int kSecond = 1;
    const static int kMinute = kSecond * 60;
    const static int kHour = kMinute * 60;
};
//---------------------------------------------------------------------------
extern HTTPConfig MyHTTPConfig;
//---------------------------------------------------------------------------
}//namespace tinyhttp 
//---------------------------------------------------------------------------
#endif //TINYHTTP_CONFIG_H_

