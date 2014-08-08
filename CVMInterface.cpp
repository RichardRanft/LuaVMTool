/* 
 * File:   CVMInterface.cpp
 * Author: Richard Ranft
 * 
 * Created on June 27, 2014, 12:15 PM
 */

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

#include "CVMInterface.h"

#ifndef WINDOWS
#include "/usr/include/vmware-vix/vix.h"
#else
#include "vix.h"
#endif

using namespace std;

#define  HOSTTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define  HOSTNAME NULL
#define  HOSTPORT 0

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_NORMAL   // Launches the VMware Workstaion UI
                                                     // when powering on the virtual machine.

#define VMXPATH_INFO "where vmxpath is an absolute path to the .vmx file " \
                     "for the virtual machine."

extern int TOOLS_TIMEOUT;

// ----------------------------------------------------------------------------
// constructors
CVMInterface::CVMInterface() {
    m_luaState = NULL;
    m_hostHandle = VIX_INVALID_HANDLE;
    m_jobHandle = VIX_INVALID_HANDLE;
    m_vmHandle = VIX_INVALID_HANDLE;
    m_guestUser = NULL;
    m_guestPassword = NULL;
    m_hostUser = NULL;
    m_hostPassword = NULL;
    m_vmPath = NULL;
    m_timeout = 300;
    m_loggedIn = false;
    m_toolsActive = false;
    TOOLS_TIMEOUT = m_timeout;
#ifdef DEBUG
    cout << " -- CVMInterface::CVMInterface() complete" << endl;
#endif
}

CVMInterface::CVMInterface(lua_State* L) {
    m_luaState = L;
    m_hostHandle = VIX_INVALID_HANDLE;
    m_jobHandle = VIX_INVALID_HANDLE;
    m_vmHandle = VIX_INVALID_HANDLE;
    m_guestUser = NULL;
    m_guestPassword = NULL;
    m_hostUser = NULL;
    m_hostPassword = NULL;
    m_vmPath = NULL;
    m_timeout = 300;
    m_loggedIn = false;
    m_toolsActive = false;
    TOOLS_TIMEOUT = m_timeout;
#ifdef DEBUG
    cout << " -- CVMInterface::CVMInterface() complete" << endl;
#endif
}

// ----------------------------------------------------------------------------
// destructor
CVMInterface::~CVMInterface() {
    m_luaState = NULL;
    m_hostHandle = VIX_INVALID_HANDLE;
    m_jobHandle = VIX_INVALID_HANDLE;
    m_vmHandle = VIX_INVALID_HANDLE;
    delete(m_guestUser);
    delete(m_guestPassword);
    delete(m_hostUser);
    delete(m_hostPassword);
    delete(m_vmPath);
#ifdef DEBUG
    cout << " -- CVMInterface::~CVMInterface() complete" << endl;
#endif
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::initialize()
/// Performs initialization that could fail, and therefore should not be
/// performed in the constructor.
/// </summary>
/// <return>true if successful, false otherwise.</return>
bool CVMInterface::initialize()
{
#ifdef DEBUG
    cout << " -- CVMInterface::initialize() complete" << endl;
#endif
    return true;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::abort()
/// Performs cleanup in the event that one of the VIX interface calls fails.
/// </summary>
void CVMInterface::abort()
{
#ifdef DEBUG
    cout << " --- CVMInterface::abort()" << endl;
#endif
    if( m_loggedIn )
    {
        logout();
        stopVM();
    }
//    Vix_ReleaseHandle(m_jobHandle);
//    Vix_ReleaseHandle(m_vmHandle);
//    VixHost_Disconnect(m_hostHandle);
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::startVM()
/// Starts the selected virtual machine.  The path to the vm file is set in the
/// lua interface call.
/// </summary>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::startVM()
{
#ifdef DEBUG
    cout << " --- CVMInterface::startVM()" << endl;
#endif
    // vm start step
    VixError err;
    int result = 0;
    if( m_hostUser == NULL )
        m_hostUser = new string("");
    if( m_hostPassword == NULL )
        m_hostPassword = new string("");
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixHost_Connect()..." << endl;
    cout << " --- CVMInterface::startVM() host user name : " << m_hostUser->c_str() << endl;
    cout << " --- CVMInterface::startVM() host password  : " << m_hostPassword->c_str() << endl;
    cout << " --- CVMInterface::startVM() guest user name : " << m_guestUser->c_str() << endl;
    cout << " --- CVMInterface::startVM() guest password  : " << m_guestPassword->c_str() << endl;
    cout << " --- starting " << *(m_vmPath) << endl;
#endif
    m_jobHandle = VixHost_Connect(VIX_API_VERSION,
                                HOSTTYPE,
                                HOSTNAME,                       // *hostName,
                                HOSTPORT,                       // hostPort,
								m_hostUser->c_str(),            // *userName,
								m_hostPassword->c_str(),        // *password,
                                0,                              // options,
                                VIX_INVALID_HANDLE,             // propertyListHandle,
                                NULL,                           // *callbackProc,
                                NULL);                          // *clientData);
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &m_hostHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        result = 1;
    }

#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixVM_Open()..." << endl;
#endif
    m_jobHandle = VixVM_Open(m_hostHandle,
                           m_vmPath->c_str(),
                           NULL,        // VixEventProc *callbackProc,
                           NULL);       // void *clientData);
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, 
                      VIX_PROPERTY_JOB_RESULT_HANDLE, 
                      &m_vmHandle,
                      VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        result = 1;
    }

#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixVM_PowerOn()..." << endl;
#endif
    m_jobHandle = VixVM_PowerOn(m_vmHandle,
                              VMPOWEROPTIONS,
                              VIX_INVALID_HANDLE,
                              NULL,     // *callbackProc,
                              NULL);    // *clientData);
#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        result = 1;
    }

#ifdef DEBUG
    cout << " --- CVMInterface::startVM() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::stopVM()
/// Stops the selected virtual machine.
/// </summary>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::stopVM()
{
#ifdef DEBUG
    cout << " --- CVMInterface::stopVM()" << endl;
    //return 0;
#endif
    // vm start step
    VixError err;
    int result = 0;
#ifdef DEBUG
    cout << " --- CVMInterface::stopVM() calling VixVM_PowerOff()..." << endl;
#endif
    m_jobHandle = VixVM_PowerOff(m_vmHandle,
                               VIX_VMPOWEROP_NORMAL,
                               NULL,    // *callbackProc,
                               NULL);   // *clientData);
#ifdef DEBUG
    cout << " --- CVMInterface::stopVM() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
#ifdef DEBUG
    cout << " --- CVMInterface::stopVM() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        result = 1;
    }
#ifdef DEBUG
    cout << " --- CVMInterface::stopVM() vm stopped" << endl;
#endif
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::runScriptInVM(interpreter, scriptText)
/// Runs the provided scriptText verbatim using the specified interpreter.
/// </summary>
/// <param name="interpreter">std::string* with the name of the script interpreter to use i.e. /bin/bash.</param>
/// <param name="scriptText">std::string* containing the script text to run.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::runScriptInVM(string* interpreter, string* scriptText)
{
    // runs script in guest
    VixError err;
    int result = 0;
    if( interpreter->length() < 1 )
        interpreter->append("\"\"");
#ifdef DEBUG
    cout << " --- CVMInterface::runScriptInVM(" << interpreter->c_str() << ", " << scriptText->c_str() << ")" << endl;
#endif
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::runScriptInVM() - Logging into guest..." << endl;
#endif
        login();
#ifdef DEBUG
        cout << " --- CVMInterface::runScriptInVM() - Logged in." << endl;
#endif
    }
#ifdef DEBUG
    cout << " --- CVMInterface::runScriptInVM() - about to do work" << endl;
#endif
    m_jobHandle = VixVM_RunScriptInGuest(m_vmHandle,
                                        interpreter->c_str(),   // command
                                        scriptText->c_str(),    // cmd args
                                        0,                      // options
                                        VIX_INVALID_HANDLE,     // prop handle
                                        NULL,                   // callback
                                        NULL);                  // client data
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to run script in virtual machine '%s'(%"FMT64"d %s)\n",
            m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
        result = 1;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::runScriptInVM() - logging out of guest..." << endl;
#endif
        logout();
#ifdef DEBUG
        cout << " --- CVMInterface::runScriptInVM() - logged out" << endl;
#endif
    }
#ifdef DEBUG
    cout << " --- CVMInterface::runScriptInVM() - work complete" << endl;
#endif
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::runProgramInVM(program, args)
/// Runs the program using the specified arguments.
/// </summary>
/// <param name="program">std::string* with the name of the program to run.</param>
/// <param name="args">std::string* containing program arguments.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::runProgramInVM(string* program, string* args)
{
    // runs program in guest
    VixError err;
    int result = 0;
#ifdef DEBUG
    cout << " --- CVMInterface::runProgramInVM(" << program->c_str() << ", " << args->c_str() << ")" << endl;
#endif
    if( args[0] != " " )
    {
        args->insert(0, " ");
    }
    if( !m_toolsActive )
    {
		bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::runProgramInVM() - Logging into guest..." << endl;
#endif
        login();
#ifdef DEBUG
        cout << " --- CVMInterface::runProgramInVM() - Logged in..." << endl;
#endif
    }
#ifdef DEBUG
    cout << " --- CVMInterface::runProgramInVM() - About to do work..." << endl;
#endif
    m_jobHandle = VixVM_RunProgramInGuest(m_vmHandle,
                                        program->c_str(),       // command
                                        args->c_str(),          // cmd args
                                        0,                      // options
                                        VIX_INVALID_HANDLE,     // prop handle
                                        NULL,                   // callback
                                        NULL);                  // client data
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to run program in virtual machine '%s'(%"FMT64"d %s)\n",
            m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
        result = 1;
    }
#ifdef DEBUG
    cout << " --- CVMInterface::runProgramInVM() - Work completed." << endl;
#endif
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::runProgramInVM() - Logging out of guest..." << endl;
#endif
        logout();
#ifdef DEBUG
        cout << " --- CVMInterface::runProgramInVM() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::waitForTools(program, args)
/// Waits for the VMware tools to respond from the guest OS.
/// </summary>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::waitForTools()
{
    int result = 0;
    VixError err;
#ifdef DEBUG
    cout << " --- CVMInterface::waitForTools() calling VixVM_WaitForToolsInGuest()..." << endl;
#endif
    m_jobHandle = VixVM_WaitForToolsInGuest(m_vmHandle,
                                          m_timeout,    // timeout in secs
                                          NULL,         // callback
                                          NULL);        // client data
#ifdef DEBUG
    cout << " --- CVMInterface::waitForTools() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
#ifdef DEBUG
    cout << " --- CVMInterface::waitForTools() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to wait for tools in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
#ifdef DEBUG
    cout << " -- tools up" << endl;
#endif
    m_toolsActive = true;
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::createFolderInVM()
/// Creates a folder in the guest.
/// </summary>
/// <param name="path">The desired folder path.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::createFolderInVM(string* path)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::createFolderInVM() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::createFolderInVM() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    m_jobHandle = VixVM_CreateDirectoryInGuest(m_vmHandle,
                                path->c_str(),
                                VIX_INVALID_HANDLE,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to create directory in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::createFolderInVM() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::createFolderInVM() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::removeFolderInVM()
/// Deletes a folder in the guest.
/// </summary>
/// <param name="path">The folder to delete.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::removeFolderInVM(string* path)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::removeFolderInVM() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::removeFolderInVM() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    m_jobHandle = VixVM_DeleteDirectoryInGuest(m_vmHandle,
                                path->c_str(),
                                VIX_INVALID_HANDLE,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to remove directory in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::removeFolderInVM() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::removeFolderInVM() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::addSharedFolder()
/// Adds a share between the host and guest.
/// </summary>
/// <param name="pathOnGuest">The pathname the guest will use.</param>
/// <param name="pathOnHost">The pathname the host will use.</param>
/// <param name="writeAccess">If true, the guest can write to the share.  Default true.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::addSharedFolder(string* pathOnGuest, string* pathOnHost, bool writeAccess)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::addSharedFolder() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::addSharedFolder() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    VixMsgSharedFolderOptions opt = 0;
    if( writeAccess )
        opt = VIX_SHAREDFOLDER_WRITE_ACCESS;
    m_jobHandle = VixVM_AddSharedFolder(m_vmHandle,
                                pathOnHost->c_str(),
                                pathOnGuest->c_str(),
                                opt,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to add shared folder in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::addSharedFolder() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::addSharedFolder() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::removeSharedFolder()
/// Removes an existing share between the host and guest.
/// </summary>
/// <param name="pathOnGuest">The guest path to remove.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::removeSharedFolder(string* pathOnGuest)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::removeSharedFolder() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::removeSharedFolder() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    m_jobHandle = VixVM_RemoveSharedFolder(m_vmHandle,
                                pathOnGuest->c_str(),
                                0,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to remove shared folder in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::removeSharedFolder() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::removeSharedFolder() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::copyFileToGuest()
/// Copys a file from the host to the guest.
/// </summary>
/// <param name="pathOnHost">The file to copy.</param>
/// <param name="pathOnGuest">The target file path.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::copyFileToGuest(string* pathOnHost, string* pathOnGuest)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToGuest() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToGuest() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    m_jobHandle = VixVM_CopyFileFromHostToGuest(m_vmHandle,
                                pathOnHost->c_str(),
                                pathOnGuest->c_str(),
                                0,
                                VIX_INVALID_HANDLE,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to copy file to guest (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToGuest() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToGuest() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::copyFileToHost()
/// Copys a file from the guest to the host.
/// </summary>
/// <param name="pathOnGuest">The file path to copy.</param>
/// <param name="pathOnHost">The target file path.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::copyFileToHost(string* pathOnGuest, string* pathOnHost)
{
    int result = 0;
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToHost() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToHost() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            result = 1;
            return result;
        }
    }
    m_jobHandle = VixVM_CopyFileFromGuestToHost(m_vmHandle,
                                pathOnGuest->c_str(),
                                pathOnHost->c_str(),
                                0,
                                VIX_INVALID_HANDLE,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to copy file to host (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        result = 1;
        return result;
    }
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToHost() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
            result = 1;
        }
#ifdef DEBUG
        cout << " --- CVMInterface::copyFileToHost() - Logged out of guest..." << endl;
#endif
    }
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::listDirectoryInGuest()
/// Gets the contents of the selected path on the guest.
/// </summary>
/// <param name="pathOnGuest">The desired target path.</param>
/// <param name="dirList">A pointer to a vector of string pointers - holds file list.</param>
/// <return>Returns a pointer to a vector of string pointers containing the file list.</return>
vector<string*>* CVMInterface::listDirectoryInGuest(string* pathOnGuest, vector<string*>* dirList)
{
    VixError err;
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - Logging into guest..." << endl;
#endif
        m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                       m_guestUser->c_str(),        // guest OS user
                                       m_guestPassword->c_str(),    // guest OS passwd
                                       0,                           // options
                                       NULL,                        // callback
                                       NULL);                       // client data
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
        }
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - Logged in..." << endl;
#endif
    }
    if( !m_toolsActive )
    {
        bool canRun = waitForTools() != 0;
        if( !canRun )
        {
            fprintf(stderr, " -- Tools failed to respond within %d seconds", m_timeout);
            return dirList;
        }
    }
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - listing files in " << pathOnGuest->c_str() << endl;
#endif
    m_jobHandle = VixVM_ListDirectoryInGuest(m_vmHandle,
                                pathOnGuest->c_str(),
                                0,
                                NULL,
                                NULL);    
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to list directory in virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        return dirList;
    }
    int count = VixJob_GetNumProperties(m_jobHandle, VIX_PROPERTY_JOB_RESULT_ITEM_NAME);
    for( int i = 0; i < count; i++ )
    {
        char* fname;
        err = VixJob_GetNthProperties(m_jobHandle,
                                    i, 
                                    VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
                                    &fname,
                                    VIX_PROPERTY_NONE);
        string* filename = new string(fname);
        dirList->push_back(filename);
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - found " << filename->c_str() << endl;
#endif
        Vix_FreeBuffer(fname);
    }
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to create file list from virtual machine (%"FMT64"d %s)\n", err,
            Vix_GetErrorText(err, NULL));
        return dirList;
    }
#ifdef DEBUG
    cout << " --- CVMInterface::listDirectoryInGuest() - found " << dirList->size() << " files in " << pathOnGuest->c_str() << endl;
    for( vector<string*>::iterator it = dirList->begin(); it != dirList->end(); it++ )
    {
        cout << " ---- : " << (char*)(*it)->c_str() << endl;
    }
#endif
    if( !m_loggedIn )
    {
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - Logging out of guest..." << endl;
#endif
        m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                            NULL,
                                            NULL);
        err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
        Vix_ReleaseHandle(m_jobHandle);
        if (VIX_FAILED(err)) {
            fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
                m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
        }
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - Logged out of guest..." << endl;
#endif
    }
#ifdef DEBUG
        cout << " --- CVMInterface::listDirectoryInGuest() - complete" << endl;
#endif
    return dirList;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::login()
/// Logs into the currently controlled VM.
/// Set isTransient to false if you wish to call several functions in the guest
/// in the span of a single login.
/// </summary>
/// <param name="isTransient">Logging in for a single function call.</param>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::login(bool isTransient)
{
#ifdef DEBUG
    cout << " --- CVMInterface::login()" << endl;
#endif
    // log into guest
    VixError err;
    int result = 0;
#ifdef DEBUG
    cout << " --- CVMInterface::login() calling VixVM_LoginInGuest()..." << endl;
#endif
    m_jobHandle = VixVM_LoginInGuest(m_vmHandle,
                                   m_guestUser->c_str(),        // guest OS user
                                   m_guestPassword->c_str(),    // guest OS passwd
                                   0,                           // options
                                   NULL,                        // callback
                                   NULL);                       // client data
#ifdef DEBUG
    cout << " --- CVMInterface::login() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
#ifdef DEBUG
    cout << " --- CVMInterface::login() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- failed to login to virtual machine '%s'(%"FMT64"d %s)\n", m_vmPath->c_str(), err,
                Vix_GetErrorText(err, NULL));
        m_loggedIn = false;
        result = 1;
    }
#ifdef DEBUG
    cout << " -- logged in to guest" << endl;
#endif
    if( !isTransient )
        m_loggedIn = true;
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::logout()
/// Logs out of the currently controlled VM.
/// </summary>
/// <return>Returns 0 if successful, 1 otherwise.</return>
int CVMInterface::logout()
{
#ifdef DEBUG
    cout << " --- CVMInterface::logout()" << endl;
#endif
    // log out of guest
    int result = 0;
    VixError err;
#ifdef DEBUG
    cout << " --- CVMInterface::logout() calling VixVM_LogoutFromGuest()..." << endl;
#endif
    m_jobHandle = VixVM_LogoutFromGuest(m_vmHandle,
                                        NULL,
                                        NULL);
#ifdef DEBUG
    cout << " --- CVMInterface::logout() calling VixJob_Wait()..." << endl;
#endif
    err = VixJob_Wait(m_jobHandle, VIX_PROPERTY_NONE);
#ifdef DEBUG
    cout << " --- CVMInterface::logout() calling Vix_ReleaseHandle()..." << endl;
#endif
    Vix_ReleaseHandle(m_jobHandle);
    if (VIX_FAILED(err)) {
        fprintf(stderr, " -- Error logging out from virtual machine '%s'(%"FMT64"d %s)\n",
            m_vmPath->c_str(), err, Vix_GetErrorText(err, NULL));
        result = 1;
    }
    m_loggedIn = false;
#ifdef DEBUG
    cout << " -- logged out of guest" << endl;
#endif
    return result;
}

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::setTimeout(timout)
/// Sets the timeout for guest operations.
/// </summary>
/// <param name="timout">
/// <return>Returns 0 if successful, 1 otherwise.</return>
bool CVMInterface::setTimeout(int timeout)
{
#ifdef DEBUG
    cout << " --- CVMInterface::setTimeout(" << timeout << ")..." << endl;
#endif
    if( timeout > 0 && timeout <= 3600 ) // longer than zero seconds, less than an hour
    {
        m_timeout = timeout;
        TOOLS_TIMEOUT = m_timeout;
#ifdef DEBUG
        cout << " --- CVMInterface::setTimeout() - timeout set to " << timeout << " seconds." << endl;
#endif
        return true;
    }
#ifdef DEBUG
    cout << " --- CVMInterface::setTimeout() - failed; timeout value out of range (1 - 3600)." << endl;
#endif
    return false;
}


// ----------------------------------------------------------------------------
// Lua interface

// ----------------------------------------------------------------------------
/// <summary>
/// CVMInterface::L_startVM(lua_State)
/// Sets the timeout for guest operations.
/// </summary>
int CVMInterface::L_startVM(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_startVM()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm path
    if (lua_isstring(L, top))
    {
        if( m_vmPath != NULL )
            delete(m_vmPath);
        m_vmPath = new string(lua_tostring(L, top));
    }
    else
    {
        lua_pushnil(L);
        return 0;
    }
    result = startVM();
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_stopVM(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_stopVM()" << endl;
#endif
    int result = 0;
    result = stopVM();
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_runScriptInVM(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_runScriptInVM()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L);
    string* interpreter = new string(lua_tostring(L, 1));
    string* scriptText = new string(lua_tostring(L, 2));
#ifdef DEBUG
    cout << " --- CVMInterface::L_runScriptInVM() - Lua stack depth is " << top << endl;
    cout << " --- CVMInterface::L_runScriptInVM() - interpreter is " << interpreter->c_str() << endl;
    cout << " --- CVMInterface::L_runScriptInVM() - script text is " << scriptText->c_str() << endl;
#endif
    result = runScriptInVM(interpreter, scriptText);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_runProgramInVM(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_runProgramInVM()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L);
    string* program;
    string* args;
    if (lua_isstring(L, 1))
    {
        program = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, 2))
    {
        args = new string(lua_tostring(L, 2));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = runProgramInVM(program, args);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_createFolderInVM(lua_State* L)
{
    int result = 0;
    string* path;
    if (lua_isstring(L, 1))
    {
        path = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = createFolderInVM(path);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_removeFolderInVM(lua_State* L)
{
    int result = 0;
    string* path;
    if (lua_isstring(L, 1))
    {
        path = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = removeFolderInVM(path);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_addSharedFolder(lua_State* L)
{
    int result = 0;
    string* guestPath;
    string* hostPath;
    bool allowWrite;
    if (lua_isstring(L, 1))
    {
        guestPath = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, 2))
    {
        hostPath = new string(lua_tostring(L, 2));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isboolean(L, 3))
    {
        allowWrite = lua_toboolean(L, 3) != 0;
    }
    else
    {
        allowWrite = true;
    }
    result = addSharedFolder(guestPath, hostPath, allowWrite);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_removeSharedFolder(lua_State* L)
{
    int result = 0;
    string* path;
    if (lua_isstring(L, 1))
    {
        path = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = removeSharedFolder(path);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_copyFileToGuest(lua_State* L)
{
    int result = 0;
    string* guestPath;
    string* hostPath;
    if (lua_isstring(L, 1))
    {
        hostPath = new string(lua_tostring(L, 2));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, 2))
    {
        guestPath = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = copyFileToGuest(hostPath, guestPath);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_copyFileToHost(lua_State* L)
{
    int result = 0;
    string* guestPath;
    string* hostPath;

	if (lua_isstring(L, 1))
    {
        guestPath = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    if (lua_isstring(L, 2))
    {
        hostPath = new string(lua_tostring(L, 2));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
    result = copyFileToHost(guestPath, hostPath);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_listDirectoryInGuest(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_listDirectoryInGuest()" << endl;
#endif
    int result = 0;
    string* guestPath;
    if (lua_isstring(L, 1))
    {
        guestPath = new string(lua_tostring(L, 1));
    }
    else
    {
        lua_pushnil(L);
        return 1;
    }
#ifdef DEBUG
    cout << " --- CVMInterface::L_listDirectoryInGuest() - listing files in " << guestPath->c_str() << endl;
#endif
    vector<string*> *fileList = new vector<string*>();
    listDirectoryInGuest(guestPath, fileList);
#ifdef DEBUG
    cout << " --- CVMInterface::L_listDirectoryInGuest() - found " << fileList->size() << " files in " << guestPath->c_str() << endl;
    for( vector<string*>::iterator it = fileList->begin(); it != fileList->end(); it++ )
    {
        cout << " ---- : " << (char*)(*it)->c_str() << endl;
    }
#endif
    lua_newtable(L);
    int i = 0;
    for (vector<string*>::iterator it = fileList->begin(); it != fileList->end(); it++)
    {
            cout << " ** CVMInterface::L_listDirectoryInGuest() : " << (char*)(*it)->c_str() << endl;
            lua_pushnumber(L, i++);
            lua_pushstring(L, (char*)(*it)->c_str());
            lua_settable(L, -3);
    }
#ifdef DEBUG
    cout << " --- CVMInterface::L_listDirectoryInGuest() - complete" << endl;
#endif
    return 1;
}

int CVMInterface::L_setTimeout(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_setTimeout()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm timeout in seconds
    int seconds = 0;
    if (lua_isnumber(L, top))
    {
        seconds = lua_tointeger(L, top);
        if( seconds > 0 && seconds <= 3600 )
            setTimeout(seconds);
        else
            result = 1;
    }
    else
    {
        result = 1;
    }
    lua_pushnumber(L, result);
    return 1;
#ifdef DEBUG
    cout << " --- CVMInterface::L_setTimeout() - timeout set to " << seconds << endl;
#endif
}

int CVMInterface::L_setUserName(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_setUserName()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm path
    if (lua_isstring(L, top))
    {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setUserName(): lua passed in string...." << endl;
#endif
        if( m_guestUser != NULL )
        {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setUserName(): removing existing username...." << endl;
#endif
            delete(m_guestUser);
        }
        m_guestUser = new string((char*)lua_tostring(L, top));
#ifdef DEBUG
    cout << " --- CVMInterface::L_setUserName(): username set to " << *(m_guestUser) << endl;
#endif
    }
    else
    {
        result = 1;
    }
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_setPassword(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_setPassword()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm path
    if (lua_isstring(L, top))
    {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setPassword(): lua passed in string...." << endl;
#endif
        if( m_guestPassword != NULL )
        {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setPassword(): removing existing password...." << endl;
#endif
            delete(m_guestPassword);
        }
        m_guestPassword = new string((char*)lua_tostring(L, top));
#ifdef DEBUG
    cout << " --- CVMInterface::L_setPassword(): password set to " << *(m_guestPassword) << endl;
#endif
    }
    else
    {
        result = 1;
    }
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_setHostUserName(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostUserName()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm path
    if (lua_isstring(L, top))
    {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostUserName(): lua passed in string...." << endl;
#endif
        if( m_hostUser != NULL )
        {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostUserName(): removing existing host username...." << endl;
#endif
            delete(m_hostUser);
        }
        m_hostUser = new string((char*)lua_tostring(L, top));
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostUserName(): host username set to " << *(m_hostUser) << endl;
#endif
    }
    else
    {
        result = 1;
    }
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_setHostPassword(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostPassword()" << endl;
#endif
    int result = 0;
    int top = lua_gettop(L); // should be vm path
    if (lua_isstring(L, top))
    {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostPassword(): lua passed in string...." << endl;
#endif
        if( m_hostPassword != NULL )
        {
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostPassword(): removing existing host password...." << endl;
#endif
            delete(m_hostPassword);
        }
        m_hostPassword = new string((char*)lua_tostring(L, top));
#ifdef DEBUG
    cout << " --- CVMInterface::L_setHostPassword(): host password set to " << *(m_hostPassword) << endl;
#endif
    }
    else
    {
        result = 1;
    }
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_login(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_login()" << endl;
#endif
    int result = 0;
    result = login(false);
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_logout(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_logout()" << endl;
#endif
    int result = 0;
    result = logout();
    lua_pushnumber(L, result);
    return 1;
}

int CVMInterface::L_waitForTools(lua_State* L)
{
#ifdef DEBUG
    cout << " --- CVMInterface::L_waitForTools()" << endl;
#endif
    int result = 0;
    result = waitForTools();
    lua_pushnumber(L, result);
    return 1;
}

const char *CVMInterface::className = "CVMInterface";
const Luna < CVMInterface >::FunctionType CVMInterface::methods[] = {
	{ "startVM", &CVMInterface::L_startVM },
	{ "stopVM", &CVMInterface::L_stopVM },
	{ "waitForTools", &CVMInterface::L_waitForTools },
	{ "runScriptInVM", &CVMInterface::L_runScriptInVM },
	{ "runProgramInVM", &CVMInterface::L_runProgramInVM },
	{ "createFolderInVM", &CVMInterface::L_createFolderInVM },
	{ "removeFolderInVM", &CVMInterface::L_removeFolderInVM },
	{ "addSharedFolder", &CVMInterface::L_addSharedFolder },
	{ "removeSharedFolder", &CVMInterface::L_removeSharedFolder },
	{ "copyFileToGuest", &CVMInterface::L_copyFileToGuest },
	{ "copyFileToHost", &CVMInterface::L_copyFileToHost },
	{ "listDirectoryInGuest", &CVMInterface::L_listDirectoryInGuest },
	{ "setTimeout", &CVMInterface::L_setTimeout },
	{ "setUserName", &CVMInterface::L_setUserName },
	{ "setPassword", &CVMInterface::L_setPassword },
	{ "setHostUserName", &CVMInterface::L_setHostUserName },
	{ "setHostPassword", &CVMInterface::L_setHostPassword },
	{ "login", &CVMInterface::L_login },
	{ "logout", &CVMInterface::L_logout },
	{ 0 }
};
const Luna < CVMInterface >::PropertyType CVMInterface::properties[] = {
	{ 0 }
};
