/* 
 * File:   CVMInterface.h
 * Author: Richard Ranft
 *
 * Created on June 27, 2014, 12:15 PM
 * 
 * This class is intended to be used solely from Lua.  All of its actual functional methods are private
 * for this reason.
 *
 */

#ifndef CVMINTERFACE_H
#define	CVMINTERFACE_H

#include <string>
#include <vector>

#ifndef WINDOWS
#include "vix.h"
#else
#include "vix.h"
#endif

#include "luna.h"

using namespace std;

class CVMInterface {
public:
    CVMInterface();
    CVMInterface(lua_State*);
    virtual ~CVMInterface();

    static const char *className;
    static const Luna < CVMInterface >::FunctionType methods[];
    static const Luna < CVMInterface >::PropertyType properties[];

    bool isExisting; // This is used by Luna to see whether it's been created by createFromExisting.  Don't set it.
    bool isPrecious; // This is used to tell Luna not to garbage collect the object, in case other objects might reference it.  Set it in your class's constructor.
    
    bool initialize();
    
    int L_startVM(lua_State*);
    int L_stopVM(lua_State*);
    int L_waitForTools(lua_State*);
    int L_runScriptInVM(lua_State*);
    int L_runProgramInVM(lua_State*);
    int L_createFolderInVM(lua_State*);
    int L_removeFolderInVM(lua_State*);
    int L_addSharedFolder(lua_State*);
    int L_removeSharedFolder(lua_State*);
    int L_copyFileToGuest(lua_State*);
    int L_copyFileToHost(lua_State*);
    int L_listDirectoryInGuest(lua_State*);
    int L_setTimeout(lua_State*);
    int L_login(lua_State*);
    int L_logout(lua_State*);
    int L_setUserName(lua_State*);
    int L_setPassword(lua_State*);
    int L_setHostUserName(lua_State*);
    int L_setHostPassword(lua_State*);

private:
    lua_State*  m_luaState;
    VixHandle   m_hostHandle;
    VixHandle   m_jobHandle;
    VixHandle   m_vmHandle;
    int         m_timeout;
    string*     m_hostUser;
    string*     m_hostPassword;
    string*     m_guestUser;
    string*     m_guestPassword;
    string*     m_vmPath;
    bool        m_loggedIn;
    bool        m_toolsActive;
    
    int startVM();
    void abort();
    int stopVM();
    int waitForTools();
    int runScriptInVM(string*, string*);
    int runProgramInVM(string*, string*);
    int createFolderInVM(string*);
    int removeFolderInVM(string*);
    int addSharedFolder(string*, string*, bool writeAccess = true);
    int removeSharedFolder(string*);
    int copyFileToGuest(string*, string*);
    int copyFileToHost(string*, string*);
    vector<string*>* listDirectoryInGuest(string*, vector<string*>*);
    bool setTimeout(int);
    int login(bool isTransient = true);
    int logout();
};

#endif	/* CVMINTERFACE_H */

