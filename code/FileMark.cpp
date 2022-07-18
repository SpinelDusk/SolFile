/// \file
/// \brief File signature and extension functions
/// \author SpinelDusk
/// \details Contains functions for converting the characteristics of file types

#pragma package(smart_init)//Ensures that packaged units are initialized in the order determined by their dependencies

#include "FileMark.h"

/// \brief Get file type based on file signature
/// \param[in] local_Input File signature
/// \return File type on success or 1000 (FM_UNKNOWN)
cnv::dword GetFileTypeFromSignature(cnv::byte* local_Input)
{
	const int Basis = 4;
	const int Signs = 5;

	cnv::byte* InputChar = new cnv::byte[Basis];
	for(int i = 0; i < Basis - 1; i++)
	{
		InputChar[i] = local_Input[i];
	}
	InputChar[Basis - 1] = 0;

	char SignArray[Signs][Basis] = {{"GIF"}, {"1.1"}, {"FIT"}, {"WAV"}, {static_cast<char>(0xCE), static_cast<char>(0xFA), static_cast<char>(0xED)}};
	cnv::dword ModeArray[Signs] = {FM_GIF, FM_SHP, FM_FIT, FM_WAV, FM_PAK};
	for(int i = 0; i < Signs; i++)
	{
		if(!memcmp(InputChar, SignArray[i], Basis))
		{
			delete[] InputChar; InputChar = nullptr;
			return ModeArray[i];
		}
	}
	delete[] InputChar; InputChar = nullptr;
	return FM_UNKNOWN;
}
/// \brief Get file extension based on file type
/// \param[in] local_Input File type
/// \return File extension
std::string GetExtFromFileType(cnv::unsint local_Input)
{
	const int Exts = 14;
	std::string local_Output;
	std::string ExtArray[Exts] = {"unk", "pak", "gif", "shp", "fit", "wav", "dpk", "fst", "mpk", "pkk", "sav", "sol", "pal", "tga"};
	cnv::unsint ModeArray[Exts] = {FM_UNKNOWN, FM_PAK, FM_GIF, FM_SHP, FM_FIT, FM_WAV, FM_DPK, FM_FST, FM_MPK, FM_PKK, FM_SAV, FM_SOL, FM_PAL, FM_TGA};
	for(int i = 0; i < Exts; i++)
	{
		if(local_Input == ModeArray[i])
		{
			local_Output = ExtArray[i];
			return local_Output;
		}
	}
	local_Output = ExtArray[0];
	return local_Output;
}
/// \brief Get file extension based on file extension in path
/// \param[in] local_Input File path
/// \return File extension
std::string GetExtFromPath(std::string local_Input)
{
	std::string local_Output = "unk";
	cnv::unsint Position = 0;
	for(cnv::unsint i = 0; i < local_Input.size(); i++)
	{
		if(local_Input.at(i) == '.')
		{
			Position = i;
		}
	}
	if(Position != 0)
	{
		local_Output = local_Input.substr(Position + 1);
		for(cnv::unsint j = 0; j < local_Output.size(); j++)
			local_Output.at(j) = static_cast<char>(tolower(local_Output.at(j)));
	}
	return local_Output;
}
/// \brief Get file type based on file extension
/// \param[in] local_Input File extension
/// \return File type
cnv::unsint GetFileTypeFromExt(std::string local_Input)
{
	std::transform(local_Input.begin(), local_Input.end(), local_Input.begin(), ::tolower);
    const int Exts = 14;
	cnv::unsint local_Output;
	std::string ExtArray[Exts] = {"unk", "pak", "gif", "shp", "fit", "wav", "dpk", "fst", "mpk", "pkk", "sav", "sol", "pal", "tga"};
	cnv::unsint ModeArray[Exts] = {FM_UNKNOWN, FM_PAK, FM_GIF, FM_SHP, FM_FIT, FM_WAV, FM_DPK, FM_FST, FM_MPK, FM_PKK, FM_SAV, FM_SOL, FM_PAL, FM_TGA};
	for(int i = 0; i < Exts; i++)
	{
		if(local_Input == ExtArray[i])
		{
			local_Output = ModeArray[i];
			return local_Output;
		}
	}
	local_Output = ModeArray[0];
	return local_Output;
}
