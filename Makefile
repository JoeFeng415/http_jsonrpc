# 原来路径如下
# MUDUO_DIRECTORY ?= $(HOME)/build/debug-install
# 根据实际修改 muduo库安装之后在用户主目录下有个build文件夹
MUDUO_DIRECTORY ?= $(HOME)/build/release-install-cpp11
#MUDUO_DIRECTORY ?= $(HOME)/build/install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
# SRC = ../src
SRC = ./

CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(MUDUO_INCLUDE) \
	   -std=c++11         #使用c++11

LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_net -lmuduo_base -lpthread -lrt

all: server
clean:
	rm -f server core

# echo: $(SRC)/echo.cc
# server: $(SRC)/server_threadpool.cc sudoku.cc
# server: $(SRC)/test_httpserver.cc ./HttpServer.cc ./HttpResponse.cc ./HttpContext.cc ./HttpRequest.h JsonrpcProcesser.cc
server: $(SRC)/origin_test.cc ./HttpServer.cc ./HttpResponse.cc ./HttpContext.cc ./HttpRequest.h JsonrpcProcesser.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
