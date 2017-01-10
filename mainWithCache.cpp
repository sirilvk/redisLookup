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
            RedisInstrumentCacheWrapper redisInstrumentCache(redisConnection);
            unsigned int numOfRecord = 0;
            
            while( getline(infile, line) )
            {
                RedisInstrumentPtr inst;
                redisInstrumentCache.getInstrument(ISIN, line, inst);
        
                if (inst)
                {
                    numOfRecord++;
                    std::cout << *inst << std::endl;
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
