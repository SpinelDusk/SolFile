/// \file
/// \brief Lead module (Header)
/// \author SpinelDusk

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef FileH
#define FileH

#include "SolFile.h"
#include "FileMark.h"

extern ErrorData LastError;///< Defined in ErrorDescription.cpp
extern std::vector<std::string> SolFileErrorDescription;

void Gap(cnv::dword, cnv::unsint);
void Gap16(cnv::dword, cnv::unsint);
void ArchiveHead();
void ArchiveEntry(cnv::unsint);
int  Show(char*);
int  Unpack(char*);
int  Extract(char*, cnv::unsint);
int  List(char*);
int  Pack(char*, char*);
int  Complex(char*);
#endif

/// \mainpage SolFile technical documentation