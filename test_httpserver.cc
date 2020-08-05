#include "muduo/base/Atomic.h"
#include "muduo/base/Logging.h"
#include "muduo/base/Thread.h"
#include "muduo/base/ThreadPool.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpServer.h"

#include "HttpServer.h"

#include <utility>

#include <stdio.h>
#include <unistd.h>
using namespace muduo;
using namespace muduo::net;

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  EventLoop loop;
  InetAddress listenAddr(9982);
  HttpServer server(&loop, listenAddr, 2);

  server.start();

  loop.loop();
}