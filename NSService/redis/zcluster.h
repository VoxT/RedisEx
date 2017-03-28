/* 
 * File:   zcluster.h
 * Author: danhcc
 *
 * Created on Octorber 19, 2016, 9:06 AM
 */

#ifndef ZCLUSTER_H
#define ZCLUSTER_H

#include <thread>
#include "hirediscommand.h"

#include <Poco/Mutex.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>

#include <vector>

#define VA_LENGTH(...) VA_LENGTH_(0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_LENGTH_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N
#define ExecCommandRedis(FuncRedis, ...)  FuncRedis(VA_LENGTH(__VA_ARGS__), __VA_ARGS__)

class ZCluster {
private:
    RedisCluster::Cluster<redisContext>::ptr_t m_pCluster;
    Poco::FastMutex m_mtSync;

private:

    redisReply* ExecRedisCommand(ushort usNumParam, std::string strCommand, ...) {
        if (!m_pCluster || (usNumParam < 2))
            return NULL;

        std::string strKey = "";
        std::string strRedisCmd = strCommand;

        va_list ap;
        va_start(ap, strCommand);
        for (int i = 0; i < usNumParam - 1; i++) {
            if (i == 0) {
                strKey = *(std::string*)va_arg(ap, std::string*);
                strRedisCmd += " " + strKey;
                continue;
            }

            strRedisCmd += " " + *(std::string*)va_arg(ap, std::string*);
        }
        va_end(ap);

        redisReply* reply = NULL;

        m_mtSync.lock();
        try {
            reply = static_cast<redisReply*> (RedisCluster::HiredisCommand<>::Command(m_pCluster, strKey.c_str(), strRedisCmd.c_str()));
        } catch (const RedisCluster::ClusterException &e) {
            reply = NULL;
        } catch (std::exception& ex) {
            reply = NULL;
        }
        m_mtSync.unlock();

        return reply;
    }

    void DeleteRedisReply(redisReply* reply) {
        if (!reply)
            return;

        if ((reply->type != REDIS_REPLY_STRING)
                && (reply->type != REDIS_REPLY_ARRAY)
                && (reply->type != REDIS_REPLY_INTEGER)
                && (reply->type != REDIS_REPLY_NIL)
                && (reply->type != REDIS_REPLY_STATUS)
                && (reply->type != REDIS_REPLY_ERROR)) {
            // reply is garbage
        } else {
            RedisCluster::HiredisCommand<>::deleteReply(reply);
            reply = NULL;
        }
    }

public:

    ZCluster() {
        m_pCluster = NULL;
    }

    ~ZCluster() {
        if (m_pCluster)
            delete m_pCluster;
    }

    bool Init(const std::string& strHost, uint32_t u32Port) {
        try {
            m_pCluster = RedisCluster::HiredisCommand<>::createCluster(strHost.c_str(), u32Port);
            return true;
        } catch (const RedisCluster::ClusterException &e) {

        }
        return false;
    }

    /**
     * 
     * @param strKey
     * @return 0 if the key does not exist.
     * @return 1 if the key exists.
     * @return -1 failed.
     */
    int Exists(const std::string& strKey) {
        int nRet = -1;
        if (strKey.empty() || !m_pCluster)
            return nRet;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "EXISTS", &strKey);

        if (!reply)
            return nRet;

        if (reply->type == REDIS_REPLY_INTEGER) {
            nRet = reply->integer;
        }

        DeleteRedisReply(reply);
        return nRet;
    }

    /**
     * 
     * @param strSet
     * @param strKey
     * @return the number of elements that were added to the set, not including all the elements already present into the set.
     * @return -1 failed.
     */
    int64_t SAdd(const std::string& strSet, const std::string& strKey) {
        int64_t i64Ret = -1;
        if (strSet.empty() || strKey.empty() || !m_pCluster)
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SADD", &strSet, &strKey);
        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    /**
     * 
     * @param strSet
     * @param strKey
     * @return the number of members that were removed from the set, not including non existing members.
     * @return -1 failed.
     */
    int64_t SRem(const std::string& strSet, const std::string& strKey) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strSet.empty() || strKey.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SREM", &strSet, &strKey);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    std::string GetString(const std::string& strKey) {
        std::string strRet = "";
        if (!m_pCluster || strKey.empty())
            return strRet;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "GET", &strKey);

        if (!reply)
            return strRet;

        if ((reply->type == REDIS_REPLY_STRING) && reply->str) {
            strRet = reply->str;
        }

        DeleteRedisReply(reply);
        return strRet;
    }

    uint64_t GetInteger(const std::string& strKey) {
        uint64_t u64Ret = 0;
        Poco::NumberParser::tryParseUnsigned64(GetString(strKey), u64Ret);
        return u64Ret;
    }

    bool Set(const std::string& strKey, const std::string strValue) {
        bool bRet = false;
        if (!m_pCluster || strKey.empty() || strValue.empty())
            return bRet;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SET", &strKey, &strValue);

        if (!reply)
            return bRet;

        if ((reply->type == REDIS_REPLY_STRING) && reply->str) {
            bRet = (strcasecmp(reply->str, "OK") == 0);
        }

        DeleteRedisReply(reply);
        return bRet;
    }

    std::string HGetString(const std::string& strHash, const std::string& strKey) {
        std::string strRet = "";
        if (!m_pCluster || strHash.empty() || strKey.empty())
            return strRet;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "HGET", &strHash, &strKey);

        if (!reply)
            return strRet;

        if ((reply->type == REDIS_REPLY_STRING) && reply->str) {
            strRet = reply->str;
        }

        DeleteRedisReply(reply);
        return strRet;
    }

    uint64_t HGetInteger(const std::string& strHash, const std::string& strKey) {
        uint64_t u64Ret = 0;
        Poco::NumberParser::tryParseUnsigned64(HGetString(strHash, strKey), u64Ret);

        return u64Ret;
    }

    /**
     * 
     * @param strKey
     * @return The number of keys that were removed.
     * @return -1 failed.
     */
    int64_t Del(const std::string& strKey) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strKey.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "DEL", &strKey);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    /**
     * 
     * @param strHash
     * @param strField
     * @param strValue
     * @return 0 Field was updated
     * @return 1 Field was set
     * @return -1 Set failed
     */
    int64_t HSet(const std::string& strHash, const std::string& strField, const std::string& strValue) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strHash.empty() || strField.empty() || strValue.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "HSET", &strHash, &strField, &strValue);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    uint64_t Incr(const std::string& strKey) {
        uint64_t u64Ret = 0;
        if (!m_pCluster || strKey.empty())
            return u64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "INCR", &strKey);

        if (!reply)
            return u64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            u64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return u64Ret;
    }

    /**
     * 
     * @param strKey
     * @param u64ExpireTime
     * @return 1 if the timeout was set.
     * @return 0 if key does not exist or the timeout could not be set.
     * @return -1 Set failed
     */
    uint64_t Expire(const std::string& strKey, uint64_t u64ExpireTime) {
        uint64_t u64Ret = 0;
        if (!m_pCluster || strKey.empty() || (!u64ExpireTime))
            return u64Ret;

        std::string strExpireTime = Poco::NumberFormatter::format(u64ExpireTime);
        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "EXPIRE", &strKey, &strExpireTime);

        if (!reply)
            return u64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            u64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return u64Ret;
    }

    bool SMembersString(const std::string& strSet, std::vector<std::string>& vtElements) {
        vtElements.clear();
        if (!m_pCluster || strSet.empty())
            return false;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SMEMBERS", &strSet);

        if (!reply)
            return false;

        if (reply->type != REDIS_REPLY_ARRAY) {
            DeleteRedisReply(reply);
            return false;
        }

        for (uint32_t i = 0; i < reply->elements; i++) {
            redisReply* element = reply->element[i];

            if (!element)
                continue;
            if (!element->str)
                continue;

            std::string strElement = element->str;
            if (strElement.empty())
                continue;

            vtElements.push_back(strElement);
        }

        DeleteRedisReply(reply);
        return true;
    }

    bool SMembersInteger(const std::string& strSet, std::vector<uint64_t>& vtElements) {
        vtElements.clear();

        std::vector<std::string> vtString;
        if (!SMembersString(strSet, vtString))
            return false;

        uint64_t u64Element = 0;
        for (int i = 0; i < vtString.size(); i++) {
            if (Poco::NumberParser::tryParseUnsigned64(vtString[i], u64Element))
                vtElements.push_back(u64Element);
        }

        return true;
    }

    /**
     * 
     * @param strKey
     * @return the cardinality (number of elements) of the set.
     * @return 0 if key does not exist.
     * @return -1 Set failed
     */
    int64_t SCard(const std::string& strKey) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strKey.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SCARD", &strKey);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    /**
     * 
     * @param strSet
     * @param strKey
     * @return 1 if the element is a member of the set.
     * @return 0 if the element is not a member of the set, or if key does not exist.
     * @return -1 Set failed
     */
    int SIsMember(const std::string& strSet, const std::string& strKey) {
        int nRet = -1;
        if (strSet.empty() || strKey.empty() || !m_pCluster)
            return nRet;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "SISMEMBER", &strSet, &strKey);

        if (!reply)
            return nRet;

        if (reply->type == REDIS_REPLY_INTEGER) {
            nRet = reply->integer;
        }

        DeleteRedisReply(reply);
        return nRet;
    }
    
    /**
     * 
     * @param strZSet
     * @param uScore
     * @param strMember
     * @return The number of elements added to the sorted sets, not including elements already existing for which the score was updated.
     * @return -1 failed.
     */
    int64_t ZAdd(const std::string& strZSet, const uint64_t uScore, const std::string& strMember) {
        int64_t i64Ret = -1;
        if (strZSet.empty() || strMember.empty() || !m_pCluster)
            return i64Ret;
        
        std::string strScore = Poco::NumberFormatter::format(uScore);
        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "ZADD", &strZSet, &strScore, &strMember);
        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    /**
     * 
     * @param strZSet
     * @param strMember
     * @return The number of members removed from the sorted set, not including non existing members.
     * @return -1 failed.
     */
    int64_t ZRem(const std::string& strZSet, const std::string& strMember) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strZSet.empty() || strMember.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "ZREM", &strZSet, &strMember);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }
    
    /**
     * 
     * @param strKey
     * @return the cardinality (number of elements) of the zset.
     * @return 0 if key does not exist.
     * @return -1 Set failed
     */
    int64_t ZCard(const std::string& strKey) {
        int64_t i64Ret = -1;
        if (!m_pCluster || strKey.empty())
            return i64Ret;

        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "ZCARD", &strKey);

        if (!reply)
            return i64Ret;

        if (reply->type == REDIS_REPLY_INTEGER) {
            i64Ret = reply->integer;
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }

    /**
     * 
     * @param strKey
     * @return list of elements in the specified range.
     * @return 0 if key does not exist.
     * @return -1 Set failed
     */
    int64_t ZRange(const std::string& strKey, const int64_t uStart,\
                   const int64_t uStop, std::vector<std::string>& vtElements) 
    {
        vtElements.clear();
        int64_t i64Ret = -1;
        if (!m_pCluster || strKey.empty())
            return i64Ret;
        
        std::string strStart = Poco::NumberFormatter::format(uStart);
        std::string strStop = Poco::NumberFormatter::format(uStop);
        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "ZRANGE",&strKey, &strStart, &strStop);

        if (!reply)
            return i64Ret;

        if (reply->type != REDIS_REPLY_ARRAY)
        {
            DeleteRedisReply(reply);
            return i64Ret;
        }
        
        for (uint32_t i = 0; i < (i64Ret = reply->elements); i++)
        {
            redisReply* element = reply->element[i];
            
            if (!element)
                continue;
            if (!element->str)
                continue;
            
            std::string strKey = element->str;
            if (strKey.empty())
                continue;
            
            vtElements.push_back(strKey);
        }

        DeleteRedisReply(reply);
        return i64Ret;
    }
    
    bool Keys(const std::string& strPattern, std::vector<std::string>& vtKeys)
    {
        vtKeys.clear();
        if (strPattern.empty() || !m_pCluster)
            return false;
        
        redisReply* reply = ExecCommandRedis(ExecRedisCommand, "KEYS", &strPattern);
        
        if (!reply)
            return false;
        
        if (reply->type != REDIS_REPLY_ARRAY)
        {
            DeleteRedisReply(reply);
            return false;
        }
        
        for (uint32_t i = 0; i < reply->elements; i++)
        {
            redisReply* element = reply->element[i];
            
            if (!element)
                continue;
            if (!element->str)
                continue;
            
            std::string strKey = element->str;
            if (strKey.empty())
                continue;
            
            vtKeys.push_back(strKey);
        }
        
        DeleteRedisReply(reply);      
        return true;
    }
};

#endif /* ZCLUSTER_H */

