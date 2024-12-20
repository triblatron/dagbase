#ifndef __ERROR_HANDLER_H__
#define __ERROR_HANDLER_H__

#include "config/DagBaseExport.h"

#include "util/errod.h"

#include <mutex>

namespace dagbase
{
	//! Interface for error handlers.
	// class DAGBASE_API ErrorHandler
	// {
	// public:
	// 	ErrorHandler() { }
	// 	virtual ~ErrorHandler() { }
	// 	virtual void raiseError( int code, const char * format, ... ) = 0;
	// 	virtual void reportInfo(const char * format, ... ) = 0;
	// 	virtual bool ok() const = 0;
	// };
	//
	// //! Default implementation of ErrorHandler that prints errors to the console.
	// struct DAGBASE_API DefaultErrorHandler : public ErrorHandler
	// {
	// 	DefaultErrorHandler()
	// 	:
	// 	_errod(NoError),
	// 	_errorMessage(""),
	// 	_infoMessage("")
	// 	{
	// 	}
 //
	// 	virtual void raiseError( int code, const char * format, ... );
	// 	virtual void reportInfo(const char * format, ... );
 //
	// 	virtual bool ok() const
	// 	{
	// 		return _errod == NoError;
	// 	}
 //
	// 	virtual void resetError()
	// 	{
	// 		_errod=NoError;
	// 		_errorMessage = "";
	// 		_infoMessage = "";
	// 	}
 //
	// 	int					_errod;
	// 	std::string			_errorMessage;
	// 	std::string			_infoMessage;
 //        static std::mutex _mut;
	// };
}

#endif // !__ERROR_HANDLER_H__
