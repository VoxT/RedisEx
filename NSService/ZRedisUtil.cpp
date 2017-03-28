/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRedisUtil.cpp
 * Author: Thieu Vo
 * 
 * Created on March 22, 2017, 9:43 AM
 */

#include <set>

#include "ZRedisUtil.h"

ZRedisUtil& ZRedisUtil::GetInstance()
{
    static ZRedisUtil zRedis;
    return zRedis;
}

bool ZRedisUtil::Init(const std::string& host, uint32_t port)
{
    if (!m_zCluster.Init(host, port))
    {
        std::cerr << "Connect to Redis failed";
        return false;
    }
    
    return true;
}

bool ZRedisUtil::SaveInfo(const uint64_t uSenderId, const uint64_t uUserId,\
                           const std::string& strMsgData, bool bProcessedMsgResult,\
                           const uint64_t uReqTime, const uint64_t uSendTime)
{
    if (strMsgData.empty() || (uReqTime > uSendTime))
        return false;
    
    // get msgId
    uint64_t uMsgIdIncr = m_zCluster.Incr(RDS_NS_MSG_ID_INCR);
    if (uMsgIdIncr == 0)
        return false;
    
    std::string strMsgId = Poco::NumberFormatter::format(uMsgIdIncr);
    std::string strSenderId = Poco::NumberFormatter::format(uSenderId);
    std::string strUserId = Poco::NumberFormatter::format(uUserId);
    
    // hash msg
    std::string strHash = GetMsgKey(uMsgIdIncr);
    if (strHash.empty())
        return false;
    
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_SENDER_ID, strSenderId) == -1)
        return false;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_USER_ID, strUserId) == -1)
        return false;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_DATA, strMsgData) == -1)
        return false;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FILED_PROCESSED_RESULT, (bProcessedMsgResult?"1":"0")) == -1)
        return false;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_REQUEST_TIME, Poco::NumberFormatter::format(uReqTime)) == -1)
        return false;
    if (m_zCluster.HSet(strHash, RDS_NS_MSG_INFO_FIELD_SEND_TIME, Poco::NumberFormatter::format(uSendTime)) == -1)
        return false;
    
    // set sender
    if (m_zCluster.SAdd(RDS_NS_SENDERS, strSenderId) == -1)
        return false;
    if (m_zCluster.SAdd(RDS_NS_USERS, strUserId) == -1)
        return false;
    
    if (m_zCluster.ZAdd(GetSenderMsgListKey(uSenderId), uReqTime, strMsgId) == -1)
        return false;
    if (m_zCluster.ZAdd(GetUserMsgListKey(uUserId), uSendTime, strMsgId) == -1)
        return false;
    
    return true;
}

bool ZRedisUtil::GetSenderStatistic(const uint64_t uSenderId)
{    
    std::string strKey = GetSenderMsgListKey(uSenderId);
    
    std::vector<std::string> vtMsgId;
    if (m_zCluster.ZRange(strKey, 0, -1, vtMsgId) == -1)
        return false;
    if(vtMsgId.size() == 0)
        return false;
    
    std::set<std::string> setUserId;
    for (uint64_t i = 0; i < vtMsgId.size(); i++)
    {
        uint64_t uMsgId = 0;
        if (!Poco::NumberParser::tryParseUnsigned64(vtMsgId[i], uMsgId))
            return false;
        
        std::string strMsgKey = GetMsgKey(uMsgId);
        
        std::string strUserId = m_zCluster.HGetString(strMsgKey, RDS_NS_MSG_INFO_FIELD_USER_ID);
        if (strUserId.empty())
            return false;
        setUserId.insert(strUserId);
        
        std::cout << strMsgKey << " { user_id: " << strUserId << " }" << std::endl;
    }
    
    std::cout << "sender send to users: " ;
    for (std::set<std::string>::iterator it = setUserId.begin(); it != setUserId.end(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    return true;
}
