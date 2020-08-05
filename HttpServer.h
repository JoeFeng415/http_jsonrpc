#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "muduo/net/TcpServer.h"
#include <string>
#include "muduo/net/EventLoop.h"
#include "muduo/base/ThreadPool.h"
#include "muduo/net/InetAddress.h"
#include "muduo/base/Timestamp.h"
#include "muduo/net/Buffer.h"
#include "muduo/base/Atomic.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"

//前置声明
class HttpRequest;
class HttpResponse;
using muduo::net::EventLoop;
using muduo::net::TcpServer;
using muduo::ThreadPool;
using muduo::net::InetAddress;
using muduo::Timestamp;
using muduo::net::Buffer;
using muduo::net::TcpConnectionPtr;

class HttpServer : muduo::noncopyable
{
public:
    typedef std::function<void (const HttpRequest&,
                                HttpResponse*)> HttpCallback;
    HttpServer(EventLoop* loop,
               const InetAddress& listeAddr,
               int numThreads,
               const std::string& name = "HttpServer",
               TcpServer::Option option = TcpServer::kNoReusePort);

    EventLoop* getLoop() const { return m_server.getLoop(); }
    
    /// Not thread safe, callback be registered before calling start().
    void setHttpCallback(const HttpCallback& cb)
    {
        m_httpCallback = cb;
    }

    void start();

    static void processRequestBody(const TcpConnectionPtr& conn,
                      HttpRequest);
    static void test(const HttpRequest& req, HttpResponse* resp);
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);
    TcpServer m_server;
    ThreadPool m_threadPool;
    int m_numThreads;
    Timestamp m_startTime;
    HttpCallback m_httpCallback;  
};
#endif