#ifndef JSONRPCPROCESSER_H
#define JSONRPCPROCESSER_H
#include <string>
#include <vector>

// class HttpRequest;
using std::string;
class JsonrpcProcesser
{
public:
    JsonrpcProcesser();
    ~JsonrpcProcesser();

    void parseJson(const string& request_body);

    void process();

private:
    string m_jsonrpcVersion;
    string m_method;
    std::vector<string> m_params;
    int m_id;
    
};
#endif