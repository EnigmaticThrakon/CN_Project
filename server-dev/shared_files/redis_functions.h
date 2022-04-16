#ifndef REDIS_FUNCTIONS_H
#define REDIS_FUNCTIONS_H

#include <mutex>
#include <hiredis/hiredis.h>
#include <string>
#include "game_constants.h"
#include "redis_keys.h"

/*
 * Class to hold basic functions of the Redis Database and context
 *
 * Allows for the settings and retreiving of data from the Redis Database
 *      while also making sure that each redis context is accessing the database
 *      through mutex locks preventing threading errors
 */

class redis_handler
{
private:
    //Private variables that are utilized in multiple functions
    std::mutex mtx;
    redisContext *context;
    redisReply *reply;

    //Private variables that are constants used to initialize the database
    //      context when constructing the class variable
    const char *hostname = "127.0.0.1";
    int port = 6379;
    struct timeval timeout = { 1, 500000 };

public:
    redis_handler()
    {
        //Initially setting the pointers to null
        context = NULL;
        reply = NULL;

        //Establishing the context pointer to the database based on the 
        //      global constants
        context = redisConnectWithTimeout(hostname, port, timeout);
    }

    ~redis_handler()
    {
        //If the reply needs to be freed, then free the reply pointer
        if(reply != NULL)
        {
            freeReplyObject(reply);
        }

        //Free the context pointer
        redisFree(context);
    }

    void reset_database()
    {
        //Set the mutex lock to make a command call that removes all data from the 
        //      Redis database preventing errors from pulling data from previously set
        //      set keys
        mtx.lock();
        redisCommand(context, "FLUSHALL");
        mtx.unlock();

        //Go through and set all the necessary keys to beginning values that will be needed
        //      when the keys are initially checked
        set_key(_left_player_response, std::to_string(_initial_paddle_y));
        set_key(_right_player_response, std::to_string(_initial_paddle_y));

        set_key(_left_player_connected, "0");
        set_key(_right_player_connected, "0");

        set_key(_game_setup, "0");
        set_key(_game_started, "0");

        set_key(_left_player_update, "0");
        set_key(_right_player_update, "0");

        set_key(_left_started_received, "0");
        set_key(_right_started_received, "0");

        set_key(_status_update_sent, "0");
        set_key(_status_update_ready, "0");
    }

    void set_key(std::string key, std::string value)
    {
        //Enter a mutex lock to set the value of a key
        //      Sets the key input with the value input
        mtx.lock();
        redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
        mtx.unlock();
    }

    std::string get_key(std::string key)
    {
        //Enter a mutex lock to get the data stored in the key
        //      that's input to the function
        mtx.lock();
        reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());

        //Converting the data to a standard string if there is data that
        //      can be converted to a string
        std::string returnString = "";
        if(reply->type == REDIS_REPLY_STRING) {
            returnString = std::string(reply->str);
        }
        
        //Freeing the reply pointer and the mutex lock before returning the
        //      data stored at the key
        freeReplyObject(reply);
        mtx.unlock();
        
        return returnString;
    }
};

#endif