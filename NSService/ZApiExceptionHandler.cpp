/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZApiExceptionHandler.h"
#include "ZApiUtil.h"

ZApiExceptionHandler::ZApiExceptionHandler(uint32_t u32Code, std::string& strMessage)
{
    m_u32Code = u32Code;
    m_strMessage = strMessage;
}

void ZApiExceptionHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    std::ostream& responseStream = response.send();
    responseStream << ZApiUtil::HandleResult(m_u32Code, m_strMessage, true);
}