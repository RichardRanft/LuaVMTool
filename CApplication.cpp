/* 
 * File:   CApplication.cpp
 * Author: sgp1000
 * 
 * Created on June 27, 2014, 11:35 AM
 */

#ifndef WINDOWS
#include <unistd.h>
#include <dirent.h>
#else
#include <Windows.h>
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

#include "CApplication.h"
#include "lua/lua.hpp"

#include "CVMInterface.h"

using namespace std;

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

CApplication::CApplication() {
    m_luaState = NULL;
    m_argumentCount = 0;
    m_argumentList = new vector<string*>();
    m_luaStarted = false;
    m_initialized = false;
}

CApplication::CApplication(int argc, char** argv)
{
    // start up with command parameters
    m_argumentCount = argc;
    m_argumentList = new vector<string*>();
    for (int i = 0; i < argc; i++)
    {
        m_argumentList->push_back(new string(argv[i]));
    }
    m_luaStarted = false;
    m_initialized = false;
}

CApplication::CApplication(const CApplication& orig) {
}

CApplication::~CApplication() {
    if( m_argumentList != NULL )
    {
        m_argumentList->clear();
        delete(m_argumentList);
    }
    if( m_luaState != NULL )
        lua_close(m_luaState);
#ifdef DEBUG
    cout << " -- CApplication::~CApplication() complete" << endl;
#endif
}

bool CApplication::initialize()
{
    if(startLua())
    {
#ifdef DEBUG
        cout << " -- Lua started, registering objects with Lua... " << endl;
#endif
        m_luaStarted = true;
    }
    Luna<CVMInterface>::Register(m_luaState);
    m_initialized = true;
    return m_initialized;
}

int CApplication::run()
{
    if( !m_initialized )
    {
        if( !initialize() )
            return -1;
    }
#ifdef DEBUG
    if( m_argumentCount > 0 )
    {
        cout << endl << " -- Argument list:" << endl;
        for( vector<string*>::iterator it = m_argumentList->begin(); it != m_argumentList->end(); it++ )
        {
            cout << *(*it) << endl;
        }
        cout << endl;
    }
    runScript();
#endif
    return 0;
}

bool CApplication::startLua()
{
    if (m_luaState == NULL || m_luaStarted == false)
    {
        m_luaState = luaL_newstate();
#ifdef DEBUG
        cout << " -- Lua State " << m_luaState << " created." << endl;
#endif
        /* open standard libraries */
        luaL_checkversion(m_luaState);
        lua_gc(m_luaState, LUA_GCSTOP, 0);  /* stop collector during initialization */
        luaL_openlibs(m_luaState);  /* open libraries */

        // expose functions to the Lua global namespace

        // restart garbage collection.
        lua_gc(m_luaState, LUA_GCRESTART, 0);
    }

    if (m_luaState == NULL)
        m_luaStarted = false;
    else
    {
        loadScripts();
        m_luaStarted = true;
    }

    return m_luaStarted;
}

bool CApplication::callLua(std::vector<luaResult>& results, const char* func, const char* sig, ... )
{
#ifdef DEBUG
    cout << " -- callLua checking function " << func << " in LuaState " << m_luaState << endl;
#endif
    lua_getglobal(m_luaState, func);  /* get function */
    if (lua_isfunction(m_luaState, -1))
    {
        va_list vl;
        int narg, nres;  /* number of arguments and results */
        va_start(vl, sig);
        /* push arguments */
        narg = 0;
        while (*sig)
        {  /* push arguments */
            switch (*sig++)
            {
                case 'd':  /* double argument */
                {
                    double dArg = va_arg(vl, double);
                    lua_pushnumber(m_luaState, dArg);
#ifdef DEBUG
                    cout << " -- parsed " << dArg << endl;
#endif
                    break;
                }
                case 'i':  /* int argument */
                {
                    int iArg = va_arg(vl, int);
                    lua_pushnumber(m_luaState, iArg);
#ifdef DEBUG
                    cout << " -- parsed " << iArg << endl;
#endif
                    break;
                }
                case 's':  /* string argument */
                {
                    char* cArg = va_arg(vl, char*);
                    lua_pushstring(m_luaState, cArg);
#ifdef DEBUG
                    cout << " -- parsed " << cArg << endl;
#endif
                    break;
                }
                case '>':
                    goto endwhile;

                default:
                    error(" -- invalid option (%c)\n", *(sig - 1));
            }
            narg++;
            luaL_checkstack(m_luaState, 1, "too many arguments");
        } endwhile:
        va_end(vl);
        
        /* do the call */
        nres = strlen(sig);  /* number of expected results */
        if (lua_pcall(m_luaState, narg, nres, 0) != 0)  /* do the call */
        {
            error(" -- error running function `%s': %s\n", func, lua_tostring(m_luaState, -1));
        }

        /* retrieve results */
        nres = -nres;  /* stack index of first result */
        results.clear();
        int count = 0;
        luaResult *temp;
        while (*sig) {  /* get results */
            temp = new luaResult();
            switch (*sig++) {
                case 'd':  /* double result */
                    if (!lua_isnumber(m_luaState, nres))
                        error(" -- wrong result type: expected double\n");
                    temp->type = DBL;
                    temp->data.d = (double)lua_tonumber(m_luaState, nres);
                    results.push_back(*temp);
                    break;

                case 'i':  /* int result */
                    if (!lua_isnumber(m_luaState, nres))
                        error(" -- wrong result type: expected int\n");
                    temp->type = ITGR;
                    temp->data.i = (int)lua_tonumber(m_luaState, nres);
                    results.push_back(*temp);
                    break;

                case 's':  /* string result */
                    if (!lua_isstring(m_luaState, nres))
                        error(" -- wrong result type: expected string\n");
                    temp->type = STRG;
                    temp->data.c = (char*)lua_tostring(m_luaState, nres);
                    results.push_back(*temp);
                    break;

                default:
                    error(" -- invalid option (%c)\n", *(sig - 1));
            }
            nres++;
            count++;
        }
        return true;
    }
    else
    {
        error(" -- %s is not a registered Lua function.\n", func);
        return false;
    }
}

bool CApplication::runScript()
{
#ifdef DEBUG
    cout << " -- entering CApplication::runScript()..." << endl << endl;
#endif
    bool result = false;
    if( m_argumentCount == 2 )
    {
#ifdef DEBUG
    cout << " -- CApplication::runScript(): " << m_argumentCount << endl << endl;
#endif
        // just run the script provided on the command line
        result = luaL_dofile(m_luaState, m_argumentList->back()->c_str());
    }
    else if ( m_argumentCount > 2 )
    {
#ifdef DEBUG
    cout << " -- CApplication::runScript(): " << m_argumentCount << endl << endl;
#endif
        // run the script with parameters from the command line
        string* filename = m_argumentList->at(1);
#ifdef DEBUG
    cout << " -- CApplication::runScript(): load " << filename << endl;
#endif
        luaL_loadfile(m_luaState, filename->c_str());
#ifdef DEBUG
    cout << " -- CApplication::runScript(): prepare to run script..." << endl;
#endif
        lua_newtable(m_luaState);
        for( int i = 2; i < m_argumentCount; i++ )
        {
            lua_pushnumber(m_luaState, i-1);
            lua_pushstring(m_luaState, m_argumentList->at(i)->c_str());
            lua_rawset(m_luaState, -3);
        }
        lua_setglobal(m_luaState, "arg");
#ifdef DEBUG
    cout << " -- CApplication::runScript(): try to run with arguments..." << endl << endl;
#endif
        int status = lua_pcall(m_luaState, 0, 1, 0);
        int result = lua_toboolean(m_luaState, -1);  /* get result */
    }
#ifdef DEBUG
    cout << endl << " -- CApplication::runScript() complete." << endl << endl;
#endif
    return result;
}

void CApplication::error (const char *fmt, ...) 
{
    va_list argp;
    va_start(argp, fmt);
    printf(fmt, argp);
    va_end(argp);
}

#ifndef WINDOWS
bool CApplication::loadScripts()
{
#ifdef DEBUG
    printf(" -- CApplication::loadScripts - get list from ./scripts....\n");
#endif
    struct dirent *dir_entry_pointer;
    std::string srcDir = "./packetScripts/";
    DIR *dir_pointer = opendir(srcDir.c_str());
    if(dir_pointer != NULL)
    {
        char srcFileName[PATH_MAX+1];
        memset(srcFileName, '\0', PATH_MAX+1);
        bool result = false;
#ifdef DEBUG
        printf(" -- CApplication::loadScripts - begin loading scripts.\n");
#endif
        std::string dirName = "";

        while((dir_entry_pointer = readdir(dir_pointer)) != NULL)
        {
            dirName = dir_entry_pointer->d_name;
            if (strcmp(dirName.c_str(), ".") == 0 || strcmp(dirName.c_str(), "..") == 0)
                continue;
            strcpy(srcFileName, srcDir.c_str());
            strcat(srcFileName, dir_entry_pointer->d_name);
#ifdef DEBUG
            cout << " -- CApplication::loadScripts -(" << m_luaState << ") loading " << dir_entry_pointer->d_name << endl;
#endif
            result = luaL_dofile(m_luaState, srcFileName);
            if ( result )
            {
#ifdef DEBUG
                printf(" -- CApplication::loadScripts - error loading %s: %s\n", dir_entry_pointer->d_name, lua_tostring(m_luaState, -1));
#endif
                memset(srcFileName, '\0', PATH_MAX+1);
                continue;
            }
#ifdef DEBUG
            printf(" -- CApplication::loadScripts - loaded %s\n", dir_entry_pointer->d_name);
#endif
            memset(srcFileName, '\0', PATH_MAX+1);
        }
    }
    closedir(dir_pointer);
    dir_pointer = NULL;
    return true;
}

#else

bool CApplication::loadScripts()
{
	vector<string*>* data = new vector<string*>();
	string* dir = new string(".");
	int result = 0;
	getFileList(dir, data);
	string fileName = "";
	string dOut = " -- Begin loading scripts...\n";
	OutputDebugString(dOut.c_str());
	if (data != NULL && data->size() > 0)
	{
		for (vector<string*>::iterator i = data->begin(); i != data->end(); i++)
		{
			fileName = dir->c_str();
			fileName.append("/");
			fileName.append(**i);
			dOut.clear();
			dOut = " -- Loading ";
			dOut.append(fileName);
			dOut.append("\n");
			OutputDebugString(dOut.c_str());
			result = luaL_dofile(m_luaState, fileName.c_str());
		}
		dOut.clear();
		dOut = " -- Scripts loaded\n";
		OutputDebugString(dOut.c_str());
		return true;
	}
	dOut.clear();
	dOut = " -- No scripts found in ";
	dOut.append(dir->c_str());
	dOut.append("\n");
	OutputDebugString(dOut.c_str());
	return false;
}

bool CApplication::getFileList(string *dirName, vector<string*>* target)
{
	char psBuffer[128];
	string command = "dir ";
	command.append(dirName->c_str());
	FILE* data = _popen(command.c_str(), "rt");
	if (data != NULL)
	{
		int line = 0;
		while (fgets(psBuffer, 128, data))
		{
			line++;
			if (line > 7)
			{
				string* templine = new string(psBuffer);
				if (templine->find(".lua") != string::npos)
				{
					string fileName;
					getScriptFileName(psBuffer, fileName, " ");
					if (fileName != "")
						target->push_back(new string(fileName));
				}
			}
		}
		return true;
	}
	return false;
}

void CApplication::getScriptFileName(char* str, string& dest, const char* delim)
{
	char* nextTok;
	char* token = strtok_s(str, delim, &nextTok);
	string whitespaces(" \t\f\v\n\r");
	string tok;
	string result;

	while (token != NULL)
	{
		tok = token;
		if (tok.find(".lua") != string::npos)
		{
			result = token;
			size_t found = result.find_last_not_of(whitespaces);
			if (found != string::npos)
				result.erase(found + 1);
			else
				result.clear();			break;
		}
		token = strtok_s(NULL, delim, &nextTok);
	}
	dest = result.c_str();
}
#endif
