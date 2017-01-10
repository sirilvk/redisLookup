#include <RedisInstrument.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

const std::string getInstrumentType(RedisInstrumentType type)
{
    switch(type)
    {
    case SYMBOL:
        return "SYMBOL";
    case RIC:
        return "RIC";
    case ISIN:
        return "ISIN";
    case EXL:
        return "EXL";
    }

    return "";
}

const std::string getCacheFromInstrumentType(RedisInstrumentType type)
{
    switch(type)
    {
    case SYMBOL:
        return "SYMBOLCache";
    case RIC:
        return "RICCache";
    case ISIN:
        return "ISINCache";
    case EXL:
        return "EXLCache";
    }

    return "";
}

std::ostream& operator<<(std::ostream& os, const RedisInstrumentHeader& instr)
{
    os << "IssueType : " << instr.issueType_ << std::endl;
    os << "Exchange : " << instr.exchange_ << std::endl;
    os << "Service : " << instr.service_ << std::endl;
    os << "DDS_Source : " << instr.dds_source_ << std::endl;
    os << "DDS_Category : " << instr.dds_category_ << std::endl;
}

std::ostream& operator<<(std::ostream& os, const RedisInstrument& instr)
{
    os << "Symbol : " << instr.symbol_ << std::endl;
    os << "Ric : " << instr.ric_ << std::endl;
    os << "Domain : " << instr.domain_ << std::endl;
    os << "Display Name : " << instr.display_name_ << std::endl;
    os << "Prod Perm : " << instr.prod_perm_ << std::endl;
    os << "Context ID : " << instr.context_id_ << std::endl;
    os << "EXL Key : " << instr.exlKey_ << std::endl;
    os << "EXL Header : " << std::endl;
    os << instr.header_ << std::endl;
    os << "JSON Message : " << instr.jsonMsg_ << std::endl;
    os << "JSON Header Message : " << instr.jsonHeaderMsg_ << std::endl;
}

void parseInstHeaderJson(const std::string& headerJson, RedisInstrument& inst)
{
    inst.jsonHeaderMsg_ = headerJson;
    RedisInstrumentHeader& header = inst.header_;
    pt::ptree jsonPT;
    std::stringstream ss(headerJson.c_str());
    pt::read_json(ss, jsonPT);
    for (pt::ptree::const_iterator it = jsonPT.begin(); it != jsonPT.end(); ++it) {
        if (it->first == "it:ISSUTYPE")
            header.issueType_ = it->second.get_value<std::string>();
        else if (it->first == "it:EXCHANGE")
            header.exchange_ = it->second.get_value<std::string>();
        else if (it->first == "it:SERVICE")
            header.service_ = it->second.get_value<std::string>();
        else if (it->first == "it:DDS_SOURCE")
            header.dds_source_ = it->second.get_value<std::string>();
        else if (it->first == "it:DDS_CATEGORY")
            header.dds_category_ = it->second.get_value<std::string>();
    }
}

void parseInstJson(const std::string& instJson, RedisInstrument& inst)
{
    pt::ptree jsonPT;
    std::stringstream ss(instJson.c_str());
    pt::read_json(ss, jsonPT);
    inst.jsonMsg_ = instJson;
    for (pt::ptree::const_iterator it = jsonPT.begin(); it != jsonPT.end(); ++it) {
        if (it->first == "it:SYMBOL")
            inst.symbol_ = it->second.get_value<std::string>();
        else if (it->first == "it:RIC")
            inst.ric_ = it->second.get_value<std::string>();
        else if (it->first == "it:DOMAIN")
            inst.domain_ = it->second.get_value<std::string>();
        else if (it->first == "ExlTplKey")
            inst.exlKey_ = it->second.get_value<std::string>();
        else if (it->first == "exlObjectFields")
        {
            // drill down in the exlObjectFields
            pt::ptree::const_iterator innerit = it->second.begin();
            for ( ; innerit != it->second.end(); ++innerit) {
                if (innerit->first == "it:DSPLY_NAME")
                    inst.display_name_ = innerit->second.get_value<std::string>();
                else if (innerit->first == "it:UP_ISIN")
                    inst.isin_ = innerit->second.get_value<std::string>();
            }
        }
    }
}
