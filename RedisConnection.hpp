#ifndef __REDIS_CONNECTION_HPP__
#define __REDIS_CONNECTION_HPP__

#include <hiredis/hiredis.h>
#include <exception>
#include <RedisInstrument.hpp>

class RedisException : public std::exception
{
    std::string error_;

public:
    RedisException(const std::string & error): error_(error) {}
    virtual ~RedisException() throw() { }
    virtual const char* what() const throw()
    {
        return error_.c_str();
    }
};                                           

class RedisConnection
{
private:
    redisContext * currContext_;

    void connect(const std::string& host, int port);
    
public:
    RedisConnection(const std::string& host = "localhost", int port = 6379):currContext_(nullptr)
    {
        std::cout << "Connecting to Redis server" << std::endl;
        connect(host, port);
    }

    virtual ~RedisConnection()
    {
        if (currContext_)
        {
            std::cout << "Closing connection ... " << std::endl;
            redisFree(currContext_);
            currContext_ = nullptr;
        }
    }

    void runCommand(const std::string& cmd, std::function<void(const std::string& response)> callback);
};


#endif // __REDIS_CONNECTION_HPP__
