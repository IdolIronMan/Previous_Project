#ifndef TCP_ACCEPTOR_HPP_
#define TCP_ACCEPTOR_HPP_

//TCP接收类
class TcpAcceptor 
{
public:
	//构造函数
    TcpAcceptor(short port);
    //析构函数
    ~TcpAcceptor();
    //接收
    int accept();
private:
    int sockfd_;
};


#endif