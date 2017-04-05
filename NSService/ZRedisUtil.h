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

    std::string GetMsgKey(uint64_t uMsgId);
    std::string GetKeyListMsgByUserID(uint64_t uUserId);
    std::string GetKeyListMsgBySenderID(uint64_t uSenderId);
    
    bool SetProcessedTime(uint64_t uPTime);
    bool UpdateProcessedTime(uint64_t uPTime);
    bool UpdateRequestCounter(bool bProcessedMsgResult);
    
    /**
     * @param uSenderId
     * @param uUserId
     * @param strMsgData
     * @param bProcessedMsgResult
     * @param uReqTime
     * @param uSendTime
     * @return 0 if failed. otherwise return msg id.
     */
    uint64_t SaveMsgInfo(uint64_t uSenderId, uint64_t uUserId,\
                   const std::string& strMsgData, bool bProcessedMsgResult,\
                   uint64_t uReqTime, uint64_t uSendTime);
    
    bool CheckMsgExists(uint64_t uMsgId);
        
public:
    static ZRedisUtil& GetInstance();
    bool Init(const std::string& host, uint32_t port);
            
    bool SaveInfo(uint64_t uSenderId, uint64_t uUserId,\
                   const std::string& strMsgData, bool bProcessedMsgResult,\
                   uint64_t uReqTime, uint64_t uSendTime);
    
    bool GetAllUsers(std::vector<uint64_t>& vtUsers);
    bool GetAllSenders(std::vector<uint64_t>& vtSenders);
    bool GetListUserBySender(uint64_t uSenderId, std::vector<uint64_t>& vtUserId);
    bool GetListSenderByUser(uint64_t uUserId, std::vector<uint64_t>& vtSenderid);
    bool GetListMsgBySender(uint64_t uSenderId, std::vector<uint64_t>& vtMsgId);
    bool GetListMsgByUser(uint64_t uUserId, std::vector<uint64_t>& vtMsgId);
    
    /**
     * @return 0 if failed. otherwise return total request.
     */
    bool GetTotalRequest(uint64_t& uResult);
    bool GetTotalSucceedRequest(uint64_t& uResult);
    bool GetTotalFailedRequest(uint64_t& uResult);
    
    bool GetAverageProcessedTime(uint64_t& uResult);
    bool GetMaxProcessedTime(uint64_t& uResult);
    bool GetMinProcessedTime(uint64_t& uResult);
    
};

#endif /* ZREDISUTIL_H */

