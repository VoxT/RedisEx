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

bool ZRedisUtil::SavedInfo(const std::string& strSenderId, const std::string& strUserId,\
                           const std::string& strMsgData, bool bProcessedMsgResult,\
                           const uint64_t uReqTime, const uint64_t uSendTime)
{
    if (strMsgData.empty() || strSenderId.empty() || strUserId.empty())
        return false;
    
    // get msgId
    uint64_t uMsgIdIncr = m_zCluster.Incr(RDS_NS_MSG_ID_INCR);
    if (uMsgIdIncr == 0)
        return false;
    std::string strMsgId = Poco::NumberFormatter::format(uMsgIdIncr);
    
    // hash msg
    std::string strHash = GetMsgKey(strMsgId);
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
    
    if (m_zCluster.ZAdd(GetSenderMsgListKey(strSenderId), uReqTime, strMsgId) == -1)
        return false;
    if (m_zCluster.ZAdd(GetUserMsgListKey(strUserId), uSendTime, strMsgId) == -1)
        return false;
    
    return true;
}

bool ZRedisUtil::GetSenderStatistic(const std::string& strSenderId)
{
    if (strSenderId.empty())
        return false;
    
    std::string strKey = GetSenderMsgListKey(strSenderId);
    if(strKey.empty())
        return false;
    
    std::vector<std::string> vtMsgId;
    if (m_zCluster.ZRange(strKey, 0, -1, vtMsgId) == -1)
        return false;
    if(vtMsgId.size() == 0)
        return false;
    
    std::set<std::string> setUserId;
    for (uint64_t i = 0; i < vtMsgId.size(); i++)
    {
        std::string strMsgKey = GetMsgKey(vtMsgId[i]);
        if (strMsgKey.empty())
            return false;
        
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
