/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiMsgHandler.cpp
 * Author: Thieu Vo
 * 
 * Created on March 21, 2017, 2:43 PM
 */

#include "ZApiMsgHandler.h"
#include "ZApiUtil.h"
#include "ZApiExceptionHandler.h"
#include "ZRedisUtil.h"
#include "ZApiDefine.h"
#include "ZStringDefine.h"

bool ZApiMsgHandler::ProceessData(const std::string& strMsgData)
{
    if (strMsgData.empty())
        return false;
    
    return (rand() % 2);
}

void ZApiMsgHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    uint64_t uReqTime = ZApiUtil::GetTimestampMillis();
    std::ostream& respStream = response.send();
    
    std::string strJsonData, strError;
    if (!ZApiUtil::GetJsonDataFromRequest(request, strJsonData, strError))
    {
        respStream << strError;
        return;
    }

    std::string strMsgData;
    uint64_t uUserId = 0, uSenderId = 0;
    if (!ZApiUtil::GetValueFromJsonString<uint64_t>(strJsonData, API_KEY_SENDER_ID, uSenderId))
    {
        respStream << ZApiUtil::HandleResult(API_RES_PARSE_FAIL, "get senderId from json failed.", true);
        return;
    }
    if (!ZApiUtil::GetValueFromJsonString<uint64_t>(strJsonData, API_KEY_USER_ID, uUserId))
    {
        respStream << ZApiUtil::HandleResult(API_RES_PARSE_FAIL, "get userId from json failed.", true);
        return;
    }
    if (!ZApiUtil::GetValueFromJsonString<std::string>(strJsonData, API_KEY_DATA, strMsgData))
    {
        respStream << ZApiUtil::HandleResult(API_RES_PARSE_FAIL, "get data from json failed.", true);
        return;
    }
    
    bool bPMsgResult = ProceessData(strMsgData);
    uint64_t uSendTime = ZApiUtil::GetTimestampMillis() + (rand() % 1000 + 10);
    
    if (!ZRedisUtil::GetInstance().SaveInfo(uSenderId, uUserId, strMsgData, bPMsgResult, uReqTime, uSendTime))
    {
        respStream << ZApiUtil::HandleResult(API_RES_SAVE_MSG_FAIL, "save info failed.", true);
        return;
    }
    
    respStream << ZApiUtil::HandleResult(API_RES_SUCCESS, "save info succeed.", true);
}

