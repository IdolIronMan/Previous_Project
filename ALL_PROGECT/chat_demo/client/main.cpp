#include "chat_client.hpp"
#include <iostream>
using namespace std;
//主函数
//./main server-ip server-port
int main(int argc, char const* argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server-ip> <server-port>" << endl;
        return 1;
    }
    try
    {
		//实例化客户端,并且运行
        ChatClient cc(argv[1],std::atoi(argv[2]));
        cc.run();
    }
    catch(const std::runtime_error& e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }
    return 0;
}
