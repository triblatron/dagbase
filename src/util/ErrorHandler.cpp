#include "config/config.h"

#include "util/ErrorHandler.h"

#include <iostream>
#include <cstdarg>

namespace dagbase
{
 //    std::mutex DefaultErrorHandler::_mut;
 //
	// void
	// DefaultErrorHandler::raiseError( int code, const char * format, ... )
	// {
	// 	_errod = code;
	// 	std::ostringstream errorStr;
	// 	char buf[256];
 //
	// 	va_list args;
 //
	// 	va_start( args, format );
	// 		vsnprintf( buf, 255, format, args );
	// 	va_end(args);
	// 	errorStr << "Error " << code << " ( " << ErrorCodeToName(code) << " ) Details: " << buf;
	// 	_errorMessage = errorStr.str();
 //
 //        {
 //            std::scoped_lock<std::mutex> guard(_mut);
 //
 //            std::cerr << _errorMessage << '\n';
 //        }
	// }
	//
	// void
	// DefaultErrorHandler::reportInfo(const char * format, ... )
	// {
	// 	std::ostringstream infoStr;
	// 	char buf[256];
 //
	// 	va_list args;
 //
	// 	va_start( args, format );
	// 		vsnprintf( buf, 255, format, args );
	// 	va_end(args);
	// 	infoStr << " Info  " << buf;
	// 	_infoMessage = infoStr.str();
 //
 //        {
 //            std::scoped_lock<std::mutex> guard(_mut);
 //
 //            std::cout << _infoMessage << std::endl;
 //        }
	// }
}
