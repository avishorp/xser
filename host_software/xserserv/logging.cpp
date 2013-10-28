#include "stdafx.h"
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/event_log_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

using namespace std;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

// Interal Forwards
VOID SetupBasicLogging();
VOID SetupAdvancedLogging();

// The registry key defining advanced logging
WCHAR DebugLogRegKey[] = TEXT("SOFTWARE\\XSer\\");

// Complete sink type
typedef sinks::synchronous_sink< sinks::simple_event_log_backend > sink_t;

// Define application-specific severity levels
enum severity_level
{
    normal,
    warning,
    error
};


VOID SetupLogging()
{
	SetupBasicLogging();
	SetupAdvancedLogging();
}

// Basic logging - log warnings and messages to the Windows
//                 evento log
VOID SetupBasicLogging()
{
    // Create an event log sink
    boost::shared_ptr< sink_t > sink(new sink_t());

//    sink->set_formatter
//    (
//        expr::format("%1%: [%2%] - %3%")
//            % expr::attr< unsigned int >("LineID")
//            % expr::attr< boost::posix_time::ptime >("TimeStamp")
//            % expr::smessage
//    );

    // We'll have to map our custom levels to the event log event types
    sinks::event_log::custom_event_type_mapping< severity_level > mapping("Severity");
    mapping[normal] = sinks::event_log::info;
    mapping[warning] = sinks::event_log::warning;
    mapping[error] = sinks::event_log::error;

    sink->locked_backend()->set_event_type_mapper(mapping);

    // Add the sink to the core
    logging::core::get()->add_sink(sink);

	// The basic logger logs only errors and warnings
	sink->set_filter(
        logging::trivial::severity >= logging::trivial::warning
	);
}

VOID SetupAdvancedLogging()
{
	// Check if the registry key enabling advanced logging
	// exists, and retrieve it's value (log filename)
	DWORD dwType;
	char szLogFilename[256];
	DWORD dwDataSize = sizeof(szLogFilename) - 1;
	HKEY hSoftwareKey;
	HKEY hXSerKey;
	LONG r;

	// Create a key in the registry (if not already exists)                   
	r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE",  0, KEY_READ, &hSoftwareKey);
	if (r != ERROR_SUCCESS) {
		BOOST_LOG_TRIVIAL(warning) << "Could not open the registry SOFTWARE key";
		return;
	}

//	r = RegCreateKeyEx(hSoftwareKey, L"XSer", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hXSerKey, NULL);
//	if (r != ERROR_SUCCESS) {
//		BOOST_LOG_TRIVIAL(warning) << "Could not create or access the HKLM\SOFTWARE\XSer Key";
//		return;
//	}

	r = RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\XSer", &hXSerKey);

	r = RegQueryValueEx(hXSerKey, TEXT("DebugLog"), NULL, &dwType, (BYTE*)szLogFilename, &dwDataSize);
	if (r != ERROR_SUCCESS)
		// The key does not exist, no debug log is requested
		return;

	wstring LogFilenameLong((LPCWCHAR)szLogFilename);
	string LogFilename(LogFilenameLong.begin(), LogFilenameLong.end());

    boost::shared_ptr< sinks::text_file_backend > backend =
        boost::make_shared< sinks::text_file_backend >(
		keywords::file_name = LogFilename.c_str());

    // Wrap it into the frontend and register in the core.
    // The backend requires synchronization in the frontend.
    typedef sinks::synchronous_sink< sinks::text_file_backend > sink_t;
    boost::shared_ptr< sink_t > sink(new sink_t(backend));

	boost::shared_ptr< logging::core > core = logging::core::get();
    core->add_sink(sink);
}



