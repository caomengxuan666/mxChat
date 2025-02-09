#ifndef MSG_HPP
#define MSG_HPP

#include <rpc/msgpack.hpp>
#include <string>

// 基类 Message
struct Message {
    std::string method;
    std::string params;

    MSGPACK_DEFINE(method, params); // 自动生成序列化和反序列化代码
};


// 请求类 Request
struct Request : public Message {
    // 请求类可以扩展额外的字段
    // 如果需要，可以在这里添加更多字段
    MSGPACK_DEFINE(method, params); // 继承 Message 后可直接使用
};

// 响应类 Response
struct Response {
    int status;         // 响应状态
    std::string result; // 响应内容

    MSGPACK_DEFINE(status, result); // 自动生成序列化和反序列化代码
};

#endif // MSG_HPP
