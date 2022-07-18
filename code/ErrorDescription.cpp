/// \file
/// \brief Error description list
/// \author SpinelDusk
/// \details Contains error handling functions and error descriptions

#pragma GCC diagnostic ignored "-Wc++98-compat"					//Disable warning: 'nullptr' is incompatible with C++98
#pragma GCC diagnostic ignored "-Wexit-time-destructors"		//Disable warning: Declaration requires an exit-time destructor
#pragma GCC diagnostic ignored "-Wglobal-constructors"			//Disable warning: Declaration requires a global destructor
#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"//Disable warning: No previous extern declaration for non-static variable

#include "ErrorDescription.h"

/// \brief Instance of structure to store error information
ErrorData LastError;

/// \brief The function to handle error message
/// \param[in] FILE Module name
/// \param[in] FUNC Function name in the module
/// \param[in] LINE Line number in the module
/// \param[in] Index Error index
/// \param[in] DeSet Active array number
/// \param[in] Add Additional error information
void SendAlert(const char* FILE, const char FUNC[], int LINE, cnv::unsint Index, cnv::unsint DeSet, std::string Add)
{
	LastError.Additional = new char[MAX_PATH];
	strncpy(LastError.Additional, Add.data(), Add.size());
	AlertControl(FILE, FUNC, LINE, Index, DeSet);
}
/// \brief The function to handle error message
/// \param[in] FILE Module name
/// \param[in] FUNC Function name in the module
/// \param[in] LINE Line number in the module
/// \param[in] Index Error index
/// \param[in] DeSet Active array number
/// \param[in] Add Additional error information
void SendAlert(const char* FILE, const char FUNC[], int LINE, cnv::unsint Index, cnv::unsint DeSet, char* Add)
{
	LastError.Additional = new char[MAX_PATH];
	if(Add != nullptr)
		strncpy(LastError.Additional, Add, chf::SizeOfVector(Add));
	AlertControl(FILE, FUNC, LINE, Index, DeSet);
}
/// \brief The function to handle error message
/// \param[in] FILE Module name
/// \param[in] FUNC Function name in the module
/// \param[in] LINE Line number in the module
/// \param[in] Index Error index
/// \param[in] DeSet Active array number
/// \param[in] Add Additional error information
void SendAlert(const char* FILE, const char FUNC[], int LINE, cnv::unsint Index, cnv::unsint DeSet, cnv::unistr Add)
{
	LastError.Additional = new char[MAX_PATH];
	LastError.Additional = USTRINGtoCHAR(Add);
	AlertControl(FILE, FUNC, LINE, Index, DeSet);
}
/// \brief Error handling control function
/// \param[in] FILE Module name
/// \param[in] FUNC Function name in the module
/// \param[in] LINE Line number in the module
/// \param[in] Index Error index
/// \param[in] DeSet Active array number
void AlertControl(const char* FILE, const char FUNC[], int LINE, cnv::unsint Index, cnv::unsint DeSet)
{
	LastError.File		= new char[MAX_PATH];
	LastError.Function	= new char[MAX_PATH];
	LastError.Line		= new char[MAX_PATH];
	strncpy(LastError.File,		FILE, chf::SizeOfVector(FILE));
	strncpy(LastError.Function,	FUNC, chf::SizeOfArray(FUNC));
	strncpy(LastError.Line,		INTtoCHAR(LINE), chf::SizeOfVector(INTtoCHAR(LINE)));
	LastError.ErrorIndex = Index;
	LastError.DescriptionSet = DeSet;
}

/// \brief Vector containing description of errors for SolFile
std::vector<std::string> SolFileErrorDescription =
{
		"NULL", "NULL",
		"Can't open input file list.", "File error",
		"The file is empty.", "File error",
		"Can't open output file.", "File error",
		"Instruction read error. Unknown command. Check if the command is entered correctly.", "File error",
/*5*/	"Instruction read error. Unknown command. Check if the command is entered correctly.", "File error",
		"Instruction read error. Packing file not found.", "File error",
		"Instruction read error. Incorrect entry.", "File error",
		"No entries were received.", "File error",
		"Can't open file.", "File error",
/*10*/	"File limit exceeded. Files no larger than 50 MB are allowed.", "File error",
		"Invalid position indicator value was received in the archive file.", "File error",
		"Can't open file.", "File error",
		"Invalid position indicator value was received in the archive file.", "File error",
		"Invalid position indicator value was received in the archive file.", "File error",
/*15*/	"Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)", "File error",
		"The file is empty.", "File error",
		"Archive not loaded into memory.", "Logic error",
		"Violating boundaries of the table of contents (Attempted to read outside the list limits).", "Logic error",
		"Attempt to unpack empty entry.", "File error",
/*20*/	"Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)", "File error",
		"Directory does not exist.", "Logic error",
		"The file can't be opened or does not exist.", "Logic error",
		"The created file does not exist or has size of zero.", "Logic error",
		"Archive not loaded into memory.", "Logic error",
/*25*/	"Attempt to create list failed. File does not exist or is locked. (Or the file path contains Unicode characters.)", "File error",
		"The checksums don't match. The file is corrupted or is not archive.", "File error",
		"Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)", "File error",
		"Unexpected end of file.", "File error",
		"The requested read size exceeds the buffer allocated for storage.", "Logic error",
/*30*/	"The file data ended before data block was read. The file is corrupted.", "File error",
		"ReadStream - Logical error on input operation.", "Logic error",
		"ReadStream - Read error on input operation.", "Logic error",
		"The file data ended before data block was read. The file is corrupted.", "File error",
		"ReadStream - Logical error on input operation.", "Logic error",
/*35*/	"ReadStream - Read error on input operation.", "Logic error",
		"The file data ended before data block was read. The file is corrupted.", "File error",
		"ReadStream - Logical error on input operation.", "Logic error",
		"ReadStream - Read error on input operation.", "Logic error"
};
