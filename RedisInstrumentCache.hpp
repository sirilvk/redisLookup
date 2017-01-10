#ifndef __REDIS_INSTRUMENT_CACHE_HPP__
#define __REDIS_INSTRUMENT_CACHE_HPP__

#include <RedisInstrument.hpp>
#include <list>
#include <unordered_map>
#include <RedisConnection.hpp>
#include <memory>

// Maintaining an LRU cache. We will be looking up this cache
// before making a redis query.

class RedisInstrumentCache
{
    typedef std::list<std::pair<std::string, RedisInstrumentPtr> > CacheList;
    typedef CacheList::iterator cache_iterator_t;
    typedef std::unordered_map<std::string, cache_iterator_t> CacheMap;
    
    CacheList instrList_;
    CacheMap instrMap_;
    RedisInstrumentType instType_;
    size_t maxCacheSize_;
    RedisConnection& connection_;

    void clearOldEntries();

    bool fetchRecordForKey(const std::string& key, RedisInstrumentPtr& instrument);
    
public:
    RedisInstrumentCache(RedisConnection& connection, RedisInstrumentType instType,
                         size_t cacheSize = 32)
        : connection_(connection)
        , instType_(instType)
        , maxCacheSize_(cacheSize) {}

    bool get(const std::string& key, RedisInstrumentPtr& instrument, bool getFromRedis = false);

    void put(RedisInstrumentPtr& instrument);
    
    bool existsInCache(const std::string& key)
    {
        return (instrMap_.find(key) != instrMap_.end());
    }
};

class RedisInstrumentCacheWrapper
{
    std::unique_ptr<RedisInstrumentCache> isinCache_;
    std::unique_ptr<RedisInstrumentCache> ricCache_;
    std::unique_ptr<RedisInstrumentCache> symbolCache_;

public:

    RedisInstrumentCacheWrapper(RedisConnection& connection, size_t cacheSize = 32)
    {
        isinCache_.reset(new RedisInstrumentCache(connection, ISIN, cacheSize));
        ricCache_.reset(new RedisInstrumentCache(connection, RIC, cacheSize));
        symbolCache_.reset(new RedisInstrumentCache(connection, SYMBOL, cacheSize));
    }

    
    bool getInstrument(RedisInstrumentType instType, const std::string& key, RedisInstrumentPtr& instrument);
};



#endif // __REDIS_INSTRUMENT_CACHE_HPP__
