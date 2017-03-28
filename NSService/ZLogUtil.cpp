/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZLogUtil.cpp
 * Author: root
 * 
 * Created on July 15, 2016, 11:12 AM
 */

#include "ZLogUtil.h"

#include <sys/stat.h>

#include <Poco/Logger.h>
#include <Poco/Util/LoggingConfigurator.h>
#include "Poco/Util/PropertyFileConfiguration.h"
#include <Poco/FileChannel.h>
#include <Poco/NumberFormatter.h>
#include <Poco/AutoPtr.h>
#include <Poco/File.h>
#include <Poco/Path.h>

#include <sstream>
#include <iostream>

using namespace std;

//account
#define ACC_OWNER       1
#define ACC_GROUP       2
#define ACC_OTHER       4
#define ACC_ALL         8
//permission
#define PER_READABLE    1        
#define PER_WRITEABLE   2
#define PER_EXECUTE     4
#define PER_ALL         8

using namespace std;
using namespace Poco;
using namespace Poco::Util;

#define LOG_NAME        "zpconnectionmanager"

ZLogUtil::ZLogUtil()
{
}

ZLogUtil::~ZLogUtil()
{
}

void ZLogUtil::Error(const std::string& strContent)
{
    WriteLog(Message::PRIO_ERROR, LOG_NAME, strContent);
}

void ZLogUtil::Debug(const std::string& strContent)
{
    WriteLog(Message::PRIO_DEBUG, LOG_NAME, strContent);
}

void ZLogUtil::Info(const std::string& strContent)
{
    WriteLog(Message::PRIO_INFORMATION, LOG_NAME, strContent);
}

void ZLogUtil::Warning(const std::string& strContent)
{
    WriteLog(Message::PRIO_WARNING, LOG_NAME, strContent);
}

bool ZLogUtil::SetupLog(const char* szLogPath, uint uLogSize)
{
    return SetupLog(LOG_NAME, szLogPath, uLogSize);
}

bool ZLogUtil::SetupLog(const char* szLogName, const char* szLogPath, uint uLogSize)
{
    if (!szLogName || !szLogPath)
        return false;

    if (!strlen(szLogName) || !strlen(szLogPath))
        return false;

    std::string strLogPath = szLogPath;
    //loai bo dau '/' o cuoi (neu co)
    int nPos = strLogPath.find_last_of('/');
    if (nPos == (strLogPath.length() - 1))
        strLogPath = strLogPath.substr(0, nPos);

    //get folder log
    std::string strLogFolder = strLogPath.substr(0, strLogPath.find_last_of('/'));

    if (!CreateFolderRecursive(strLogFolder.c_str()))
        return false;

    std::string config = std::string("logging.loggers.l1.name = ") + std::string(szLogName) + "\n"
            "logging.loggers.l1.level = debug\n"
            "logging.loggers.l1.channel.class = FileChannel\n"
            "logging.loggers.l1.channel.formatter.class = PatternFormatter\n"
            "logging.loggers.l1.channel.formatter.pattern = %Y-%m-%d %H:%M:%S,%i : {%p} %t\n"
            "logging.loggers.l1.channel.formatter.times = local\n"
            + std::string("logging.loggers.l1.channel.path = ") + strLogPath + "\n"
            "logging.loggers.l1.channel.archive = timestamp\n"
            + std::string("logging.loggers.l1.channel.rotation = ") + Poco::NumberFormatter::format(uLogSize) + " M\n";

    std::istringstream istr(config);
    AutoPtr<PropertyFileConfiguration> pConfig = new PropertyFileConfiguration(istr);

    LoggingConfigurator configurator;
    configurator.configure(pConfig);

    AutoPtr<FileChannel> pChannel(new FileChannel);
    Logger::root().setChannel(pChannel);

    // Logger::get(szLogName).debug("Setup log success");

    return true;
}

bool ZLogUtil::SetPathPermission(const char* path, int account, unsigned int permission)
{
    unsigned int mode = 0;

    if (!path)
        return false;

    if (account == ACC_OWNER) {
        switch (permission) {
        case PER_ALL: mode = S_IRWXU;
            break;
        case PER_WRITEABLE: mode = S_IRUSR | S_IWUSR;
            break;
        case PER_EXECUTE: mode = S_IXUSR | S_IXUSR;
            break;
        default: mode = S_IRUSR;
        }
    }
    else if (account == ACC_GROUP) {
        switch (permission) {
        case PER_ALL: mode = S_IRWXG;
            break;
        case PER_WRITEABLE: mode = S_IRGRP | S_IWGRP;
            break;
        case PER_EXECUTE: mode = S_IRGRP | S_IXGRP;
            break;
        default: mode = S_IRGRP;
        }
    }
    else if (account == ACC_OTHER) {
        switch (permission) {
        case PER_ALL: mode = S_IRWXO;
            break;
        case PER_WRITEABLE: mode = S_IROTH | S_IWOTH;
        case PER_EXECUTE: mode = S_IROTH | S_IXOTH;
            break;
        default: mode = S_IROTH;
        }
    }
    else {
        switch (permission) {
        case PER_ALL: mode = S_IRWXU | S_IRWXG | S_IRWXO;
            break;
        case PER_WRITEABLE: mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            break;
        case PER_EXECUTE: mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
            break;
        default: mode = S_IRUSR | S_IRGRP | S_IROTH;
        }
    }

    struct stat sb;
    if (stat(path, &sb) == -1)
        return false;
    unsigned int current_mode = sb.st_mode;

    unsigned int mask = 0;
    if (account == ACC_OWNER)
        mask = 63; //000111111
    else if (account == ACC_GROUP)
        mask = 455; //111000111
    else if (account == ACC_OTHER)
        mask = 504; //111111000

    mode = (mode | (current_mode & mask));

    if (chmod(path, mode) == -1)
        return false;
    return true;
}

bool ZLogUtil::CreateFolderRecursive(const char* szPath)
{
    if (!szPath)
        return false;

    if (!szPath[0])
        return true;

    //std::string strPath;
    bool bRet = true;
    try {
        //CHelper::wchar_utf8(szPath, strPath);
        //Poco::File f(strPath);
        ////f.createDirectories();
        Poco::File f(szPath);
        if (f.exists()) {
            SetPathPermission(szPath, ACC_ALL, PER_ALL);
        }
        else {
            Poco::Path p(szPath);
            p.makeDirectory();
            if (p.depth() > 1) {
                p.makeParent();
                Poco::File f1(p);
                if (!f1.exists()) {
                    if (!CreateFolderRecursive(f1.path().c_str()))
                        return false;
                }
                SetPathPermission(p.toString().c_str(), ACC_ALL, PER_ALL);
            }
            if (!f.createDirectory())
                return false;
            SetPathPermission(szPath, ACC_ALL, PER_ALL);
        }
    }
    catch (Poco::Exception& ex) {
        bRet = false;
    }
    return bRet;
}

bool ZLogUtil::WriteLog(int nLogPrio, const std::string& strLogName, const std::string& strContent)
{
#ifndef WRITE_LOG
    return true;
#endif

    if (!strlen(strLogName.c_str()) || !strlen(strContent.c_str()))
        return false;

    Lock();
    try {
        switch (nLogPrio) {
        case Poco::Message::PRIO_CRITICAL:
            Poco::Logger::get(strLogName).critical(strContent);
            break;
        case Poco::Message::PRIO_DEBUG:
            Poco::Logger::get(strLogName).debug(strContent);
            break;
        case Poco::Message::PRIO_ERROR:
            Poco::Logger::get(strLogName).error(strContent);
            break;
        case Poco::Message::PRIO_FATAL:
            Poco::Logger::get(strLogName).fatal(strContent);
            break;
        case Poco::Message::PRIO_INFORMATION:
            Poco::Logger::get(strLogName).information(strContent);
            break;
        case Poco::Message::PRIO_NOTICE:
            Poco::Logger::get(strLogName).notice(strContent);
            break;
        case Poco::Message::PRIO_TRACE:
            Poco::Logger::get(strLogName).trace(strContent);
            break;
        case Poco::Message::PRIO_WARNING:
            Poco::Logger::get(strLogName).warning(strContent);
            break;
        default:
        {
            Unlock();
            return false;
        }
        }
    }
    catch (Poco::Exception&) {
        Unlock();
        return false;
    }

    Unlock();
    return true;
}

void ZLogUtil::Lock()
{
    m_mtSync.lock();
}

void ZLogUtil::Unlock()
{
    m_mtSync.unlock();
}