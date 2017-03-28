/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRedisUtil.h
 * Author: Thieu Vo
 *
 * Created on March 22, 2017, 9:43 AM
 */

#ifndef ZREDISUTIL_H
#define ZREDISUTIL_H

#include "redis/zcluster.h"
#include "ZRedisDefine.h"

class ZRedisUtil {
private:
    ZCluster m_zCluster;

public:
    static ZRedisUtil& GetInstance();
    bool Init(const std::string& host, uint32_t port);
    
    
    std::string GetMsgKey(const std::string& strMsgId)
    {
        if(strMsgId.empty())
            return "";
        
        return std::string("ns:msg:" + strMsgId + ":info");
    }
    
    std::string GetUserMsgListKey(const std::string& strUserId)
    {
        if(strUserId.empty())
            return "";
        
        return std::string("ns:msg_list_of_user:" + strUserId);
    }
    
    std::string GetSenderMsgListKey(const std::string& strSenderId)
    {
        if(strSenderId.empty())
            return "";
        
        return std::string("ns:msg_list_of_sender:" + strSenderId);
    }

    
    bool SavedInfo(const std::string& strSenderId, const std::string& strUserId,\
                   const std::string& strMsgData, bool bProcessedMsgResult,\
                   const uint64_t uReqTime, const uint64_t uSendTime);
    
    bool GetSenderStatistic(const std::string& strSenderId);
};

#endif /* ZREDISUTIL_H */

