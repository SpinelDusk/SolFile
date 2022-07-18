/// \file
/// \brief Common hidden features (Header)
/// \author SpinelDusk

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef CHFH
#define CHFH

#include <stdlib.h> 	// _splitpath
#include <string> 		// strdup
#include <io.h>         // filelength
#include <fstream>
#pragma hdrstop//This directive terminates the list of header files eligible for precompilation (Using ONLY for <standard> headers, no "custom")

#include "Conversion.h"

namespace chf
{
	void		SplitPath(char*, char**, char**, char**, char**);
	void		SplitPath(std::string, std::string&, std::string&, std::string&, std::string&);
	cnv::unsint	SizeOfVector(char*);
	cnv::unsint	SizeOfVector(const char*);
	cnv::unsint	SizeOfArray(const char[]);
	bool		CreateDirectory(char*, cnv::unsint);
	bool		CreateDirectory(std::string);
	cnv::unistr	GetExecutableDirectory();
	cnv::unistr	GetFileName(cnv::unistr);
	cnv::unistr	GetFileExtension(cnv::unistr);
	cnv::dword	GetFileSize(FILE*);
	cnv::dword	GetFileSize(ifstream&);
    cnv::dword	GetFileSize(std::string);
	char*		GetFullPath(char*);
	bool		IsDirectoryExists(const wchar_t*);
	bool		IsDirectoryExists(const char*);
	bool		IsDirectoryExists(std::string);
}
#endif
