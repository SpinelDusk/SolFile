/// \file
/// \brief File signature and extension functions (Header)
/// \author SpinelDusk

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef FileMarkH
#define FileMarkH

#include "CHF.h"

/// \brief Enumeration of compression flags
enum CompressFlag
{
	CF_UNCOMPRESS = 0,	///< Uncompress entry
	CF_COMPRESS = 1,	///< Compress entry
	CF_EMPTY = 2		///< Empty entry (no size entry)
};
/// \brief Enumerating file types
enum FileMode
{
	FM_UNKNOWN = 1000,		///< Unknown file type
	FM_NOTYPE = 0,          ///< No type
	FM_ANY = 1,             ///< Any types
	FM_ALL = 2,				///< All types
	FM_ALL_CHECKBOX = 5,	///< All types (works only with enable checkbox)
	FM_ARCHIVES = 3,		///< All archive types (DPK, FST, MPK, PAK, PKK, SAV, SOL)
	FM_GRAPHICS = 4,		///< All graphics types (PAL, SHP, TGA)
	FM_DPK = 10,			///< DPK type
	FM_FST = 11,			///< FST type
	FM_MPK = 12,			///< MPK type
	FM_PAK = 13,			///< PAK type
	FM_PKK = 14,			///< PKK type
	FM_SAV = 15,			///< SAV type
	FM_SOL = 16,			///< SOL type
	FM_PAL = 17,			///< PAL type
	FM_SHP = 18,			///< SHP type
	FM_TGA = 19,			///< TGA type
	FM_BMP = 20,			///< BMP type
	FM_GIF = 21,			///< GIF type
	FM_FIT = 22,			///< FIT type
	FM_WAV = 23				///< WAV type
};

cnv::dword  GetFileTypeFromSignature(cnv::byte*);
cnv::unsint GetFileTypeFromExt(std::string local_Input);
std::string GetExtFromFileType(cnv::unsint);
std::string GetExtFromPath(std::string);
#endif
