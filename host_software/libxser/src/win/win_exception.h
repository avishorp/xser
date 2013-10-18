#ifndef __WIN_EXCEPTION_H__
#define __WIN_EXCEPTION_H__

#define WIN_API_THROW(message)                        \
	stringstream serr;                                 \
	serr << message << " (" << GetLastError() << ")";  \
	throw runtime_error(serr.str())


#endif