#include "stdafx.h"
#include "xserserv.h"
#include <xser.h>
#include <exception>
#include <stdlib.h>
#include <memory.h>
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace xser;
using namespace std;

#pragma comment(lib, "advapi32.lib")

#define SVCNAME TEXT("XSerService")

SERVICE_STATUS          gSvcStatus; 
SERVICE_STATUS_HANDLE   gSvcStatusHandle; 
HANDLE                  ghSvcStopEvent = NULL;
HDEVNOTIFY              gDeviceNotify = NULL;
xser_instance_manager_ifx* gInstMan;
	


VOID SvcInstall(void);
VOID SvcUninstall(void);

DWORD WINAPI SvcCtrlHandler (DWORD, DWORD, LPVOID, LPVOID);
VOID WINAPI SvcMain( DWORD, LPTSTR * ); 

VOID ReportSvcStatus( DWORD, DWORD, DWORD );
VOID SvcInit( DWORD, LPTSTR * );

VOID SvcReportEvent( LPTSTR, WORD, DWORD );
VOID SvcReportError( LPTSTR, DWORD );
VOID SvcReportWarning( LPTSTR, DWORD );
VOID SvcReportInfo( LPTSTR, DWORD );
VOID SvcReportDebug( LPTSTR, DWORD );

//
// Purpose: 
//   Entry point for the process
//
// Parameters:
//   None
// 
// Return value:
//   None
//
void __cdecl _tmain(int argc, TCHAR *argv[]) 
{ 
	SetupLogging();

	if( lstrcmpi( argv[1], TEXT("install")) == 0 )
    {
        SvcInstall();
        return;
    }
	else if (lstrcmpi(argv[1], TEXT("uninstall")) == 0)
	{
		SvcUninstall();
		return;
	}
	else if (lstrcmpi(argv[1], TEXT("list")) == 0)
	{
	}
	else if (lstrcmpi(argv[1], TEXT("refresh")) == 0)
	{
	}

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY DispatchTable[] = 
    { 
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain }, 
        { NULL, NULL } 
    }; 
 
    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.

    if (!StartServiceCtrlDispatcher( DispatchTable )) 
    { 
        BOOST_LOG_TRIVIAL(error) << "StartServiceCtrlDispatcher"; 
    } 
} 

//
// Purpose: 
//   Installs a service in the SCM database
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

	// logging::add_file_log("sample.log");

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service

    schService = CreateService( 
        schSCManager,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_AUTO_START,        // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
 
    if (schService == NULL) 
    {
        printf("CreateService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }
    else {
		printf("Service installed successfully\n"); 
		if (StartService(schService, NULL, NULL)) {
			printf("Service started\n");
		}
	}

	
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
}

VOID SvcUninstall(void)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus; 

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the service.

    schService = OpenService( 
        schSCManager,       // SCM database 
        SVCNAME,          // name of service 
        SERVICE_ALL_ACCESS);  // need delete access 
 
    if (schService == NULL)
    { 
        printf("OpenService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }

	// Check if the service is started
	SERVICE_STATUS stat;
	if (!QueryServiceStatus(schService, &stat)) {
		printf("QueryServiceStatus failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
	}

	if (stat.dwCurrentState != SERVICE_STOPPED) {
		// The service is running, stop it
		SERVICE_CONTROL_STATUS_REASON_PARAMS reason;
		memset(&reason, 0, sizeof(SERVICE_CONTROL_STATUS_REASON_PARAMS));
		reason.dwReason = SERVICE_STOP_REASON_FLAG_PLANNED|SERVICE_STOP_REASON_MAJOR_OTHER|SERVICE_STOP_REASON_MINOR_INSTALLATION;

		if (!ControlServiceEx(schService, SERVICE_CONTROL_STOP, SERVICE_CONTROL_STATUS_REASON_INFO, &reason)) {
			printf("ControlServiceEx failed (%d)\n", GetLastError());
		    CloseServiceHandle(schSCManager);
			return;
		}
	}

    // Delete the service.
 
    if (! DeleteService(schService) ) 
    {
        printf("DeleteService failed (%d)\n", GetLastError()); 
    }
    else printf("Service deleted successfully\n"); 
 
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
}


//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
	BOOST_LOG_TRIVIAL(info) << "xserserv Started";

	
	// Register the handler function for the service

    gSvcStatusHandle = RegisterServiceCtrlHandlerEx( 
        SVCNAME, 
        SvcCtrlHandler, NULL);

    if( !gSvcStatusHandle )
    { 
        BOOST_LOG_TRIVIAL(error) << "RegisterServiceCtrlHandler"; 
        return; 
    } 

    // These SERVICE_STATUS members remain as set here

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
    gSvcStatus.dwServiceSpecificExitCode = 0;    

    // Report initial status to the SCM

    ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

    // Perform service-specific initialization and work.

    SvcInit( dwArgc, lpszArgv );
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{
	BOOST_LOG_TRIVIAL(debug) << "SvcInit Entered";

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.

    ghSvcStopEvent = CreateEvent(
                         NULL,    // default security attributes
                         TRUE,    // manual reset event
                         FALSE,   // not signaled
                         NULL);   // no name

    if ( ghSvcStopEvent == NULL)
    {
		BOOST_LOG_TRIVIAL(error) << "CreateEvent";
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        return;
    }

	BOOST_LOG_TRIVIAL(debug) << "CreateEvent succeeded";

	// Register to accept device change notification
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_HID;//GUID_DEVCLASS_PORTS;//GUID_DEVCLASS_HIDCLASS;

	gDeviceNotify = RegisterDeviceNotification(gSvcStatusHandle, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);
	if (gDeviceNotify == NULL) {
		BOOST_LOG_TRIVIAL(error) << "RegisterDeviceNotification";
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
	}
	BOOST_LOG_TRIVIAL(debug) << "Device Notification Registered";

	try {
		gInstMan = &get_xser_instance_manager();
	}
	catch(runtime_error& e) {
		BOOST_LOG_TRIVIAL(error) << "Could not get instace manager: " << e.what();
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
	}

    // Report running status when initialization is complete.
    ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );

    while(1)
    {
        // Check whether to stop the service.

        WaitForSingleObject(ghSvcStopEvent, INFINITE);
		BOOST_LOG_TRIVIAL(debug) << "Wait on stop event returned";

        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        break;
    }

	// Unregister
	if (gDeviceNotify != NULL)
		UnregisterDeviceNotification(gDeviceNotify);
	BOOST_LOG_TRIVIAL(debug) << "DeviceNotification cleaned up";
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
DWORD WINAPI SvcCtrlHandler(DWORD dwCtrl,
  DWORD dwEventType,
  LPVOID lpEventData,
  LPVOID lpContext )
{
   // Handle the requested control code. 

   switch(dwCtrl) 
   {  
      case SERVICE_CONTROL_STOP: 
         ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

         // Signal the service to stop.

         SetEvent(ghSvcStopEvent);
         ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
         
         return NO_ERROR;
 
      case SERVICE_CONTROL_INTERROGATE: 
         break; 

	  case SERVICE_CONTROL_DEVICEEVENT:
		  BOOST_LOG_TRIVIAL(debug) << "SERVICE_CONTROL_DEVICEEVENT";

		  if (dwEventType == DBT_DEVICEARRIVAL) {
			  BOOST_LOG_TRIVIAL(debug) << "Device Arrival";

			  try {
				gInstMan->rescan();
				gInstMan->update_all_adaptors();
			  }
			  catch(runtime_error& e) {
				  BOOST_LOG_TRIVIAL(error) << e.what();
			  }
		  }
		  break;
 
      default: 
         break;
   } 

   return NO_ERROR;
   
}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction, WORD type, DWORD id) 
{ 
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, SVCNAME);

    if( NULL != hEventSource )
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
                    type,                // event type
                    0,                   // event category
                    id,                  // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}

VOID SvcReportError(LPTSTR message, DWORD id)
{
	SvcReportEvent(message, EVENTLOG_ERROR_TYPE, id);
}

VOID SvcReportWarning(LPTSTR message, DWORD id)
{
	SvcReportEvent(message, EVENTLOG_WARNING_TYPE, id);
}

VOID SvcReportInfo(LPTSTR message, DWORD id)
{
	SvcReportEvent(message, EVENTLOG_INFORMATION_TYPE, id);
}

VOID SvcReportDebug(LPTSTR message, DWORD id)
{
#ifdef _DEBUG
	SvcReportEvent(message, EVENTLOG_INFORMATION_TYPE, id);
#endif // DEBUG
}

