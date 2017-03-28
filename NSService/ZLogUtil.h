/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZLogUtil.h
 * Author: root
 *
 * Created on July 15, 2016, 11:12 AM
 */

#ifndef ZLOGUTIL_H
#define ZLOGUTIL_H

#include <Poco/NamedMutex.h>
#include <Poco/Mutex.h>
#include <string>

using namespace std;
using namespace Poco;

class ZLogUtil
{
public:

    static ZLogUtil& instance()
    {
        static ZLogUtil inst;
        return inst;
    }
    ZLogUtil(const ZLogUtil& orig) = delete;
    virtual ~ZLogUtil();

    bool SetupLog(const char* szLogPath, uint uLogSize);
    void Error(const std::string& strContent);
    void Debug(const std::string& strContent);
    void Info(const std::string& strContent);
    void Warning(const std::string& strContent);

private:
    Poco::FastMutex m_mtSync;

    ZLogUtil();
    bool SetupLog(const char* szLogName, const char* szLogPath, uint uLogSize);
    bool WriteLog(int nLogPrio, const std::string& strLogName, const std::string& strContent);
    void Lock();
    void Unlock();

    static bool SetPathPermission(const char* path, int account, unsigned int permission);
    static bool CreateFolderRecursive(const char* szPath);

};

#endif /* ZLOGUTIL_H */

