//---------------------------------------------------------------------------
#include "http_server.h"
#include "http_config.h"
#include "method_web.h"
//---------------------------------------------------------------------------
#include<string>
int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    tinyhttp::MyHTTPConfig.set_path("tinyhttp.cfg");
    tinyhttp::MyHTTPConfig.LoadCofig();
    tinyhttp::MyHTTPConfig.SaveCofig();

    tinyhttp::MethodWeb method;
    tinyhttp::HTTPServer server(&method);
    server.Start();

    return 0;
}
//---------------------------------------------------------------------------
