/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZHttpServer.h"
#include "ZHandlerFactory.h"
#include "ZConfigReader.h"
#include "ZLogUtil.h"
#include "ZRedisUtil.h"

#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/NumberFormatter.h>

int ZHttpServer::main(const std::vector<std::string>& args)
{
    std::string strWorkingPath = config().getString("application.dir").c_str();
    chdir(strWorkingPath.c_str());

    ZLogUtil::instance().SetupLog("logs/bcservice.log", 2);

    auto &config = ZConfigReader::instance();
    
    Poco::Net::ServerSocket socket(config.m_port);
    ZLogUtil::instance().Debug("Server Start on port:" + Poco::NumberFormatter::format(config.m_port));

    Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
    // pParams->setMaxQueued(64);
    pParams->setMaxThreads(16);

    Poco::Net::HTTPServer server(new ZHandlerFactory(), socket, pParams);
    
    //connect to redis
    if (!ZRedisUtil::GetInstance().Init("127.0.0.1", 8000))
        return EXIT_FAILURE;
    
    std::vector<uint64_t> vtMsg;
    for (uint32_t uSender = 1; uSender < 101; uSender++)
    {
        std::cout<< "List msg by sender " << uSender << ": ";
        if (!ZRedisUtil::GetInstance().GetListMsgBySender(uSender, vtMsg))
            return EXIT_FAILURE;
        for (uint32_t i = 0; i < vtMsg.size(); i++)
        {
            std::cout<< vtMsg[i] << " ";
        }
        std::cout << std::endl;
    }
    
    uint64_t uTotalReq = 0, uTotalReqSucceed = 0, uTotalReqFailed = 0;
    if (!ZRedisUtil::GetInstance().GetTotalRequest(uTotalReq))
        return EXIT_FAILURE;
    if (!ZRedisUtil::GetInstance().GetTotalSucceedRequest(uTotalReqSucceed))
        return EXIT_FAILURE;
    if (!ZRedisUtil::GetInstance().GetTotalFailedRequest(uTotalReqFailed))
        return EXIT_FAILURE;
    
    uint64_t uMaxTime = 0, uMinTime = 0, uAvegTime = 0;
    if (!ZRedisUtil::GetInstance().GetMaxProcessedTime(uMaxTime))
        return EXIT_FAILURE;
    if (!ZRedisUtil::GetInstance().GetMinProcessedTime(uMinTime))
        return EXIT_FAILURE;
    if (!ZRedisUtil::GetInstance().GetAverageProcessedTime(uAvegTime))
        return EXIT_FAILURE;
    
    std::cout << "Total Req: " << uTotalReq\
              << "\nTotal Succeed: " << uTotalReqSucceed\
              << "\nTotal failed: " << uTotalReqFailed\
              << "\nMax Processed time: " << uMaxTime\
              << "\nMin Processed time: " << uMinTime\
              << "\nAvg Processed time: " << uAvegTime;
    
    return EXIT_OK;
    
    server.start(); 

    waitForTerminationRequest();


    server.stop();

    return EXIT_OK;
}