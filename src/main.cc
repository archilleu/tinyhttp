//---------------------------------------------------------------------------
#include "http_server.h"
#include "config.h"
#include "method_web.h"
//---------------------------------------------------------------------------
#include<string>
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    tinyhttp::MyConfig.set_path("tinyhttp.cfg");
    tinyhttp::MyConfig.LoadCofig();
    tinyhttp::MyConfig.SaveCofig();

    tinyhttp::MethodWeb method;
    tinyhttp::HTTPServer server(&method);
    server.Start();

    return 0;
}
//---------------------------------------------------------------------------
