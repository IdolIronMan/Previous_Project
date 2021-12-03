#include "chat_server.hpp"
#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
	//输入服务器的端口号
    if (argc != 2)
    {
        cerr << "Usage: " <<  argv[0] << " <port>" << endl;
        return 1;
    }
    //实例化服务器实例并且运行
    ChatServer server(std::atoi(argv[1]));
    server.run();
    return 0;
}
