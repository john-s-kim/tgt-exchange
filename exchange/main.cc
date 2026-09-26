#include <drogon/drogon.h>

int main()
{
    // Port 80 needs root. 8080 is the local listener.
    drogon::app().addListener("0.0.0.0", 8080);
    drogon::app().run();
    return 0;
}
