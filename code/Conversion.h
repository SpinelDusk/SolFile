/// \file
/// \brief Conversions of types (Header)
/// \author SpinelDusk

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef ConversionH
#define ConversionH

#include <string>
#include <sstream>
#include <vcl.h>
#pragma hdrstop//This directive terminates the list of header files eligible for precompilation (Using ONLY for <standard> headers, no "custom")

namespace cnv
{
	/// \brief Abbreviated mnemonic for UnicodeString
	typedef UnicodeString       unistr;
	/// \brief Byte implementation
	typedef unsigned char		byte;
	/// \brief Word implementation (2 bytes)
	typedef unsigned short      word;
	/// \brief Dword implementation (4 bytes)
	typedef unsigned long       dword;
	/// \brief Unsigned alias for int
	typedef unsigned int		unsint;
}

char*			STRINGtoCHAR			(std::string);
std::string		USTRINGtoSTRING			(cnv::unistr);
std::string		WCHARTtoSTRING			(wchar_t*);
const char* 	STRINGtoCONSTCHAR		(std::string);
char* 			INTtoCHAR				(int);
std::string		INTtoSTRING				(int);
cnv::unistr		CHARtoUSTRING       	(char*);
const wchar_t* 	CONSTCHARtoCONSTWCHART	(const char*);
const wchar_t* 	STRINGtoCONSTWCHART		(std::string);
char* 			USTRINGtoCHAR			(cnv::unistr);
cnv::unistr		STRINGtoUSTRING         (std::string);
cnv::unistr		INTtoUSTRING         	(int);
#endif
