/// \file
/// \brief Common hidden features
/// \author SpinelDusk
/// \details Contains functions for processing strings, arrays, files, folders, and paths

#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"//Disable warning: No previous extern declaration for non-static variable

#pragma package(smart_init)//Ensures that packaged units are initialized in the order determined by their dependencies

#include "CHF.h"

/// \brief Decomposition of path to components returned in the arguments of this function
/// \param[in] local_Input Path for decomposition
/// \param[out] SymbolOfDisk Symbol of disk
/// \param[out] PathOfDirectory Path of directory
/// \param[out] NameOfFile Name of file without extension
/// \param[out] ExtensionOfFile Extension of file
void chf::SplitPath(char* local_Input, char** SymbolOfDisk, char** PathOfDirectory, char** NameOfFile, char** ExtensionOfFile)
{
	char _SymbolOfDisk			[MAX_PATH];
	char _PathOfDirectory		[MAX_PATH];
	char _NameOfFile			[MAX_PATH];
	char _ExtensionOfFile		[MAX_PATH];
	//Separating the path of file - into separate strings (specified in the function parameters)
	_splitpath(local_Input, _SymbolOfDisk, _PathOfDirectory, _NameOfFile, _ExtensionOfFile);
	//Drive letter
	if(SymbolOfDisk != nullptr)
		*SymbolOfDisk		= strdup((std::string(_SymbolOfDisk)).data());
	//Path to folder with file
	if(PathOfDirectory != nullptr)
		*PathOfDirectory	= strdup((std::string(_PathOfDirectory)).data());
	//File name
	if(NameOfFile != nullptr)
		*NameOfFile 		= strdup((std::string(_NameOfFile)).data());
	//File extension
	if(ExtensionOfFile != nullptr)
		*ExtensionOfFile	= strdup((std::string(_ExtensionOfFile)).data());
}
/// \brief Decomposition of path to components returned in the arguments of this function
/// \param[in] local_Input Path for decomposition
/// \param[out] SymbolOfDisk Symbol of disk
/// \param[out] PathOfDirectory Path of directory
/// \param[out] NameOfFile Name of file without extension
/// \param[out] ExtensionOfFile Extension of file
void chf::SplitPath(std::string local_Input, std::string& SymbolOfDisk, std::string& PathOfDirectory, std::string& NameOfFile, std::string& ExtensionOfFile)
{
	char _SymbolOfDisk			[_MAX_DRIVE];
	char _PathOfDirectory		[_MAX_DIR];
	char _NameOfFile			[_MAX_DIR];
	char _ExtensionOfFile		[_MAX_EXT];
	//Separating the path of file - into separate strings (specified in the function parameters)
	_splitpath(local_Input.data(), _SymbolOfDisk, _PathOfDirectory, _NameOfFile, _ExtensionOfFile);
	//Drive letter
	SymbolOfDisk	= std::string(_SymbolOfDisk);
	//Path to folder with file
	PathOfDirectory	= std::string(_PathOfDirectory);
	//File name
	NameOfFile		= std::string(_NameOfFile);
	//File extension
	ExtensionOfFile	= std::string(_ExtensionOfFile);
}
/// \brief Create folder
/// \param[in] DirectoryName Path to the created directory
/// \param[in] SizeOf Array length of DirectoryName
/// \return Error code: nonzero - no errors, 0 - operation error
bool chf::CreateDirectory(char* DirectoryName, cnv::unsint SizeOf)
{
	std::string buff1(DirectoryName, SizeOf);
	const char* buff2 = buff1.data();
	return CreateDirectoryA(buff2, nullptr);
}
/// \brief Create folder
/// \param[in] DirectoryName Path to the created directory
/// \return Error code: nonzero - no errors, 0 - operation error
bool chf::CreateDirectory(std::string DirectoryName)
{
	return CreateDirectoryA(DirectoryName.data(), nullptr);
}
/// \brief Get size for vector (maximum size - MAX_PATH)
/// \param[in] local_Input Input vector
/// \return Size of vector
cnv::unsint chf::SizeOfVector(char* local_Input)
{
	if(local_Input == nullptr)
	{
        return 0;
    }
	cnv::unsint SizeOf = 0;
	for(cnv::unsint i = 0; i < MAX_PATH; i++)
	{
		if(local_Input[i] != 0)
		{
			SizeOf++;
		}
		else
		{
			break;
		}
	}
	return SizeOf;
}
/// \brief Get size for vector (maximum size - MAX_PATH)
/// \param[in] local_Input Input vector
/// \return Size of vector
cnv::unsint chf::SizeOfVector(const char* local_Input)
{
	cnv::unsint SizeOf = 0;
	for(cnv::unsint i = 0; i < MAX_PATH; i++)
	{
		if(local_Input[i] != 0)
		{
			SizeOf++;
		}
		else
		{
			break;
		}
	}
	return SizeOf;
}
/// \brief Get size for array (maximum size - MAX_PATH)
/// \param[in] local_Input Input array
/// \return Size of array
cnv::unsint chf::SizeOfArray(const char local_Input[])
{
	cnv::unsint SizeOf = 0;
	for(cnv::unsint i = 0; i < MAX_PATH; i++)
	{
		if(local_Input[i] != 0)
		{
			SizeOf++;
		}
		else
		{
			break;
		}
	}
	return SizeOf;
}
/// \brief Get directory of the executable file
/// \return Path of executable directory
cnv::unistr chf::GetExecutableDirectory()
{
	TCHAR ExecutableFile[MAX_PATH];
	GetModuleFileName(GetModuleHandle(nullptr), ExecutableFile, MAX_PATH);//Get path for the executable file
	PathRemoveFileSpec(ExecutableFile);//Strip file name
	std::wstring BufferWString(ExecutableFile);
	std::string BufferSString(BufferWString.begin(), BufferWString.end());
	return BufferSString.data();
}
/// \brief Get file name from the path
/// \param[in] local_Input File path
/// \return File name
cnv::unistr chf::GetFileName(cnv::unistr local_Input)
{
	char* StrBuffer = new char[MAX_PATH];
	SplitPath(STRINGtoCHAR(USTRINGtoSTRING(local_Input)), nullptr, nullptr, &StrBuffer, nullptr);
	cnv::unistr local_Output = CHARtoUSTRING(StrBuffer);
	delete[] StrBuffer; StrBuffer = nullptr;
	return local_Output;
}
/// \brief Get file extension from the path
/// \param[in] local_Input File path
/// \return File extension
cnv::unistr chf::GetFileExtension(cnv::unistr local_Input)
{
	char* StrBuffer = new char[MAX_PATH];
	SplitPath(STRINGtoCHAR(USTRINGtoSTRING(local_Input)), nullptr, nullptr, nullptr, &StrBuffer);
	cnv::unistr local_Output = CHARtoUSTRING(StrBuffer);
	local_Output.Delete0(0, 1);//Removes the first character (null) from UnicodeString. '1' here is equal to number of characters to be deleted
	delete[] StrBuffer; StrBuffer = nullptr;
	return local_Output;
}
/// \brief Get file size
/// \param[in] local_Input File descriptor
/// \return File size
cnv::dword chf::GetFileSize(FILE* local_Input)
{
	return static_cast<cnv::dword>(filelength(fileno(local_Input)));
}
/// \brief Get file size
/// \param[in] local_Input File stream
/// \return File size
cnv::dword chf::GetFileSize(ifstream& local_Input)
{
	std::streamoff pos = local_Input.tellg();
	local_Input.seekg(0, local_Input.end);
	std::streamoff fsize = local_Input.tellg();
	local_Input.seekg(pos, local_Input.beg);
	return static_cast<cnv::dword>(fsize);
}
/// \brief Get file size
/// \param[in] local_Input Path to file
/// \return File size (or 0 - if file can't be open or not exist)
cnv::dword chf::GetFileSize(std::string local_Input)
{
	ifstream InputFile;
	InputFile.open(local_Input, ios::in|ios::binary);
	if(InputFile.is_open())
	{
		InputFile.seekg(0, InputFile.end);
		std::streamoff fsize = InputFile.tellg();
		InputFile.close();
		return static_cast<cnv::dword>(fsize);
	}
	return 0;
}
/// \brief Get full path to a file based on the path to the executable
/// \param[in] local_Input Path to the file to restore
/// \return Full path to the file
char* chf::GetFullPath(char* local_Input)
{
	std::string local_Output; local_Output.resize(MAX_PATH);
	std::string SymbolOfDisk; SymbolOfDisk.resize(_MAX_DRIVE);
	std::string PathOfDirectory; PathOfDirectory.resize(_MAX_DIR);
	std::string NameOfFile; NameOfFile.resize(_MAX_DIR);
	std::string ExtensionOfFile; ExtensionOfFile.resize(_MAX_EXT);
	SplitPath(local_Input, SymbolOfDisk, PathOfDirectory, NameOfFile, ExtensionOfFile);
	if(SymbolOfDisk.size() != 2)
	{
		wchar_t ExecutableFile[MAX_PATH];
		GetModuleFileName(GetModuleHandle(nullptr), ExecutableFile, MAX_PATH);//Get path for the executable file
		PathRemoveFileSpec(ExecutableFile);//Strip file name
		local_Output = WCHARTtoSTRING(ExecutableFile) + "\\" + NameOfFile + ExtensionOfFile;
		return local_Output.data();
	}
	return local_Input;
}
/// \brief Check if a directory exists
/// \param[in] local_Input Path to the directory
/// \return True - if exists, False - otherwise
bool chf::IsDirectoryExists(const wchar_t* local_Input)
{
	DWORD Flag = GetFileAttributes(local_Input);
	if(Flag == 0xFFFFFFFFUL)
	{
		if(GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
            return false;
    }
	if(!(Flag & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}
/// \brief Check if a directory exists
/// \param[in] local_Input Path to the directory
/// \return True - if exists, False - otherwise
bool chf::IsDirectoryExists(const char* local_Input)
{
	DWORD Flag = GetFileAttributes(CONSTCHARtoCONSTWCHART(local_Input));
	if(Flag == 0xFFFFFFFFUL)
	{
		if(GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
			return false;
    }
	if(!(Flag & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}
/// \brief Check if a directory exists
/// \param[in] local_Input Path to the directory
/// \return True - if exists, False - otherwise
bool chf::IsDirectoryExists(std::string local_Input)
{
	DWORD Flag = GetFileAttributes(STRINGtoCONSTWCHART(local_Input));
	if(Flag == 0xFFFFFFFFUL)
	{
		if(GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
            return false;
    }
	if(!(Flag & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}
