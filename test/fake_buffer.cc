//---------------------------------------------------------------------------
#include "../depend/net/include/buffer.h"
#include <cstring>
//---------------------------------------------------------------------------
namespace net
{

int Buffer::ReadFd(int len, int* dat)
{
    char* begin = Begin() + write_index_;
    char* str = reinterpret_cast<char*>(dat);
    int writable = static_cast<int>(WritableBytes());
    if(len < writable)
    {
        memcpy(begin, str, len);
        write_index_ += len;
    }
    else
    {
        write_index_ = buffer_.size();
        int remain = len - writable;
        Append(begin+remain, remain);
    }

    return len;
}

}
//---------------------------------------------------------------------------
