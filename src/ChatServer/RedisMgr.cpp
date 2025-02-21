/**
 * @FilePath     : /mxChat/src/ChatServer/RedisMgr.cpp
 * @Description  :  
 * @Author       : caomengxuan666 2507560089@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : caomengxuan666 2507560089@qq.com
 * @LastEditTime : 2025-02-21 14:31:36
 * @Copyright    : PESONAL DEVELOPER CMX., Copyright (c) 2025.
**/
#include <DataBase/RedisMgr.h>
#include <spdlog/spdlog.h>


bool RedisMgr::Connect(const std::string &host, int port) {
    this->_connect = redisConnect(host.c_str(), port);
    if (this->_connect == NULL) {
        std::cout << "Connection error: can't allocate redis context" << std::endl;
        return false;
    }
    if (this->_connect->err) {
        std::cout << "Connection error: " << this->_connect->errstr << std::endl;
        redisFree(this->_connect);
        this->_connect = NULL;
        return false;
    }
    return true;
}

bool RedisMgr::Get(const std::string &key, std::string &value,const std::string &code_prefix) {
    //给key加上code_前缀
    const std::string queryKey=code_prefix+key;
    if (!Connect("127.0.0.1", 6379)) {// 默认连接本地 Redis
        std::cerr << "Failed to connect to Redis!" << std::endl;
    }
    else{
        std::cout<<"successful connect to Redis"<<std::endl;
    }
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "GET %s", queryKey.c_str());
    if (this->_reply == NULL) {
        std::cout << "[ GET " << queryKey << " ] failed" << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    if (this->_reply->type != REDIS_REPLY_STRING) {
        std::cout << "[ GET " << queryKey << " ] failed" << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    value = this->_reply->str;
    freeReplyObject(this->_reply);

    spdlog::info("[ GET queryKey {} ] success, value : {}",queryKey,value);
    return true;
}

bool RedisMgr::Set(const std::string &key, const std::string &value) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    //执行redis命令行

    this->_reply = (redisReply *) redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());

    //如果返回NULL则说明执行失败
    if (NULL == this->_reply) {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    //如果执行失败则释放连接
    if (!(this->_reply->type == REDIS_REPLY_STATUS && (strcmp(this->_reply->str, "OK") == 0 || strcmp(this->_reply->str, "ok") == 0))) {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    freeReplyObject(this->_reply);
    std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisMgr::Auth(const std::string &password) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "AUTH %s", password.c_str());
    if (this->_reply->type == REDIS_REPLY_ERROR) {
        std::cout << "认证失败" << std::endl;
        //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
        freeReplyObject(this->_reply);
        return false;
    } else {
        //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
        freeReplyObject(this->_reply);
        std::cout << "认证成功" << std::endl;
        return true;
    }
}

bool RedisMgr::LPush(const std::string &key, const std::string &value) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "LPUSH %s %s", key.c_str(), value.c_str());
    if (NULL == this->_reply) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    if (this->_reply->type != REDIS_REPLY_INTEGER || this->_reply->integer <= 0) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

bool RedisMgr::LPop(const std::string &key, std::string &value) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "LPOP %s ", key.c_str());
    if (_reply == nullptr || _reply->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }
    value = _reply->str;
    std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

bool RedisMgr::RPush(const std::string &key, const std::string &value) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "RPUSH %s %s", key.c_str(), value.c_str());
    if (NULL == this->_reply) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    if (this->_reply->type != REDIS_REPLY_INTEGER || this->_reply->integer <= 0) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }

    std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

bool RedisMgr::HSet(const std::string &key, const std::string &hkey, const std::string &value) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (_reply == nullptr || _reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}


bool RedisMgr::HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    this->_reply = (redisReply *) redisCommandArgv(this->_connect, 4, argv, argvlen);
    if (_reply == nullptr || _reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

std::string RedisMgr::HGet(const std::string &key, const std::string &hkey) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return "";
    }
    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    this->_reply = (redisReply *) redisCommandArgv(this->_connect, 3, argv, argvlen);
    if (this->_reply == nullptr || this->_reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(this->_reply);
        std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
        return "";
    }

    std::string value = this->_reply->str;
    freeReplyObject(this->_reply);
    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
    return value;
}

bool RedisMgr::Del(const std::string &key) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "DEL %s", key.c_str());
    if (this->_reply == nullptr || this->_reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

bool RedisMgr::ExistsKey(const std::string &key) {
    if (this->_connect == NULL) {
        std::cout << "Redis connection is not established" << std::endl;
        return false;
    }
    this->_reply = (redisReply *) redisCommand(this->_connect, "exists %s", key.c_str());
    if (this->_reply == nullptr || this->_reply->type != REDIS_REPLY_INTEGER || this->_reply->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ] ! " << std::endl;
        freeReplyObject(this->_reply);
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    freeReplyObject(this->_reply);
    return true;
}

void RedisMgr::Close() {
    if (this->_connect != NULL) {
        redisFree(_connect);
        this->_connect = NULL;
    }
}