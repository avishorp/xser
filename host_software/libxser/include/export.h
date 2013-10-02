#ifndef __EXPORT_H__
#define __EXPORT_H__

#if (defined(_WIN32) || defined(_WIN64)) && defined(_MSC_VER)

// Windows build
#define EXPORT __declspec(dllexport)

#else

// Linux build
#define EXPORT

#endif

#endif
