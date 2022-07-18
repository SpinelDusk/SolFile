/// \file
/// \brief Conversions of types
/// \author SpinelDusk
/// \details Contains functions for converting data and naming custom types

#include "Conversion.h"

#pragma package(smart_init)//Ensures that packaged units are initialized in the order determined by their dependencies

/// \brief Conversion from std::string to char*
/// \param[in] local_Input Input value to be converted
/// \return Output value
char* 			STRINGtoCHAR		(std::string local_Input)
{
	return strdup(local_Input.data());
}
/// \brief Conversion from UnicodeString to std::string
/// \param[in] local_Input Input value to be converted
/// \return Output value
std::string		USTRINGtoSTRING		(cnv::unistr local_Input)
{
	std::string local_Output = AnsiString(local_Input).c_str();
	return local_Output;
}
/// \brief Conversion from wchar_t* to std::string
/// \param[in] local_Input Input value to be converted
/// \return Output value
std::string 	WCHARTtoSTRING		(wchar_t* local_Input)
{
	std::wstring StrBuffer(local_Input);
	std::string local_Output(StrBuffer.begin(), StrBuffer.end());
	return local_Output;
}
/// \brief Conversion from std::string to const char*
/// \param[in] local_Input Input value to be converted
/// \return Output value
const char* 	STRINGtoCONSTCHAR	(std::string local_Input)
{
	char* StrBuffer = new char[local_Input.size() + 1];
	StrBuffer = strdup(local_Input.data());
	const char* local_Output = StrBuffer;
	return local_Output;
}
/// \brief Conversion from int to char*
/// \param[in] local_Input Input value to be converted
/// \return Output value
char*			INTtoCHAR			(int local_Input)
{
	std::stringstream StrBuffer;
	StrBuffer << local_Input;
	std::string local_Output = StrBuffer.str();
	return strdup(local_Output.data());
}
/// \brief Conversion from int to std::string
/// \param[in] local_Input Input value to be converted
/// \return Output value
std::string 	INTtoSTRING			(int local_Input)
{
	std::stringstream StrBuffer;
	StrBuffer << local_Input;
	std::string local_Output = StrBuffer.str();
	return local_Output;
}
/// \brief Conversion from char* to UnicodeString
/// \param[in] local_Input Input value to be converted
/// \return Output value
cnv::unistr		CHARtoUSTRING       (char* local_Input)
{
	std::string StrBuffer(local_Input, strlen(local_Input));
	cnv::unistr local_Output = StrBuffer.data();
	return local_Output;
}
/// \brief Conversion from const char* to const wchar_t*
/// \param[in] local_Input Input value to be converted
/// \return Output value
const wchar_t* CONSTCHARtoCONSTWCHART(const char* local_Input)
{
	const size_t Size = strlen(local_Input) + 1;
	wchar_t* local_Output = new wchar_t[Size];
	mbstowcs(local_Output, local_Input, Size);
	return local_Output;
}
/// \brief Conversion from std::string to const wchar_t*
/// \param[in] local_Input Input value to be converted
/// \return Output value
const wchar_t* 	STRINGtoCONSTWCHART	(std::string local_Input)
{
	char* StrBuffer = new char[local_Input.size() + 1];
	StrBuffer = strdup(local_Input.data());
	wchar_t* local_Output = new wchar_t[local_Input.size()];
	mbstowcs(local_Output, StrBuffer, local_Input.size());
	return local_Output;
}
/// \brief Conversion from UnicodeString to char*
/// \param[in] local_Input Input value to be converted
/// \return Output value
char* 			USTRINGtoCHAR		(cnv::unistr local_Input)
{
	AnsiString buffer(local_Input);
	char* local_Output = new char[static_cast<cnv::unsint>(buffer.Length()+1)];
	strcpy(local_Output, buffer.c_str());
	return local_Output;
}
/// \brief Conversion from String to UnicodeString
/// \param[in] local_Input Input value to be converted
/// \return Output value
cnv::unistr		STRINGtoUSTRING     (std::string local_Input)
{
	cnv::unistr local_Output = local_Input.data();
	return local_Output;
}
/// \brief Conversion from int to UnicodeString
/// \param[in] local_Input Input value to be converted
/// \return Output value
cnv::unistr		INTtoUSTRING   		(int local_Input)
{
	std::stringstream StrBuffer;
	StrBuffer << local_Input;
	std::string local_Output = StrBuffer.str();
	return local_Output.data();
}
