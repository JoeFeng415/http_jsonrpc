#include "JsonrpcProcesser.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <iostream>
using namespace std;
using namespace rapidjson;

JsonrpcProcesser::JsonrpcProcesser()  
{
}

JsonrpcProcesser::~JsonrpcProcesser()
{
}

void JsonrpcProcesser::parseJson(const string& request_body)
{
    cout << "JsonrpcProcesser::parseJson" << endl;
    Document doc;
    if (doc.Parse(request_body.c_str()).HasParseError())
    {
        cout << "Json rpc parse error" << endl;
        return;
    }

    if (doc.HasMember("method"))
    {
        Value result(kObjectType);
        result = doc["method"];
        
        cout << result.GetString() << endl;
    }
}

void JsonrpcProcesser::process()
{

}