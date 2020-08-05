#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "muduo/base/copyable.h"
#include "muduo/base/Timestamp.h"
#include "muduo/base/Types.h"

#include <map>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <iostream>


using std::string;
using muduo::Timestamp;

class HttpRequest : public muduo::copyable
{
 public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
        : m_method(kInvalid),
        m_version(kUnknown),
        m_contentLength(0)
    {
    }

    void setVersion(Version v)
    {
        m_version = v;
    }

    Version getVersion() const
    { return m_version; }

    bool setMethod(const char* start, const char* end)
    {
        assert(m_method == kInvalid);
        string m(start, end);
        if (m == "GET")
        {
            m_method = kGet;
        }
        else if (m == "POST")
        {
            m_method = kPost;
        }
        else if (m == "HEAD")
        {
            m_method = kHead;
        }
        else if (m == "PUT")
        {
            m_method = kPut;
        }
        else if (m == "DELETE")
        {
            m_method = kDelete;
        }
        else
        {
            m_method = kInvalid;
        }
        return m_method != kInvalid;
    }

    Method method() const
    { return m_method; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(m_method)
        {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        m_path.assign(start, end);
    }

    const std::string& path() const
    { return m_path; }

    void setQuery(const char* start, const char* end)
    {
        m_query.assign(start, end);
    }

    const string& query() const
    { return m_query; }

    void setReceiveTime(Timestamp t)
    { m_receiveTime = t; }

    Timestamp receiveTime() const
    { return m_receiveTime; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }
        m_headers[field] = value;
        // printf(field.c_str());
        // printf(" ");
        // printf(value.c_str());
        // printf("\n");
        if (field == "Content-Length")
        {
            m_contentLength = std::stol(value);
        }
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = m_headers.find(field);
        if (it != m_headers.end())
        {
        result = it->second;
        }
        return result;
    }

    //ps返回content-length
    long getContentLength() const
    {
        return m_contentLength;
    }

    const std::map<string, string>& headers() const
    { return m_headers; }

    void swap(HttpRequest& that)
    {
        std::swap(m_method, that.m_method);
        std::swap(m_version, that.m_version);
        m_path.swap(that.m_path);
        m_query.swap(that.m_query);
        m_receiveTime.swap(that.m_receiveTime);
        m_headers.swap(that.m_headers);
    }

    //ps
    void setBody(const char* begin, const char* end)
    {
        m_body = string(begin, end);  //这里又要拷贝
        std::cout << m_body << std::endl;
    }

    const string& getBody() const
    {
        return m_body;
    }

private:
    Method m_method;
    Version m_version;
    string m_path;
    string m_query;
    string m_body;
    Timestamp m_receiveTime;
    std::map<string, string> m_headers;
    long m_contentLength;  //
};

#endif  //
