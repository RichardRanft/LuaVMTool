/* 
 * File:   main.cpp
 * Author: sgp1000
 *
 * Created on June 27, 2014, 11:24 AM
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef WINDOWS
extern "C"{
#include <dlfcn.h>
}
#else
#include <Windows.h>
#define __forceCRTManifestCUR 0;
#endif

#include <cstdlib>
#include <vix.h>
#include "luna.h"
#include "CApplication.h"

using namespace std;

CApplication *app;
void* vixHandle;
void* gvmomiHandle;
int TOOLS_TIMEOUT = 300;

/*
 * 
 */

#ifndef WINDOWS
void* openLib(char* libName)
{
#ifdef DEBUG
    cout << " -- loading " << libName << endl;
#endif
    void *handle;
    char *error;
    handle = dlopen (libName, RTLD_LAZY);
    if (!handle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();    /* Clear any existing error */
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }
#ifdef DEBUG
    cout << " -- " << libName << " loaded" << endl;
#endif
    return handle;
}
#endif

int main(int argc, char** argv) {
    int result = 1;
#ifndef WINDOWS
    gvmomiHandle = openLib("/usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libgvmomi-vix-1.13.2.so.0");
    vixHandle = openLib("/usr/lib/vmware-vix/Workstation-10.0.0-and-vSphere-5.5.0/32bit/libvix.so");
#else
	#ifdef DEBUG
		LoadLibrary("C:\\Program Files (x86)\\VMware\\VMware VIX\\Workstation-10.0.0-and-vSphere-5.5.0\\64bit\\vixd.dll");
	#else
		LoadLibrary("C:\\Program Files (x86)\\VMware\\VMware VIX\\Workstation-10.0.0-and-vSphere-5.5.0\\64bit\\vix.dll");
	#endif
#endif

    app = new CApplication(argc, argv);

    result = app->run();

    delete(app);
    cout << " -- vmtool exit" << endl;

#ifndef WINDOWS
	dlclose(vixHandle);
    dlclose(gvmomiHandle);
#endif

    return result;
}

