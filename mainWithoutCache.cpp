#include <iostream>
#include <RedisConnection.hpp>
#include <RedisInstrumentCache.hpp>
#include <fstream>
#include <RedisTimer.hpp>

int main()
{
    try {
        std::ifstream infile("/tmp/randomISINList");
        if (infile.is_open())
        {
            std::string line;
            RedisTimer timer;
            RedisConnection redisConnection;

            
            unsigned int numOfRecord = 0;
            
            while( getline(infile, line) )
            {
                RedisInstrumentPtr instPtr(new RedisInstrument);
                redisConnection.runCommand("hget ISINCache " + line, [&instPtr](const std::string& response) {
                        parseInstJson(response, *instPtr);
                    });
                if (!instPtr->exlKey_.empty()) {
                    redisConnection.runCommand("hget EXLCache " + instPtr->exlKey_, [&instPtr](const std::string& response) {
                            parseInstHeaderJson(response, *instPtr);
                        });
                    numOfRecord++;
                    std::cout << *instPtr << std::endl;
                }
            }

            std::cout << "Total seconds for [" << numOfRecord << "] records is " << timer.elapsed() << std::endl;
        }
    }
    catch(std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }
        
}
