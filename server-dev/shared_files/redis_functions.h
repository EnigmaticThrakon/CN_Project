#ifndef REDIS_FUNCTIONS_H
#define REDIS_FUNCTIONS_H

#include <mutex>
#include <hiredis/hiredis.h>
#include <string>
#include "game_constants.h"
#include "redis_keys.h"

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

    void reset_database()
    {
        mtx.lock();
        redisCommand(context, "FLUSHALL");
        mtx.unlock();

        set_key(_left_player_response, std::to_string(_initial_lpaddle_y));
        set_key(_right_player_response, std::to_string(_initial_rpaddle_y));
        set_key(_rscore_location_x, std::to_string(_rscore_x));
        set_key(_lscore_location_x, std::to_string(_lscore_x));
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