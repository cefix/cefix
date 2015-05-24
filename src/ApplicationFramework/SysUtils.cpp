/*
 *  SysUtils.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 05.08.10.
 *  Copyright 2010 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SysUtils.h"
#include <cefix/Log.h>
#include <cmath>

#ifdef __APPLE__
	#include <mach/mach.h>
	#include <mach/mach_host.h>
#endif

namespace cefix {


#ifdef __APPLE__

bool SysUtils::getTaskMemoryUsage(unsigned long &used_virtual_mem, unsigned long &used_resident_mem)
{
	task_basic_info info;
	mach_msg_type_number_t size = sizeof( info );
	kern_return_t kerr = task_info( mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size );
	if ( kerr == KERN_SUCCESS ) {
		used_virtual_mem = info.virtual_size;
		used_resident_mem = info.resident_size;
		return true;
	}
	else {
		return false;
	}
}

bool SysUtils::getSystemMemoryUsage(unsigned long &free_mem, unsigned long &used_mem, unsigned long &mem_size)
{
	mach_port_t host_port;
	mach_msg_type_number_t host_size;
	vm_size_t pagesize;
	
	host_port = mach_host_self();
	host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
	host_page_size(host_port, &pagesize);
	
	vm_statistics_data_t vm_stat;
	
	if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS)
		return false;
	
	/* Stats in bytes */
	natural_t mem_used = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * pagesize;
	natural_t mem_free = vm_stat.free_count * pagesize;
	natural_t mem_total = mem_used + mem_free;
	used_mem = round(mem_used);
	free_mem = round(mem_free);
	mem_size = round(mem_total);
	return true;
}
#else

bool SysUtils::getSystemMemoryUsage(unsigned long &free_mem, unsigned long &used_mem, unsigned long &mem_size)
{
	return false;
}

bool SysUtils::getTaskMemoryUsage(unsigned long &used_virtual_mem, unsigned long &used_resident_mem)
{
	return false;
}

#endif

#ifdef CEFIX_FOR_IPHONE


#elif __APPLE__
		
#include <Carbon/Carbon.h>

OSStatus SendAppleEventToSystemProcess(AEEventID EventToSend)
{
    AEAddressDesc targetDesc;
    static const ProcessSerialNumber
         kPSNOfSystemProcess = { 0, kSystemProcess };
    AppleEvent eventReply = {typeNull, NULL};
    AppleEvent appleEventToSend = {typeNull, NULL};

    OSStatus error = noErr;

    error = AECreateDesc(typeProcessSerialNumber,
        &kPSNOfSystemProcess, sizeof(kPSNOfSystemProcess),
        &targetDesc);

    if (error != noErr)
    {
        return(error);
    }

    error = AECreateAppleEvent(kCoreEventClass, EventToSend,
                     &targetDesc, kAutoGenerateReturnID,
                     kAnyTransactionID, &appleEventToSend);

    AEDisposeDesc(&targetDesc);

    if (error != noErr)
    {
        return(error);
    }

    error = AESend(&appleEventToSend, &eventReply, kAENoReply,
             kAENormalPriority, kAEDefaultTimeout,
             NULL, NULL);

    AEDisposeDesc(&appleEventToSend);

    if (error != noErr)
    {
        return(error);
    }

    AEDisposeDesc(&eventReply);

    return(error); //if this is noErr then we are successful
}
#elif WIN32

#include "windows.h"

bool win_reboot_or_shutdown(bool do_shutdown) 
{
	HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 
 
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{// open and check the privileges for to perform the actions

        LookupPrivilegeValue(    NULL,  SE_SHUTDOWN_NAME,  &tkp.Privileges[0].Luid); 
         
        tkp.PrivilegeCount = 1; 
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        if(AdjustTokenPrivileges(hToken,  FALSE,  & tkp,  0,  (PTOKEN_PRIVILEGES)NULL, 0))
			if (ExitWindowsEx(do_shutdown ? (EWX_POWEROFF| EWX_FORCE) : (EWX_REBOOT| EWX_FORCE),0)) {
				return true;
			}
	}
	return false;
}
#endif



bool SysUtils::shutdown() {
#ifdef __APPLE__
	#ifdef CEFIX_FOR_IPHONE
		return false;
	#else
		OSErr error = SendAppleEventToSystemProcess(kAEShutDown);
		return (error == noErr);
	#endif
#else
	return win_reboot_or_shutdown(true);
#endif
}

bool SysUtils::sleep() {
#ifdef __APPLE__
	#ifdef CEFIX_FOR_IPHONE
		return false;
	#else
		OSErr error = SendAppleEventToSystemProcess(kAESleep);
		return (error == noErr);
	#endif
#else
	log::error("SysUtils") << "sleep not implemented for this platform! " << std::endl;
	return false;
#endif
}



bool SysUtils::reboot() {
#ifdef __APPLE__
	#ifdef CEFIX_FOR_IPHONE
		return false;
	#else
		OSErr error = SendAppleEventToSystemProcess(kAERestart);
		return (error == noErr);
	#endif
#else
	return win_reboot_or_shutdown(false);

#endif
}

#ifdef WIN32 
bool SysUtils::isWindows() { return true; }
bool SysUtils::isMac() { return false; }
bool SysUtils::isIOS() { return false; }
bool SysUtils::isIphone() { return false; }
bool SysUtils::isIpad() { return false; }
float getUIScale(const osg::GraphicsContext::ScreenIdentifier& si) { return 1.0f; }
#endif

}