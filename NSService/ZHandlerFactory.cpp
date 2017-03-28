/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZHandlerFactory.h"

#include "ZApiDefine.h"
#include "ZApiExceptionHandler.h"
#include "ZLogUtil.h"
#include "ZApiMsgHandler.h"

#include <Poco/StringTokenizer.h>
#include <Poco/NumberFormatter.h>

#include <iostream>

ZHandlerFactory::ZHandlerFactory()
{

}

Poco::Net::HTTPRequestHandler* ZHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    ZLogUtil::instance().Debug("Accept connection from " + request.clientAddress().toString()
                               + " URI " + request.getURI() + " "
                               + Poco::NumberFormatter::format(request.getContentLength64()) + " bytes");

    if (request.getMethod() == "GET") {
        std::string strMsg = "Wrong method";
        return new ZApiExceptionHandler(API_RES_WRONG_METHOD, strMsg);
    }

    std::string strURI = request.getURI();
    Poco::StringTokenizer strTokens(strURI, "/");

    // strTokens[0] is always empty
    std::string strMsg = "Wrong API";
    if (strTokens.count() != 4) {
        return new ZApiExceptionHandler(API_RES_WRONG_API, strMsg);
    }

    if (strTokens[1].compare(API_PATH_NSSERVICE) != 0)
        return new ZApiExceptionHandler(API_RES_WRONG_API, strMsg);

    if (strTokens[2].compare(API_PATH) != 0)
        return new ZApiExceptionHandler(API_RES_WRONG_API, strMsg);


    if (strTokens[3].compare(API_PATH_MSG) == 0) {
        return new ZApiMsgHandler();
    }

    return new ZApiExceptionHandler(API_RES_WRONG_API, strMsg);

}