/// \file
/// \brief Sol file module (Header)
/// \author SpinelDusk

#pragma GCC diagnostic ignored "-Wdocumentation-unknown-command"//Disable warning: Unknown command tag name
#pragma GCC diagnostic ignored "-Wpadded"						//Disable warning: Padding class -- with -- byte to align --

#pragma once//This header guard directive designed to cause the current source file to be included only once in a single compilation
#ifndef SolFileH
#define SolFileH

#include <vcl.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <io.h>
#include <vector>
#include <fstream> 		// ofstream
#pragma hdrstop//This directive terminates the list of header files eligible for precompilation (Using ONLY for <standard> headers, no "custom")

#include "FileMark.h"
#include "CHF.h"
#include "ErrorDescription.h"

/// \cond
/// \brief Library LZ-decompression function
/// \param[out] OutputBuffer Vector to get uncompressed data
/// \param[in] InputBuffer Vector with compressed data
/// \param[in] BufferSize Buffer size for decompressing
/// \return Uncompression size of output data
extern "C" __declspec(dllexport) __stdcall long LZDecompress(unsigned char* OutputBuffer, unsigned char* InputBuffer, unsigned long BufferSize);
/// \brief Library LZ-compression function
/// \param[out] OutputBuffer Vector to get compressed data
/// \param[in] InputBuffer Vector with uncompressed data
/// \param[in] BufferSize Buffer size for compressing
/// \return Compression size of output data
extern "C" __declspec(dllexport) __stdcall long LZCompress(unsigned char* OutputBuffer, unsigned char* InputBuffer, unsigned long BufferSize);
/// \endcond

/// \brief The class contains data and functions necessary for packing SOL files
class SolPack
{
	private:
	/// \brief Archive checksum
	cnv::dword CheckSum;
	/// \brief Amount of table of contents entries
	cnv::dword AmountEntries;
	/// \brief Offset of beginning of data in the archive
	cnv::dword DataStartOffset;
	/// \brief Packing file-entry structure
	struct Entry
	{
		/// \brief Offset of file-entry in the archive
		cnv::dword DataOffset;
		/// \brief Compression state flag of file-entry in the archive
		int CompressFlag;
		/// \brief Path to file for packing
		std::string Path;
	};
	/// \brief Archive table of contents array
	std::vector<Entry> TOCList;

	/// \brief Archive Path
	std::string ArchivePath;
	/// \brief List Path
	std::string ListPath;
	/// \brief File stream for list file (input)
	ifstream OpenList;
	/// \brief File stream for destination file (output)
	ofstream OpenReceiver;

	public:
	SolPack()
	{

	}
	~SolPack()
	{

	}

	int Pack(char*, char*);

	/// \brief Get archive checksum
	cnv::dword GetCheckSum(){return CheckSum;}
	/// \brief Get amount of table of contents entries
	cnv::dword GetAmountEntries(){return AmountEntries;}
	/// \brief Get start offset of data in the archive
	cnv::dword GetDataStartOffset(){return DataStartOffset;}
	/// \brief Get archive path
	std::string GetArchivePath(){return ArchivePath;}
	/// \brief Get list path
	std::string GetListPath(){return ListPath;}
	/// \brief Get compression flag of file-entry in the archive
	int GetCompressFlag(cnv::unsint i){return TOCList.at(i).CompressFlag;}
	/// \brief Get file-entry path
	std::string GetEntryPath(cnv::unsint i){return TOCList.at(i).Path;}

	private:
	void Clear();
	int ReadOpenList(ifstream&);
	int FillData();
	int FillHead();
	int AddUncompressedFile(cnv::unsint);
	int AddCompressedFile(cnv::unsint);
	int AddEmptedFile(cnv::unsint);
};
/// \brief The class contains data and functions necessary for unpacking SOL files
class SolUnpack
{
	private:
	/// \brief Archive checksum
	cnv::dword CheckSum;
	/// \brief Offset of beginning of data in the archive
	cnv::dword DataStartOffset;
	/// \brief Amount of entries in the archive
	cnv::dword AmountEntries;
	/// \brief Archive size in bytes
	cnv::dword FileSize;
	/// \brief Unpacking file-entry structure
	struct Entry
	{
		/// \brief Offset of file-entry in the archive
		cnv::dword DataOffset;
		/// \brief Compressed size (in bytes) of file-entry in the archive
		cnv::dword CompressSize;
		/// \brief Uncompressed size (in bytes) of file-entry in the archive
		cnv::dword UncompressSize;
		/// \brief Compression state flag of file-entry in the archive
		cnv::dword CompressFlag;
		/// \brief Extension of file-entry in the archive
		cnv::dword Extension;
		/// \brief Path to file-entry in the archive
		std::string Path;
	};
	/// \brief Archive table of contents array
	std::vector<Entry> TOCList;
	
	/// \brief Buffer for reading entries from the archive
	std::vector<cnv::byte> ReadBuffer;
	/// \brief Uncompressed data buffer for single entry
	std::vector<cnv::byte> DecompBuffer;
	/// \brief Buffer to store size of uncompressed entry
	cnv::dword UncompressedSize;

	/// \brief File stream for archive (input)
	ifstream OpenArchive;
	/// \brief File stream for destination file (output)
	ofstream OpenReceiver;
	/// \brief Archive path
	std::string ArchivePath;

	public:
	SolUnpack()
	{

	}
	~SolUnpack()
	{

	}

	int Load(char*);
	int ExtractFromArchive(cnv::unsint Count);
	int UnpackEntireArchive();
    int CreateListFile();

	/// \brief Get archive path
	std::string GetArchivePath(){return ArchivePath;}
	/// \brief Get archive checksum
	cnv::dword GetCheckSum(){return CheckSum;}
	/// \brief Get start offset of data in the archive
	cnv::dword GetDataStartOffset(){return DataStartOffset;}
	/// \brief Get amount of table of contents entries
	cnv::dword GetAmountEntries(){return AmountEntries;}
	/// \brief Get file size
	cnv::dword GetFileSize(){return FileSize;}
	/// \brief Get offset of file-entry in the archive
	cnv::dword GetDataOffset(cnv::unsint i){return TOCList.at(i).DataOffset;}
	/// \brief Get compression size of file-entry in the archive
	cnv::dword GetCompressSize(cnv::unsint i){return TOCList.at(i).CompressSize;}
	/// \brief Get decompression size of file-entry in the archive
	cnv::dword GetUncompressSize(cnv::unsint i){return TOCList.at(i).UncompressSize;}
	/// \brief Get compression flag of file-entry in the archive
	cnv::dword GetCompressFlag(cnv::unsint i){return TOCList.at(i).CompressFlag;}
	/// \brief Get file extension of file-entry in the archive
	cnv::dword GetExtension(cnv::unsint i){return TOCList.at(i).Extension;}
	/// \brief Get path of file-entry in the archive
	std::string GetPath(cnv::unsint i){return TOCList.at(i).Path;}

	private:
	void Clear();
};

int ComparingCheckSums(char*, cnv::dword);
int CalcCheckSum(char*, cnv::dword&);
int DwordRead(ifstream&, std::vector<cnv::dword>&, cnv::unsint);
int DwordRead(ifstream&, cnv::dword&);
int StreamRead(ifstream&, char*, int);
#endif
