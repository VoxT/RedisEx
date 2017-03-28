/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZApiExceptionHandler.h
 * Author: danhcc
 *
 * Created on October 13, 2016, 3:53 PM
 */

#ifndef ZAPIEXCEPTIONHANDLER_H
#define ZAPIEXCEPTIONHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class ZApiExceptionHandler : public Poco::Net::HTTPRequestHandler
{
public:
    ZApiExceptionHandler(uint32_t u32Code, std::string& strMessage);
    virtual void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    uint32_t m_u32Code;
    std::string m_strMessage;
};

#endif /* ZAPIEXCEPTIONHANDLER_H */

