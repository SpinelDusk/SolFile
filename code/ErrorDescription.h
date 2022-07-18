/// \file
/// \brief Error description list (Header)
/// \author SpinelDusk

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef ErrorDescriptionH
#define ErrorDescriptionH

#include <vector>
#pragma hdrstop//This directive terminates the list of header files eligible for precompilation (Using ONLY for <standard> headers, no "custom")

#include "Conversion.h"
#include "CHF.h"

/// \brief Structure describing list of data for error
struct ErrorData
{
	/// \brief Name of module where error occurred
	char*		File;
	/// \brief Name of function (in the module) where error occurred
	char*		Function;
	/// \brief Line number (in the module) where error occurred
	char*		Line;
	/// \brief Error index
	cnv::unsint	ErrorIndex;
	/// \brief Active array number
	cnv::unsint DescriptionSet;
	/// \brief Additional error information
	char*		Additional;
};

void SendAlert(const char*, const char[], int, cnv::unsint, cnv::unsint, std::string);
void SendAlert(const char*, const char[], int, cnv::unsint, cnv::unsint, char*);
void SendAlert(const char*, const char[], int, cnv::unsint, cnv::unsint, cnv::unistr);
void AlertControl(const char*, const char[], int, cnv::unsint, cnv::unsint);
#endif
