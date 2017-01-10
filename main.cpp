#include <iostream>
#include <RedisConnection.hpp>
#include <RedisInstrumentCache.hpp>
#include <RedisTimer.hpp>

int main()
{
    try {
        RedisConnection redisConnection;
        RedisInstrumentPtr instPtr(new RedisInstrument);
        RedisTimer timer;
        redisConnection.runCommand("hget ISINCache XS1151682587", [&instPtr](const std::string& response) {
                parseInstJson(response, *instPtr);
            });
        if (!instPtr->exlKey_.empty()) {
            redisConnection.runCommand("hget EXLCache " + instPtr->exlKey_, [&instPtr](const std::string& response) {
                    parseInstHeaderJson(response, *instPtr);
                });
            std::cout << *instPtr << std::endl;
        }
        std::cout << "Total elapsed time in second for one record : " << timer.elapsed() << std::endl;
    }
    catch(std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
}
