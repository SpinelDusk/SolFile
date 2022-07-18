/// \file
/// \brief Lead module
/// \author SpinelDusk
/// \details Lead application module

#pragma GCC diagnostic ignored "-Wc++98-compat"					//Disable warning: 'nullptr' is incompatible with C++98
#pragma GCC diagnostic ignored "-Wexit-time-destructors"		//Disable warning: Declaration requires an exit-time destructor
#pragma GCC diagnostic ignored "-Wglobal-constructors"			//Disable warning: Declaration requires a global destructor
#pragma GCC diagnostic ignored "-Wmissing-variable-declarations"//Disable warning: No previous extern declaration for non-static variable

#pragma argsused//This directive disables warnings about not using function parameters

#include "Main.h"
#include "ErrorDescription.h"

/// \brief Program flag responsible for ignoring pause at end of the program
bool IgnorePause = 0;
/// \brief Program flag responsible for ignoring display of information on screen
bool IgnoreInfo = 0;
/// \brief Program flag responsible for storing the value of modifier setting error
bool InvalidMod = 0;
/// \brief Program build number (date of compilation in format YYMMDD)
int  Build = 220718;
/// \brief Program version number (sequence version)
std::string Version = "1.0.0";
/// \brief Instance of class that handles all processes for unpacking an archive
SolUnpack SolUObject;
/// \brief Instance of class that handles all processes for packing into an archive
SolPack   SolPObject;

/// \brief Gap setting for correct display of output information. Decimal numbers
/// \param[in] local_Input Number on which the gap is based
/// \param[in] Size Size of gap
void Gap(cnv::dword local_Input, cnv::unsint Size)
{
	if(Size == 0 || Size > 250)
	{
		Size = 15;
	}
	cnv::unsint j = 0;
	while(local_Input >= 10)
	{
		local_Input = static_cast<cnv::dword>(local_Input / 10);
		j++;
	}
	for(cnv::unsint i = j; i < Size; i++)
	{
		std::cout << " ";
	}
	return;
}
/// \brief Gap setting for correct display of output information. Hexadecimal numbers
/// \param[in] local_Input Number on which the gap is based
/// \param[in] Size Size of gap
void Gap16(cnv::dword local_Input, cnv::unsint Size)
{
	if(Size == 0 || Size > 250)
	{
		Size = 15;
	}
	cnv::unsint j = 0;
	while(local_Input >= 16)
	{
		local_Input = static_cast<cnv::dword>(local_Input / 16);
		j++;
	}
	for(cnv::unsint i = j; i < Size; i++)
	{
		std::cout << " ";
	}
	return;
}
/// \brief Displaying information about archive header
void ArchiveHead()
{
	//Archive path
	std::cout << "ArchivePath    : " << SolUObject.GetArchivePath() << std::endl;

	//Start offset of data
	std::cout << "DataStartOffset: " << std::dec << SolUObject.GetDataStartOffset();
	Gap(SolUObject.GetDataStartOffset(), 15);
	std::cout << "0x" << std::hex << SolUObject.GetDataStartOffset() << std::endl;

	//Amount of entries
	std::cout << "AmountEntries  : " << std::dec << SolUObject.GetAmountEntries();
	Gap(SolUObject.GetAmountEntries(), 15);
	std::cout << "0x" << std::hex << SolUObject.GetAmountEntries() << std::endl;

	//File size
	std::cout << "FileSize       : " << std::dec << SolUObject.GetFileSize();
	Gap(SolUObject.GetFileSize(), 15);
	std::cout << "0x" << std::hex << SolUObject.GetFileSize() << std::dec;
	Gap16(SolUObject.GetFileSize(), 15);
	float kbytes = SolUObject.GetFileSize()/1024.0f;
	float mbytes = kbytes/1024.0f;
	if(mbytes > 1)
	{
		std::cout << mbytes << " MB";
	}
	else if(kbytes > 1)
	{
		std::cout << kbytes << " KB";
	}
	std::cout << std::endl;

	//File CheckSum
	std::cout << "CheckSum       : " << std::dec << SolUObject.GetCheckSum();
	Gap(SolUObject.GetCheckSum(), 15);
	std::cout << "0x" << std::hex << SolUObject.GetCheckSum();
	Gap16(SolUObject.GetCheckSum(), 15);
	cnv::unsint CheckSumLetters[4];
	cnv::dword Remainder = SolUObject.GetCheckSum();
	for(int i = 3; i >= 0; i--)
	{
		CheckSumLetters[i] = static_cast<cnv::unsint>(Remainder / pow(0x100, i));
		Remainder = Remainder - static_cast<cnv::unsint>((CheckSumLetters[i] * pow(0x100, i)));
	}
	for(int i = 0; i < 4; i++)
	{
		if(CheckSumLetters[i] < 0x10)
		{
			std::cout << "0";
        }
		std::cout << std::hex << CheckSumLetters[i] << " ";
	}
	std::cout << std::endl << std::endl;

	//Description string for data table
	std::cout << "Indx Extn DataOffset   (hex)        Uncomp.Size  (hex)        CompressSize (hex)        CompressFlag" << std::endl;
}
/// \brief Displaying information about archive entry
/// \param[in] i Ordinal number of entry in file
void ArchiveEntry(cnv::unsint i)
{
	cnv::unsint Basis = 12;
	cnv::unsint HexBasis = Basis - 2;

	//Index
	std::cout << std::dec << i; Gap(i, 4);
	
	//Extn and DataOffset
	if(SolUObject.GetCompressFlag(i) == 2)
	{
		std::cout << "-    ";
		std::cout << std::dec << SolUObject.GetDataOffset(i); Gap(SolUObject.GetDataOffset(i), Basis);
		std::cout << std::hex << "0x" << SolUObject.GetDataOffset(i); Gap16(SolUObject.GetDataOffset(i), HexBasis);
	}
	else
	{
		std::cout << GetExtFromFileType(static_cast<cnv::unsint>(SolUObject.GetExtension(i))) << "  ";
		std::cout << std::dec << SolUObject.GetDataOffset(i); Gap(SolUObject.GetDataOffset(i), Basis);
		std::cout << std::hex << "0x" << SolUObject.GetDataOffset(i); Gap16(SolUObject.GetDataOffset(i), HexBasis);
	}
	
	//Uncomp.Size
	if(SolUObject.GetUncompressSize(i) == 0)
		std::cout << "-                         ";
	else
	{
		std::cout << std::dec << SolUObject.GetUncompressSize(i); Gap(SolUObject.GetUncompressSize(i), Basis);
		std::cout << std::hex << "0x" << SolUObject.GetUncompressSize(i); Gap16(SolUObject.GetUncompressSize(i), HexBasis);
	}

	//CompressSize
	if(SolUObject.GetCompressSize(i) == 0)
		std::cout << "-                         ";
	else
	{
		std::cout << std::dec << SolUObject.GetCompressSize(i); Gap(SolUObject.GetCompressSize(i), Basis);
		std::cout << std::hex << "0x" << SolUObject.GetCompressSize(i); Gap16(SolUObject.GetCompressSize(i), HexBasis);
	}

	//CompressFlag
	if(SolUObject.GetCompressFlag(i) == 0)
		std::cout << "U - uncompressed" << std::endl;
	else if(SolUObject.GetCompressFlag(i) == 1)
		std::cout << "C - compressed" << std::endl;
	else if(SolUObject.GetCompressFlag(i) == 2)
		std::cout << "E - empty" << std::endl;
}
/// \brief Displaying all information about archive
/// \param[in] local_Input Path to archive to be read
/// \return Error code: 0 - no errors, 1 - operation error
int Show(char* local_Input)
{
	if(SolUObject.Load(local_Input))
	{
		return 1;
	}
	ArchiveHead();
	for(cnv::unsint i = 0; i < SolUObject.GetAmountEntries(); i++)
	{
		ArchiveEntry(i);
	}
	return 0;
}
/// \brief Unpack entire archive
/// \param[in] local_Input Path to archive to be unpacked
/// \return Error code: 0 - no errors, 1 - operation error
int Unpack(char* local_Input)
{
	if(SolUObject.Load(local_Input))
	{
		return 1;
	}
	if(!IgnoreInfo)
	{
		ArchiveHead();
		for(cnv::unsint i = 0; i < SolUObject.GetAmountEntries(); i++)
		{
			ArchiveEntry(i);
		}
	}
	else
	{
		std::cout << "ArchivePath    : " << SolUObject.GetArchivePath();
	}
	if(SolUObject.UnpackEntireArchive() == 1)
	{
		return 1;
	}
	return 0;
}
/// \brief Extract single file from archive
/// \param[in] local_Input Path to archive to be unpacked
/// \param[in] i Ordinal number of entry in file
/// \return Error code: 0 - no errors, 1 - operation error, 2 - invalid ordinal number
int Extract(char* local_Input, cnv::unsint i)
{
	if(SolUObject.Load(local_Input))
	{
		return 1;
	}
	if(i < SolUObject.GetAmountEntries() && SolUObject.GetCompressFlag(i) != 2)
	{
		if(!IgnoreInfo)
		{
			ArchiveHead();
			ArchiveEntry(i);
		}
		if(SolUObject.ExtractFromArchive(i) == 1)
		{
			return 1;
		}
	}
	else
	{
		std::cout << "Error. Invalid input value." << std::endl;
		return 2;
	}
	return 0;
}
/// \brief Create list-file for archive
/// \param[in] local_Input Path to archive to be listed
/// \return Error code: 0 - no errors, 1 - operation error
int List(char* local_Input)
{
	if(SolUObject.Load(local_Input))
	{
		return 1;
	}
	if(!IgnoreInfo)
	{
		ArchiveHead();
		for(cnv::unsint i = 0; i < SolUObject.GetAmountEntries(); i++)
		{
			ArchiveEntry(i);
		}
	}
	else
	{
		std::cout << "ArchivePath    : " << SolUObject.GetArchivePath();
	}
	SolUObject.CreateListFile();
    return 0;
}
/// \brief Pack files into a new archive
/// \param[in] local_Input Path to list-file for packing
/// \param[in] local_Output Path to new created archive
/// \return Error code: 0 - no errors, 1 - operation error
int Pack(char* local_Input, char* local_Output)
{
	if(SolPObject.Pack(local_Input, local_Output))
		return 1;
	if(!IgnoreInfo)
	{
		//List Path
		std::cout 	<< "ListPath       : " << SolPObject.GetListPath() << std::endl;

		//Archive path
		std::cout 	<< "ArchivePath    : " << SolPObject.GetArchivePath() << std::endl;

		//Start offset of data
		std::cout	<< "DataStartOffset: " << std::dec << SolPObject.GetDataStartOffset();
		Gap(SolPObject.GetDataStartOffset(), 15);
		std::cout << "0x" << std::hex << SolPObject.GetDataStartOffset() << std::endl;

		//Amount of entries
		std::cout	<< "AmountEntries  : " << std::dec << static_cast<int>(SolPObject.GetAmountEntries());
		Gap(SolPObject.GetAmountEntries(), 15);
		std::cout << "0x" << std::hex << static_cast<int>(SolPObject.GetAmountEntries()) << std::endl;

		//File size
		std::string OutputString = local_Output;
		cnv::dword OutputSize = chf::GetFileSize(OutputString);
		std::cout	<< "FileSize       : " << std::dec << OutputSize;
		Gap(OutputSize, 15);
		std::cout << "0x" << std::hex << OutputSize << std::dec;
		Gap16(OutputSize, 15);
		float kbytes = OutputSize/1024.0f;
		float mbytes = kbytes/1024.0f;
		if(mbytes > 1)
		{
			std::cout << mbytes << " MB";
		}
		else if(kbytes > 1)
		{
			std::cout << kbytes << " KB";
		}
		std::cout << std::endl;

		//File CheckSum
		std::cout << "CheckSum       : " << std::dec << SolPObject.GetCheckSum();
		Gap(SolPObject.GetCheckSum(), 15);
		std::cout << "0x" << std::hex << SolPObject.GetCheckSum();
		Gap16(SolPObject.GetCheckSum(), 15);
		cnv::unsint CheckSumLetters[4];
		cnv::dword Remainder = SolPObject.GetCheckSum();
		for(int i = 3; i >= 0; i--)
		{
			CheckSumLetters[i] = static_cast<cnv::unsint>(Remainder / pow(0x100, i));
			Remainder = Remainder - static_cast<cnv::dword>(CheckSumLetters[i] * pow(0x100, i));
		}
		for(int i = 0; i < 4; i++)
		{
			if(CheckSumLetters[i] < 0x10)
			{
				std::cout << "0";
			}
			std::cout << std::hex << CheckSumLetters[i] << " ";
		}
		std::cout << std::endl << std::endl;

		//Description string for data table
		std::cout	<< "Indx CF File path" << std::endl;

		//Data table
		for(cnv::dword i = 0; i < SolPObject.GetAmountEntries(); i++)
		{
			std::cout << std::dec << i; Gap(i, 4);
			switch(SolPObject.GetCompressFlag(i))
			{
				case 0:
					std::cout << "u  ";
				break;
				case 1:
					std::cout << "c  ";
				break;
				case 2:
					std::cout << "e  ";
				break;
			}
		   std::cout << SolPObject.GetEntryPath(i) << std::endl;
		}
	}
	else
	{
		std::cout 	<< "ListPath       : " << SolPObject.GetListPath();
	}
	return 0;
}
/// \brief Unpack entire archive and create list-file
/// \param[in] local_Input Path to archive to be unpacked and listed
/// \return Error code: 0 - no errors, 1 - operation error
int Complex(char* local_Input)
{
	if(SolUObject.Load(local_Input))
	{
		return 1;
	}
	if(!IgnoreInfo)
	{
		ArchiveHead();
		for(cnv::unsint i = 0; i < SolUObject.GetAmountEntries(); i++)
		{
			ArchiveEntry(i);
		}
	}
	else
	{
		std::cout << "ArchivePath    : " << SolUObject.GetArchivePath();
	}
	if(SolUObject.UnpackEntireArchive() == 1)
	{
		return 1;
	}
	SolUObject.CreateListFile();
	return 0;
}
/// \brief Main function
/// \param[in] argc Amount of command line arguments
/// \param[in] argv Array for command line arguments
/// \return Error code: 0 - no errors
int _tmain(int argc, _TCHAR* argv[])
{
	std::string Title = "title SolFile " + Version;
	system(Title.data());
	if(argc == 2)
	{
		std::wstring ws(argv[1]);
		std::string Path(ws.begin(), ws.end());
		if(Path.size() == 2)
		if(Path.at(0) == '-' && Path.at(1) == 'h')
		{
			std::cout 	<< "SOLFILE    (build " << Version << " " << Build << " by SpinelDusk)" << std::endl << std::endl
						<< "SolFile is console utility for working with MechCommander game archives -- SOL format." << std::endl
						<< "The program supports: archive viewing, selective extraction, full archive unpacking, creation of a list of contents," << std::endl
						<< "and packing into archive by list-file." << std::endl << std::endl
						<< "* Syntax:" << std::endl
						<< "The program supports the commands for selecting the operating mode, modifiers for editing display mode, and parameters" << std::endl
						<< "will for file paths involved in operations. Commands and modifiers are written together. Use of space is not allowed" << std::endl
						<< "and be interpreted by the program as invalid parameter. The command must be preceded by a dash character. It is" << std::endl
						<< "possible to open a file for viewing by adding path to the file as first parameter." << std::endl << std::endl
						<< "SolFile <command+modificator> <first parameter> <second parameter>" << std::endl << std::endl
						<< "* Operating modes:" << std::endl
						<< "Command        Symbol Mods Parameters                  Result" << std::endl
						<< "Show info      s       p   <Source arc.>               The program will show all contents of archive." << std::endl
						<< "Unpack all     u      api  <Source arc.>               Unpack contents of archive into a separate folder." << std::endl
						<< "Extract single e      api  <Source arc.> <EntryNumber> Single extraction of a file from archive." << std::endl
						<< "create List    l      api  <Source arc.>               Listing contents of archive in a list-file." << std::endl
						<< "Pack           p      api  <Source list> <Target arc.> Packing files into a new archive by list-file." << std::endl
						<< "Complex        c      api  <Source arc.>               Complex unpacking of archive simultaneously with creation of" << std::endl
						<< "                                                       list-file (Unpack all + create List)." << std::endl << std::endl
						<< "* Display modifiers:" << std::endl
						<< "Modifier       Symbol Result" << std::endl
						<< "ignore All     a      This modifier uses the following two at the same time. (Cannot be used with 'Show info' mode)." << std::endl
						<< "ignore Pause   p      Pause at end of the program will be ignored, and after task is completed, the utility will" << std::endl
						<< "                      terminate its work. This parameter is useful for batch operation of the utility." << std::endl
						<< "ignore Info    i      All information output while the program is running will be skipped, only the exit message" << std::endl
						<< "                      (and errors) will be shown. This modifier does not work with 'Show info' mode." << std::endl << std::endl
						<< "Absence of modifiers will display all information available for the mode and the program will wait for any key to be" << std::endl
						<< "pressed after end of work for its completion." << std::endl << std::endl
						<< "The case of the command and modifier characters is not important. Running the utility without parameters displays" << std::endl
						<< "this help. Violation of the number, order of input or nature of the arguments will display corresponding error." << std::endl << std::endl
						<< "* Examples:" << std::endl
						<< "SolFile -s EXAMPLE.SOL              The program will display all information about archive and will wait for any" << std::endl
						<< "                                    button to be pressed." << std::endl
						<< "SolFile -ui EXAMPLE.SOL             The utility will unpack the specified archive and will not output table of contents" << std::endl
						<< "                                    to console." << std::endl
						<< "SolFile -ep EXAMPLE.SOL 0           The first entry will be extracted (numbering starts from zero). The program will" << std::endl
						<< "                                    terminate immediately after the task is completed." << std::endl
						<< "SolFile -p EXAMPLE.LIST TARGET.SOL  EXAMPLE.LIST is used as source for new archive - TARGET.SOL. After completion of" << std::endl
						<< "                                    work, you can view information and manually close program." << std::endl
						<< "SolFile -ca EXAMPLE.SOL             Unpacking the archive and creating list-file. No information output and no pause at" << std::endl
						<< "                                    end of the work." << std::endl << std::endl;
			std::cout << std::endl << "Press any key to exit..." << std::endl;
			getch();
			return 0;
		}
		if(Path.at(0) != '-')
		{
			Path = chf::GetFullPath(Path.data());
			ifstream Check(Path.data(), ios::in);
			if(!Check.is_open())
			{
				std::cout << "Error. Can't open input file: " << Path << std::endl;
				if(!IgnorePause)
				{
					std::cout << std::endl << "Press any key to exit..." << std::endl;
					getch();
				}
				return 0;
			}
			Check.close();

			int ErrorFeedBack = Show(Path.data());

			std::cout << std::endl << "Process is over. Result: " << ErrorFeedBack;
			switch(ErrorFeedBack)
			{
				case 0:
					std::cout << "    (Operation completed successfully.)" << std::endl << std::endl;
				break;
				case 1:
					std::cout << "    (File processing error.)" << std::endl << std::endl;
				break;
				case 2:
					std::cout << "    (Argument parsing error.)" << std::endl << std::endl;
				break;
				case 3:
					std::cout << "    (Invalid modifier. Was ignored.)" << std::endl << std::endl;
				break;
			}
			if(ErrorFeedBack == 1)
			{
				std::cout << std::dec;
				std::cout << "Error Index: " << LastError.ErrorIndex << std::endl;
				std::cout << "Error Type : " << SolFileErrorDescription[LastError.ErrorIndex * 2 + 1] << std::endl;
				std::cout << "Description: " << SolFileErrorDescription[LastError.ErrorIndex * 2] << std::endl;
				std::cout << "Module name: " << LastError.File << std::endl;
				std::cout << "Function   : " << LastError.Function << std::endl;
				std::cout << "Line number: " << LastError.Line << std::endl;
				std::cout << "Additional : " << LastError.Additional << std::endl << std::endl;
			}
		}
        else
		{
			std::cout << "Error. Unknown single parameter: " << Path << std::endl;
			std::cout << std::endl << "Press any key to exit..." << std::endl;
			getch();
			return 0;
		}
	}
	else if(argc > 2 && argc < 5)
	{
		int ErrorFeedBack = 0;

		LastError.Function = nullptr;
		LastError.ErrorIndex = 0;
		LastError.Additional = nullptr;

		std::wstring wsp(argv[2]);
		std::string Path(wsp.begin(), wsp.end());
		Path = chf::GetFullPath(Path.data());
		ifstream Check(Path.data(), ios::in);
		if(!Check.is_open())
		{
			std::cout << "Error. Can't open input file: " << Path << std::endl;
			if(!IgnorePause)
			{
				std::cout << std::endl << "Press any key to exit..." << std::endl;
				getch();
			}
			return 0;
		}
		Check.close();

		const int CommandLenght = 4;
		char* Command = new char[CommandLenght];
		memset(Command, 0, CommandLenght);
		strncpy(Command, AnsiString(argv[1]).c_str(), CommandLenght);
		if(Command[0] != '-')
		{
			ErrorFeedBack = 2;
			std::cout << "Error. Invalid command - Can't find the dash character." << std::endl;
		}
		if(ErrorFeedBack != 2)
		{
			switch(tolower(Command[2]))
			{
				case 'a':
					IgnorePause = 1;
					IgnoreInfo = 1;
				break;
				case 'p':
					IgnorePause = 1;
				break;
				case 'i':
					IgnoreInfo = 1;
				break;
				case 0:
				break;
				default:
					InvalidMod = 1;
				break;
			}
			switch(tolower(Command[1]))
			{
				case 's':
					ErrorFeedBack = Show(Path.data());
				break;
				case 'u':
					ErrorFeedBack = Unpack(Path.data());
				break;
				case 'e':
					if(argc == 4)
					{
						cnv::unsint Value = static_cast<cnv::unsint>(_wtoi(argv[3]));
						ErrorFeedBack = Extract(Path.data(), Value);
					}
					else
					{
						ErrorFeedBack = 2;
						std::cout << "Error. Wrong number of arguments. Argc: " << argc << std::endl;
					}
				break;
				case 'l':
					ErrorFeedBack = List(Path.data());
				break;
				case 'p':
					if(argc == 4)
					{
						std::wstring wspa(argv[3]);
						std::string PathAdd(wspa.begin(), wspa.end());
						PathAdd = chf::GetFullPath(PathAdd.data());
						ErrorFeedBack = Pack(Path.data(), PathAdd.data());
					}
					else
					{
						ErrorFeedBack = 2;
						std::cout << "Error. Wrong number of arguments. Argc: " << argc << std::endl;
					}
				break;
				case 'c':
					ErrorFeedBack = Complex(Path.data());
				break;
				default:
					ErrorFeedBack = 2;
					std::cout << "Error. Invalid command: " << Command[1] << std::endl;
				break;
			}
			if(InvalidMod)
			{
				std::cout << "Error. Invalid modifier: " << Command[2] << std::endl;
				if(ErrorFeedBack == 0)
				{
					ErrorFeedBack = 3;
				}
			}
		}

		std::cout << std::endl << "Process is over. Result: " << ErrorFeedBack;
		switch(ErrorFeedBack)
		{
			case 0:
				std::cout << "    (Operation completed successfully.)" << std::endl << std::endl;
			break;
			case 1:
				std::cout << "    (File processing error.)" << std::endl << std::endl;
			break;
			case 2:
				std::cout << "    (Argument parsing error.)" << std::endl << std::endl;
			break;
			case 3:
				std::cout << "    (Invalid modifier. Was ignored.)" << std::endl << std::endl;
			break;
		}
		if(ErrorFeedBack == 1)
		{
            std::cout << std::dec;
			std::cout << "Error Index: " << LastError.ErrorIndex << std::endl;
			std::cout << "Error Type : " << SolFileErrorDescription[LastError.ErrorIndex * 2 + 1] << std::endl;
			std::cout << "Description: " << SolFileErrorDescription[LastError.ErrorIndex * 2] << std::endl;
			std::cout << "Module name: " << LastError.File << std::endl;
			std::cout << "Function   : " << LastError.Function << std::endl;
			std::cout << "Line number: " << LastError.Line << std::endl;
			std::cout << "Additional : " << LastError.Additional << std::endl << std::endl;
		}
		delete[] Command; Command = nullptr;
	}
	else if(argc == 1)
	{
		std::cout 	<< "SOLFILE    (build " << Version << " " << Build << " by SpinelDusk)" << std::endl << std::endl
					<< "SolFile is console utility for working with MechCommander game archives -- SOL, SAV, PKK, MPK formats." << std::endl << std::endl
					<< "* Syntax:" << std::endl
					<< "SolFile <command+modificator> <first parameter> <second parameter>" << std::endl << std::endl
					<< "* Operating modes:" << std::endl
					<< "Command        Symbol Mods Parameters                  Result" << std::endl
					<< "Show info      s       p   <Source arc.>               The program will show all contents of archive." << std::endl
					<< "Unpack all     u      api  <Source arc.>               Unpack contents of archive into a separate folder." << std::endl
					<< "Extract single e      api  <Source arc.> <EntryNumber> Single extraction of a file from archive." << std::endl
					<< "create List    l      api  <Source arc.>               Listing contents of archive in a list-file." << std::endl
					<< "Pack           p      api  <Source list> <Target arc.> Packing files into a new archive by list-file." << std::endl
					<< "Complex        c      api  <Source arc.>               Complex unpacking of archive simultaneously with creation of" << std::endl
					<< "                                                       list-file (Unpack all + create List)." << std::endl << std::endl
					<< "* Display modifiers:" << std::endl
					<< "Modifier       Symbol Result" << std::endl
					<< "ignore All     a      This modifier uses the following two at the same time." << std::endl
					<< "ignore Pause   p      Pause at end of the program will be ignored." << std::endl
					<< "ignore Info    i      All information output while the program is running will be skipped." << std::endl << std::endl
					<< "To view full help, use '-h' command." << std::endl;
	}
	else
	{
		std::cout << "Error. Wrong number of arguments. Argc: " << argc << std::endl;
	}
	if(!IgnorePause)
	{
		std::cout << std::endl << "Press any key to exit..." << std::endl; getch();
	}
	return 0;
}
