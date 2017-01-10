#include <RedisInstrumentCache.hpp>

bool RedisInstrumentCache::get(const std::string& key, RedisInstrumentPtr& instrument, bool getFromRedis /* = false*/)
{
    if (getFromRedis || !fetchRecordForKey(key, instrument))
    {
        std::cout << "Fetching from redis ... " << std::endl;
        // key doesn't exist and so lookup redis cache
        RedisInstrumentPtr instrRecord(new RedisInstrument);
        connection_.runCommand("hget " +
                               getCacheFromInstrumentType(instType_) + " " +
                               key, [&instrRecord](const std::string& response)
                               {
                                   parseInstJson(response, *instrRecord);
                               });
        if (!instrRecord->exlKey_.empty())
        {
            connection_.runCommand("hget " +
                                   getCacheFromInstrumentType(EXL) + " " +
                                   instrRecord->exlKey_,
                                   [&instrRecord](const std::string& response)
                                   {
                                       parseInstHeaderJson(response, *instrRecord);
                                   });
        }
                               
        instrList_.push_front(make_pair(key, instrRecord));
        instrMap_.insert(make_pair(key, instrList_.begin()));

        // cleanup if required
        clearOldEntries();
        instrument = instrRecord;
    }

    return true;
}

bool RedisInstrumentCache::fetchRecordForKey(const std::string& key, RedisInstrumentPtr& instrument)
{
    // check if present in local cache, if present return
    // or else query the redis connection
    auto mapRecord = instrMap_.find(key);
    if (mapRecord != instrMap_.end())
    {
        // reset the records
        instrList_.splice(instrList_.begin(), instrList_, mapRecord->second);
        instrument =  mapRecord->second->second;
        return true;
    }

    return false;
}

void RedisInstrumentCache::put(RedisInstrumentPtr& instrument)
{
    std::string key = "";
    switch(instType_)
    {
    case SYMBOL:
        key = instrument->symbol_;
        break;
    case RIC:
        key = instrument->ric_;
        break;
    case ISIN:
        key = instrument->isin_;
        break;
    }

    if (!key.empty())
    {
        RedisInstrumentPtr recordOnCache;
        if (fetchRecordForKey(key, recordOnCache))
        {
            // Record exists on cache. Just update it.
            *recordOnCache = *instrument;
        }
        else
        {
            instrList_.push_front(make_pair(key, instrument));
            instrMap_.insert(make_pair(key, instrList_.begin()));
        }

        // cleanup if required
        clearOldEntries();
    }
}


void RedisInstrumentCache::clearOldEntries()
{
    while(instrList_.size() > maxCacheSize_)
    {
        std::cout << "Clearing Least Recently Used Instruments" << std::endl;
        auto lastItem = instrList_.end();
        lastItem--;
        instrMap_.erase(lastItem->first);
        instrList_.pop_back();
    }
}

bool RedisInstrumentCacheWrapper::getInstrument(RedisInstrumentType instType, const std::string& key, RedisInstrumentPtr& instrument)
{
    bool instrumentFound = false;
    switch(instType)
    {
    case SYMBOL:
        instrumentFound = symbolCache_->get(key, instrument);
        if (instrumentFound)
        {
            ricCache_->put(instrument);
            isinCache_->put(instrument);
        }
        break;
    case RIC:
        instrumentFound = ricCache_->get(key, instrument);
        if (instrumentFound)
        {
            symbolCache_->put(instrument);
            isinCache_->put(instrument);
        }
        break;
    case ISIN:
        instrumentFound = isinCache_->get(key, instrument);
        if (instrumentFound)
        {
            symbolCache_->put(instrument);
            ricCache_->put(instrument);
        }
        break;
    }

    return instrumentFound;
}    
