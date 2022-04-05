#ifndef REDIS_FUNCTIONS_H
#define REDIS_FUNCTIONS_H

#include <mutex>
#include <hiredis/hiredis.h>
#include <string>

class redis_handler
{
private:
    std::mutex mtx;
    redisContext *context;
    redisReply *reply;

    const char *hostname = "127.0.0.1";
    int port = 6379;
    struct timeval timeout = { 1, 500000 };

public:
    redis_handler()
    {
        context = NULL;
        reply = NULL;

        context = redisConnectWithTimeout(hostname, port, timeout);
    }

    ~redis_handler()
    {
        if(reply != NULL)
        {
            freeReplyObject(reply);
        }

        redisFree(context);
    }

    void set_key(std::string key, std::string value)
    {
        mtx.lock();
        redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
        mtx.unlock();
    }

    std::string get_key(std::string key)
    {
        mtx.lock();
        reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());

        std::string returnString = "";
        if(reply->type == REDIS_REPLY_STRING) {
            returnString = std::string(reply->str);
        }
        
        freeReplyObject(reply);
        mtx.unlock();
        
        return returnString;
    }
};

#endif