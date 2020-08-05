
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "muduo/base/copyable.h"
#include "muduo/base/Types.h"
#include "muduo/net/Buffer.h"

#include <map>
#include <string>

using std::string;
using muduo::net::Buffer;
class HttpResponse : public muduo::copyable
{
public:
    enum HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(bool close)
        : m_statusCode(kUnknown),
        m_closeConnection(close)
    {
    }

    void setStatusCode(HttpStatusCode code)
    { m_statusCode = code; }

    void setStatusMessage(const string& message)
    { m_statusMessage = message; }

    void setCloseConnection(bool on)
    { m_closeConnection = on; }

    bool closeConnection() const
    { return m_closeConnection; }

    void setContentType(const string& contentType)
    { addHeader("Content-Type", contentType); }

    // FIXME: replace string with StringPiece
    void addHeader(const string& key, const string& value)
    { m_headers[key] = value; }

    void setBody(const string& body)
    { m_body = body; }

    void appendToBuffer(Buffer* output) const;

 private:
    std::map<string, string> m_headers;
    HttpStatusCode m_statusCode;
    // FIXME: add http version
    string m_statusMessage;
    bool m_closeConnection;
    string m_body;
};

#endif  // 
