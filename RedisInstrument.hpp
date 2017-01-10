#ifndef __REDIS_INSTRUMENT_HPP__
#define __REDIS_INSTRUMENT_HPP__

#include <string>
#include <iostream>
#include <memory>

struct RedisInstrumentHeader
{
    std::string issueType_;
    std::string exchange_;
    std::string service_;
    std::string dds_source_;
    std::string dds_category_;
};

struct RedisInstrument
{
    RedisInstrumentHeader header_;
    std::string symbol_;
    std::string ric_;
    std::string isin_;
    std::string domain_;
    std::string display_name_;
    unsigned long prod_perm_;
    unsigned long context_id_;
    std::string exlKey_;
    std::string jsonMsg_;
    std::string jsonHeaderMsg_;
};

enum RedisInstrumentType
{
    INVALID,
    SYMBOL,
    RIC,
    ISIN,
    EXL
};

typedef std::shared_ptr<RedisInstrument> RedisInstrumentPtr;

const std::string getInstrumentType(RedisInstrumentType type);
const std::string getCacheFromInstrumentType(RedisInstrumentType type);
std::ostream& operator<<(std::ostream& os, const RedisInstrumentHeader& instr);
std::ostream& operator<<(std::ostream& os, const RedisInstrument& instr);

void parseInstHeaderJson(const std::string& headerJson, RedisInstrument& inst);
void parseInstJson(const std::string& instJson, RedisInstrument& inst);

#endif // __REDIS_INSTRUMENT_HPP__
