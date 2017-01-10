#include <RedisConnection.hpp>
#include <boost/lexical_cast.hpp>

void RedisConnection::connect(const std::string & host, int port)
{
    if (!currContext_)
    {
        currContext_ = redisConnect(host.c_str(), port);

        if (!currContext_ || (currContext_ && currContext_->err))
        {
            if (currContext_)
            {
                RedisException rerr(currContext_->errstr);
                redisFree(currContext_);
                currContext_ = nullptr;

                throw rerr;
            }

            throw RedisException("Failed connecting to the Redis Server");
        }
    }
}

// will only handle string and array reply from redis
void RedisConnection::runCommand(const std::string& cmd, std::function<void(const std::string& response)> callback)
{
    redisReply* reply = nullptr;
    if (currContext_)
    {
        reply = static_cast<redisReply*>(redisCommand(currContext_, cmd.c_str()));

        switch(reply->type)
        {
        case REDIS_REPLY_ERROR:
            throw RedisException(reply->str);
            break;
        case REDIS_REPLY_ARRAY:
            for (unsigned int j = 0; j < reply->elements; j++) {
                callback(reply->element[j]->str);
            }
            break;
        case REDIS_REPLY_STRING:
            callback(reply->str);
            break;
        case REDIS_REPLY_NIL:
            throw RedisException("No data available for command [" + cmd + "]");
            break;
        default:
            throw RedisException("Invalid reply format [" + boost::lexical_cast<std::string>(reply->type) + "] received");
            break;
        }
    }

    if (reply)
        freeReplyObject(reply);
}
