#include "HttpServer.h"

#include "muduo/base/Logging.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <stdio.h>
#include <iostream>
#include "JsonrpcProcesser.h"  

using namespace muduo;
using namespace muduo::net;

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       int numThreads,
                       const string& name,
                       TcpServer::Option option)
    : m_server(loop, listenAddr, name),
      m_numThreads(numThreads),
      m_startTime(Timestamp::now()),
        m_httpCallback(defaultHttpCallback)
{
    m_server.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, _1));
    m_server.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, _1, _2, _3));
    printf("HttpServer ctr!!!\n");
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << m_server.name()
        << "] starts listenning on " << m_server.ipPort();
    LOG_WARN << "starting " << m_numThreads << " threads.";
    m_threadPool.start(m_numThreads);
    m_server.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());  //这个HttpContext后面还会用到
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf,
                           Timestamp receiveTime)
{
    printf("HttpServer::onMessage\n");
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext()); //conn->getMutableContext()返回的是引用

    if (!context->parseRequest(buf, receiveTime))
    {
        printf("HttpServer::onMessage parseRequest error!\n");
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll())
    {
        printf("HttpServer::onMessage context->gotAll!\n");
        //放到线程池里处理
        m_threadPool.run(std::bind(&processRequestBody,
                                    conn, context->request()));
        //onRequest(conn, context->request());
        context->reset();   //会把request清空
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    m_httpCallback(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}

//1.问题:为什么一定要用static function,下面是不用static function后的报错
//ISO C++ forbids taking the address 
//of an unqualified or parenthesized non-static member function to form a pointer to member function
//2.注意点:类的静态函数,这里定义不要再在前面加static了
void HttpServer::processRequestBody(const TcpConnectionPtr& conn,
                     HttpRequest req)  //这里暂时只能想到拷贝了
                     //不拷贝如果后面还有数据过来,怎么办?待解决
{
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
        (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    test(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}

void HttpServer::test(const HttpRequest& req, HttpResponse* resp)  //回复的业务逻辑在此,先做个测试
{
    std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
    bool benchmark = false;
    if (!benchmark)
    {
        const std::map<string, string>& headers = req.headers();
        for (const auto& header : headers)
        {
        std::cout << header.first << ": " << header.second << std::endl;
        }
    }

    JsonrpcProcesser jrpcProcesser;  
    jrpcProcesser.parseJson(req.getBody());

    if (req.path() == "/")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "Muduo");
        string now = Timestamp::now().toFormattedString();
        // resp->setBody("<html><head><title>This is title</title></head>"
        //     "<body><h1>Hello</h1>Now is " + now +
        //     "</body></html>");
        resp->setBody("<html><head><title>This is title</title></head>"
            "<body><h1>Hello</h1>Welcome to my house! "
            "</body></html>");
    }
    else if (req.path() == "/favicon.ico")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("image/png");
        resp->setBody(string("hei how are you"));
    }
    else if (req.path() == "/hello")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "Muduo");
        resp->setBody("hello, how are you!\n");
    }
    else
    {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}
