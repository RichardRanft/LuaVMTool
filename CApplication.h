/* 
 * File:   CApplication.h
 * Author: sgp1000
 *
 * Created on June 27, 2014, 11:35 AM
 */

#ifndef CAPPLICATION_H
#define	CAPPLICATION_H

#ifndef WINDOWS
#include <unistd.h>
#include <dirent.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <vector>

#include "lua/lua.hpp"
#include "luna.h"

#include "CVMInterface.h"

using namespace std;

// -----------------------------------------------------------------------------
// Types to support Lua call passing and multiple return values
// -----------------------------------------------------------------------------
typedef enum { DBL = 0, ITGR, STRG }luaDataType;

typedef union
{
    double      d;
    int         i;
    char*       c;
}luaData;

typedef struct
{
    int type;
    luaData data;
}luaResult;

typedef class lua_pcall_args
{
public:
    lua_pcall_args()
    {
        argSpec = NULL;
        retVals.clear();
    }
    ~lua_pcall_args()
    {
        if (argSpec != NULL)
            delete(argSpec);
        retVals.clear();
    }

    string* argSpec;
    vector<luaData> retVals;
} pcall_args;

// -----------------------------------------------------------------------------

class CApplication {
public:
    CApplication();
    CApplication(int, char**);
    CApplication(const CApplication& orig);
    virtual ~CApplication();
    
    bool initialize();
    bool startLua();
    bool callLua(std::vector<luaResult>& results, const char* func, const char* sig, ... );
    void error (const char *fmt, ...);
    bool loadScripts();
    int run();

private:
    lua_State*          m_luaState;
    int                 m_argumentCount;
    vector<string*>*    m_argumentList;
    bool                m_luaStarted;
    bool                m_initialized;
    
    bool                runScript();
    void		getScriptFileName(char*, string&, const char*);
    bool		getFileList(string*, vector<string*>*);
};

#endif	/* CAPPLICATION_H */

