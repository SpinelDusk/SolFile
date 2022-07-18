/// \file
/// \brief Sol file module
/// \author SpinelDusk
/// \details Sol file processing class module

#pragma GCC diagnostic ignored "-Wc++98-compat"					//Disable warning: 'nullptr' is incompatible with C++98
#pragma GCC diagnostic ignored "-Wdisabled-macro-expansion"		//Disable warning: Disabled expansion of recursive macro
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"		//Disable warning: Extension used

#pragma package(smart_init)//Ensures that packaged units are initialized in the order determined by their dependencies

#include "SolFile.h"

extern ErrorData LastError;///< Defined in ErrorDescription.cpp
/// \brief Macro makes it easy to write SendAlert function in the code
#define SendAlert(a,b) SendAlert(__FILE__,__FUNCSIG__,__LINE__,a,1,b)

/// \brief Clearing instance fields (pack instance)
void SolPack::Clear()
{
	CheckSum = 0;
	AmountEntries = 0;
	DataStartOffset = 0;
	ArchivePath.erase(ArchivePath.begin(), ArchivePath.end());
	ListPath.erase(ListPath.begin(), ListPath.end());
	if(OpenList.is_open())
		OpenList.close();
	if(OpenReceiver.is_open())
		OpenReceiver.close();
	TOCList.erase(TOCList.begin(), TOCList.end());
}
/// \brief Sol-file packing control function
/// \param[in] local_Input Path to the list of files to be packed
/// \param[in] local_Output Path to the created archive
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::Pack(char* local_Input, char* local_Output)
{
	//Clearing the instance fields
	Clear();

	//Trying to open list-file
	OpenList.open(local_Input, ios::in);
	if(!OpenList.is_open())
	{
        OpenList.close();
		SendAlert(1, local_Input);//Can't open input file list.
		return 1;
	}

	//Check the file for emptiness
	if(!chf::GetFileSize(OpenList))
	{
		OpenList.close();
		SendAlert(2, local_Input);//The file is empty.
		return 1;
	}

	//Saving the path of the list-file
	ListPath = local_Input;

	//Line-by-line reading of the list-file
	if(ReadOpenList(OpenList))
	{
		OpenList.close();
		return 1;
	}

	//Create an output file, the future sol-archive
	OpenReceiver.open(local_Output, ios::out|ios::binary);
	if(!OpenReceiver.is_open())
	{
		OpenList.close();
		OpenReceiver.close();
		remove(local_Output);
		SendAlert(3, local_Output);//Can't open output file.
		return 1;
	}

	//Saving path of the archive being packed
	ArchivePath = local_Output;

	//Putting packed data to new archive
	if(FillData())
	{
		OpenList.close();
		OpenReceiver.close();
		remove(local_Output);
		return 1;
	}

	//Putting head data to new archive
	if(FillHead())
	{
		OpenList.close();
		OpenReceiver.close();
		remove(local_Output);
		return 1;		
	}

	OpenList.close();
	OpenReceiver.close();

	return 0;
}
/// \brief Reading the list-file containing paths to files to be packed
/// \param[in] List File stream for reading the list of files to be packed
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::ReadOpenList(ifstream& List)
{
	while(List)
	{
		char PackCommand;
		std::string TempPath;

		//Read line from the list
		getline(List, TempPath);

		//Ignore empty line
		if(TempPath.size() == 0)
		{
			continue;
		}

		//Checking for empty entry command
		if(TempPath.size() == 1)
		{
			if(TempPath.at(0) == 'e')//Add as empty entry
			{
				TOCList.push_back(Entry());
				TOCList.at(AmountEntries).CompressFlag = CF_EMPTY;
				TOCList.at(AmountEntries).Path = "";
				continue;
			}
			else//Unknown command
			{
				SendAlert(4, ListPath);//Instruction read error. Unknown command. Check if the command is entered correctly.
				return 1;
            }
		}

		//Instruction processing
		if(TempPath.at(1) == 0x20 && TempPath.size() > 2)
		{
			TOCList.push_back(Entry());												//Add new cell into TOCList vector
			PackCommand = static_cast<char>(tolower(TempPath.at(0)));				//Get command symbol
			TempPath.erase(0, 2);													//Erase 2 first symbols (command symbol + single space)
			ifstream TryToOpen(TempPath.data());									//Create stream to open the file from the list
			if(TryToOpen)
			{
				TryToOpen.close();
				switch(PackCommand)
				{
					case 'c':		//Add as compressed file
						TOCList.at(AmountEntries).CompressFlag = CF_COMPRESS;
						break;

					case 'u':		//Add as uncompressed file
						TOCList.at(AmountEntries).CompressFlag = CF_UNCOMPRESS;
						break;

					default:		//Unknown command
						SendAlert(5, ListPath);//Instruction read error. Unknown command. Check if the command is entered correctly.
						return 1;
				}
				TOCList.at(AmountEntries).Path = TempPath;
			}
			else
			{
				SendAlert(6, ListPath);//Instruction read error. Packing file not found.
				return 1;
            }
		}
		else
		{
			SendAlert(7, ListPath);//Instruction read error. Incorrect entry.
			return 1;
		}
		AmountEntries++;
	}
	List.close();
	if(AmountEntries == 0)
	{
		SendAlert(8, ListPath);//No entries were received.
		return 1;
	}
	return 0;
}
/// \brief Putting packed data to new archive
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::FillData()
{
	OpenReceiver.seekp((AmountEntries + 2) * 4, std::ios_base::beg);
	for(cnv::unsint i = 0; i < AmountEntries; i++)
	{
		switch(TOCList.at(i).CompressFlag)
		{
			case CF_COMPRESS:	//Add as compressed file
				if(AddCompressedFile(i))
					return 1;
				break;

			case CF_UNCOMPRESS:	//Add as uncompressed file
				if(AddUncompressedFile(i))
					return 1;
				break;

			case CF_EMPTY:		//Add as empty entry
				if(AddEmptedFile(i))
					return 1;
				break;
		}
	}
	return 0;
}
/// \brief Appending compressed file-entry to new archive
/// \param[in] Count Ordinal number of the file being packed
/// \return Error code: 0 - no errors, 1 - operation error
/// \bug Strange allocation bug. Currently fixed by fixing the allocation limit. But no guarantees.
int SolPack::AddCompressedFile(cnv::unsint Count)
{
	ifstream AddedFile;

	//Opening the file for packing into archive
	AddedFile.open(TOCList.at(Count).Path.data(), ios::in|ios::binary);
	if(!AddedFile.is_open())
	{
		SendAlert(9, TOCList.at(Count).Path);//Can't open file.
		return 1;
	}

	//Save the size of packed file
	cnv::dword AddedFileSize = chf::GetFileSize(AddedFile);

	//Checking the size for exceeding the limit
	const cnv::dword SizeLimit = 1024*1024*50;//50 MB
	if(AddedFileSize > SizeLimit)
	{
		SendAlert(10, TOCList.at(Count).Path.data());//File limit exceeded. Files no larger than 50 MB are allowed.
		return 1;
		//This is a very strange memory allocation bug.
		//Some specific numbers lead to critical error - Access violation.
		//Now memory is allocated fixedly - only 50 MB.
		//Alas, files over this limit cannot be packed yet.
		//Perhaps cause of the error is in the type of the variable.
	}

	std::vector<cnv::byte> CopyBuffer(SizeLimit);//std::vector<cnv::byte> CopyBuffer(AddedFileSize);

	//Reading content from the file being packed into CopyBuffer
	StreamRead(AddedFile, reinterpret_cast<char*>(CopyBuffer.data()), static_cast<int>(AddedFileSize));
	AddedFile.close();

	//Getting compressed data
	//Since almost all data is text, we don't need to worry about data growth instead of compression, but it's technically possible
	std::vector<cnv::byte> CompressedBuffer(SizeLimit);//std::vector<cnv::byte> CompressedBuffer(AddedFileSize);
	cnv::dword CompressedSize = static_cast<cnv::dword>(LZCompress(CompressedBuffer.data(), CopyBuffer.data(), AddedFileSize));

	//Saving offset and compression flag into receive file
	std::streamoff FtellRespond = OpenReceiver.tellp();
	if(FtellRespond != -1)
	{
		TOCList.at(Count).DataOffset = static_cast<cnv::dword>(FtellRespond);
	}
	else
	{
		SendAlert(11, ArchivePath);//Invalid position indicator value was received in the archive file.
		return 1;
	}

	//Compressed files have the uncompressed file size stored in the first DWORD of the file data
	for(int j = 0; j < 4; j++)
	{
		OpenReceiver << static_cast<cnv::byte>(AddedFileSize >> 8 * j);
	}

	//Writing resulting data for packing into receive file
	OpenReceiver.write(reinterpret_cast<const char*>(CompressedBuffer.data()), CompressedSize);

	return 0;
}
/// \brief Appending uncompressed file-entry to new archive
/// \param[in] Count Ordinal number of the file being packed
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::AddUncompressedFile(cnv::unsint Count)
{
	ifstream AddedFile;

	//Opening the file for packing into archive
	AddedFile.open(TOCList.at(Count).Path.data(), ios::in|ios::binary);
	if(!AddedFile.is_open())
	{
		SendAlert(12, TOCList.at(Count).Path);//Can't open file.
		return 1;
	}

	//Save the size of packed file
	cnv::dword AddedFileSize = chf::GetFileSize(AddedFile);

	std::vector<cnv::byte> CopyBuffer(AddedFileSize);

	//Reading content from the file being packed into CopyBuffer
	StreamRead(AddedFile, reinterpret_cast<char*>(CopyBuffer.data()), static_cast<int>(AddedFileSize));
	AddedFile.close();

	//Saving offset and compression flag into receive file
	std::streamoff FtellRespond = OpenReceiver.tellp();
	if(FtellRespond != -1)
	{
		TOCList.at(Count).DataOffset = static_cast<cnv::dword>(FtellRespond);
	}
	else
	{
		SendAlert(13, ArchivePath);//Invalid position indicator value was received in the archive file.
		return 1;
	}

	//Writing resulting data for packing into receive file
	OpenReceiver.write(reinterpret_cast<const char*>(CopyBuffer.data()), AddedFileSize);

	return 0;
}
/// \brief Appending empty file-entry to new archive
/// \param[in] Count Ordinal number of the file being packed
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::AddEmptedFile(cnv::unsint Count)
{
	//Saving offset and compression flag into receive file
	std::streamoff FtellRespond = OpenReceiver.tellp();
	if(FtellRespond != -1)
	{
		TOCList.at(Count).DataOffset = static_cast<cnv::dword>(FtellRespond);
	}
	else
	{
		SendAlert(14, ArchivePath);//Invalid position indicator value was received in the archive file.
		return 1;
	}

	return 0;
}
/// \brief Putting head data to new archive
/// \return Error code: 0 - no errors, 1 - operation error
int SolPack::FillHead()
{
	//Temporary variable for Header checksum
	cnv::dword HeaderCheckSum = 0;

	//Put the write cursor at beginning of the file (without checksum)
	OpenReceiver.seekp(4, std::ios_base::beg);

	//Calculate where the real data will begin in the archive
	DataStartOffset = (AmountEntries * 4) + 8;
	for(int j = 0; j < 4; j++)
	{
		OpenReceiver << static_cast<cnv::byte>(DataStartOffset >> 8 * j);
		HeaderCheckSum += static_cast<cnv::byte>(DataStartOffset >> 8 * j);
	}

	//Adjust table of contents offsets, mark them as compressed or empty as needed
	for(cnv::dword i = 0; i < AmountEntries; i++)
	{
		if     (TOCList.at(i).CompressFlag == CF_COMPRESS)
			TOCList.at(i).DataOffset |= 0x40000000;
		else if(TOCList.at(i).CompressFlag == CF_EMPTY)
			TOCList.at(i).DataOffset |= 0xE0000000;
		for(int j = 0; j < 4; j++)
		{
			OpenReceiver << static_cast<cnv::byte>(TOCList.at(i).DataOffset >> 8 * j);
			HeaderCheckSum += static_cast<cnv::byte>(TOCList.at(i).DataOffset >> 8 * j);
		}
	}

	//Calculate the checksum
	if(CalcCheckSum(ArchivePath.data(), CheckSum))
	{
		return 1;
	}
	CheckSum += HeaderCheckSum;

	//Put the write cursor at beginning of the file
	OpenReceiver.seekp(0, std::ios_base::beg);

	//Write checksum to the file
	for(int j = 0; j < 4; j++)
	{
		OpenReceiver << static_cast<cnv::byte>(CheckSum >> 8 * j);
	}
	return 0;
}


/// \brief Clearing instance fields (unpack instance)
void SolUnpack::Clear()
{
    CheckSum = 0;
	DataStartOffset = 0;
	AmountEntries = 0;
	FileSize = 0;
	TOCList.erase(TOCList.begin(), TOCList.end());

	ReadBuffer.erase(ReadBuffer.begin(), ReadBuffer.end());
	DecompBuffer.erase(DecompBuffer.begin(), DecompBuffer.end());
	UncompressedSize = 0;

	if(OpenArchive.is_open())
		OpenArchive.close();
	if(OpenReceiver.is_open())
		OpenArchive.close();
	ArchivePath.erase(ArchivePath.begin(), ArchivePath.end());
}
/// \brief Loading an archive to the program memory
/// \param[in] local_Input Path to the file to unpack
/// \return Error code: 0 - no errors, 1 - operation error
int SolUnpack::Load(char* local_Input)
{
	//Clearing the instance fields
	Clear();

	//Trying to open the archive
	OpenArchive.open(local_Input, ios::in|ios::binary);
	if(!OpenArchive.is_open())
	{
		OpenArchive.close();
		SendAlert(15, local_Input);//Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)
		return 1;
	}

	//Saving path of the archive
	ArchivePath = local_Input;

	//Get the file size
	FileSize = chf::GetFileSize(OpenArchive);
	if(FileSize == 0)
	{
		OpenArchive.close();
		SendAlert(16, local_Input);//The file is empty.
		return 1;
	}

	//Get file checksum
	if(DwordRead(OpenArchive, CheckSum))
	{
		OpenArchive.close();
		return 1;
	}
	if(ComparingCheckSums(local_Input, CheckSum))
	{
		OpenArchive.close();
		return 1;
    }

	//Get number of entries in the archive
	if(DwordRead(OpenArchive, DataStartOffset))
	{
		OpenArchive.close();
		return 1;
	}
	AmountEntries = ((DataStartOffset) - 8) / 4;

	//Resize table of contents array
	TOCList.resize(AmountEntries);

	//Read offsets from the file
   	for(cnv::dword i = 0; i < AmountEntries; i++)
	{
		if(DwordRead(OpenArchive, TOCList.at(i).DataOffset))
		{
			OpenArchive.close();
			return 1;
		}
	}

 	//Set up the array of flags and the array of offsets (while removing the flag from the offset value)
	for(cnv::dword i = 0; i < AmountEntries; i++)
	{
		if(TOCList.at(i).DataOffset >> ((8 * 3) + 4) == 0x4)//Read 4 high bits
		{
			TOCList.at(i).DataOffset <<= 4;//Need to remove this flag in order for the offsets to be correct
			TOCList.at(i).DataOffset >>= 4;
			TOCList.at(i).CompressFlag = CF_COMPRESS;//File is compressed
		}
		else if(TOCList.at(i).DataOffset >> ((8 * 3) + 4) == 0xE)//Read 4 high bits
		{
			TOCList.at(i).DataOffset <<= 4;//Need to remove this flag in order for the offsets to be correct
			TOCList.at(i).DataOffset >>= 4;
			TOCList.at(i).CompressFlag = CF_EMPTY;//Entry is empty
		}
		else
			TOCList.at(i).CompressFlag = CF_UNCOMPRESS;//File is uncompressed
	}

	//Calculating file-entry sizes
	for(cnv::dword i = 0; i < AmountEntries - 1; i++)
	{
		TOCList.at(i).CompressSize = TOCList.at(i + 1).DataOffset - TOCList.at(i).DataOffset;
	}
	TOCList.at(AmountEntries - 1).CompressSize = FileSize - TOCList.at(AmountEntries - 1).DataOffset;

	for(cnv::dword i = 0; i < AmountEntries; i++)
	{
		//Now let's try to extract entry
		ReadBuffer.resize(TOCList.at(i).CompressSize);
		OpenArchive.seekg(TOCList.at(i).DataOffset, std::ios_base::beg);
		StreamRead(OpenArchive, reinterpret_cast<char*>(ReadBuffer.data()), static_cast<int>(TOCList.at(i).CompressSize));

		//Set value of uncompressed file size for compressed data. And get file type
		if(TOCList.at(i).CompressFlag == CF_COMPRESS)
		{
			//The data is compressed, so the first DWORD is uncompressed size of the file, the remaining bytes are compressed data for the file-entry
			UncompressedSize = 0;
			for(cnv::unsint j = 0; j < 4; j++)
			{
				UncompressedSize += static_cast<cnv::dword>(ReadBuffer.at(j)) * pow(0x100, j);
			}
			ReadBuffer.erase(ReadBuffer.begin(), ReadBuffer.begin() + 4);
			DecompBuffer.resize(UncompressedSize + 16);//Adding some space just in case

			TOCList.at(i).UncompressSize = static_cast<cnv::dword>(LZDecompress(DecompBuffer.data(), ReadBuffer.data(), TOCList.at(i).CompressSize - 4));

			//Get file extension from file signature
			std::vector<cnv::byte> CutMagic(3);
			for(cnv::unsint j = 0; j < 3; j++)
			{
				CutMagic.at(j) = DecompBuffer.at(j);
			}
			TOCList.at(i).Extension = GetFileTypeFromSignature(CutMagic.data());
		}
		else if(TOCList.at(i).CompressFlag == CF_UNCOMPRESS)
		{
			//Get file extension from file signature
			std::vector<cnv::byte> CutMagic(3);
			for(cnv::unsint j = 0; j < 3; j++)
			{
				CutMagic.at(j) = ReadBuffer.at(j);
			}
			TOCList.at(i).Extension = GetFileTypeFromSignature(CutMagic.data());
			TOCList.at(i).UncompressSize = TOCList.at(i).CompressSize;
			TOCList.at(i).CompressSize = 0;
		}
		else if(TOCList.at(i).CompressFlag == CF_EMPTY)
		{
			TOCList.at(i).Extension = FM_NOTYPE;
		}
	}
	OpenArchive.close();
	return 0;
}
/// \brief Extracting a single file from the archive
/// \param[in] Count Ordinal number of the file being unpacked
/// \return Error code: 0 - no errors, 1 - operation error
int SolUnpack::ExtractFromArchive(cnv::unsint Count)
{
	//Checking for loading the archive into memory
	if(ArchivePath.empty())
	{
		SendAlert(17, nullptr);//Archive not loaded into memory.
		return 1;
	}

	//Checking for compliance with boundaries of the archive table of contents
	if(AmountEntries <= Count)
	{
		SendAlert(18, nullptr);//Violating boundaries of the table of contents (Attempted to read outside the list limits).
		return 1;
    }

	//Checking if the entry is empty
	if(TOCList.at(Count).CompressFlag == CF_EMPTY)
	{
		SendAlert(19, ArchivePath);//Attempt to unpack empty entry.
		return 1;
	}

	//Trying to open the archive
	OpenArchive.open(ArchivePath, ios::in|ios::binary);
	if(!OpenArchive.is_open())
	{
		SendAlert(20, ArchivePath);//Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)
		return 1;
	}

	//Formation of variables for generating folder and file names when unpacking
	std::string SymbolOfDisk; SymbolOfDisk.resize(MAX_PATH);
	std::string PathOfDirectory; PathOfDirectory.resize(MAX_PATH);
	std::string NameOfFile; NameOfFile.resize(MAX_PATH);
	std::string ExtensionOfFile; ExtensionOfFile.resize(MAX_PATH);
	std::string NameOfFileUpdated; NameOfFileUpdated.resize(MAX_PATH);
	chf::SplitPath(ArchivePath, SymbolOfDisk, PathOfDirectory, NameOfFile, ExtensionOfFile);

	NameOfFileUpdated = NameOfFile + ExtensionOfFile + "_unpack\\";

	std::string DirectoryForExtract; DirectoryForExtract.resize(MAX_PATH);

	//Getting file path assembly from parameters (DirectoryForExtract). This is the option when output folder is created
	_makepath(DirectoryForExtract.data(), SymbolOfDisk.data(), PathOfDirectory.data(), NameOfFileUpdated.data(), nullptr);
	chf::CreateDirectory(DirectoryForExtract);
	if(!(chf::IsDirectoryExists(DirectoryForExtract)))
	{
		OpenArchive.close();
		SendAlert(21, DirectoryForExtract);//Directory does not exist.
		return 1;
	}

	//Formation of path to output file
	std::string Receiver; Receiver.resize(MAX_PATH);
	Receiver = DirectoryForExtract.data();

	std::stringstream strs;
	strs << Count;
	std::string CharType;
	CharType = strs.str();
	Receiver += NameOfFile + ExtensionOfFile + "_" + CharType + "." + GetExtFromFileType(static_cast<cnv::unsint>(TOCList.at(Count).Extension));

	//Trying to open output file
	OpenReceiver.open(Receiver, ios::out|ios::binary);
	if(!OpenReceiver.is_open())
	{
		OpenArchive.close();
		SendAlert(22, Receiver);//The file can't be opened or does not exist.
		return 1;
    }

	//Trying to unpack the file-entry if it is compressed. After that, getting size of the unpacked file-entry
	if(TOCList.at(Count).CompressFlag)
	{
    	//Loading the required data segment from the archive
		ReadBuffer.resize(TOCList.at(Count).CompressSize);
		OpenArchive.seekg(TOCList.at(Count).DataOffset, std::ios_base::beg);
		StreamRead(OpenArchive, reinterpret_cast<char*>(ReadBuffer.data()), static_cast<int>(TOCList.at(Count).CompressSize));

		//The data is compressed, so the first DWORD is uncompressed size of the file, the remaining bytes are compressed data for the file-entry
		UncompressedSize = 0;
		for(cnv::unsint i = 0; i < 4; i++)
		{
			UncompressedSize += static_cast<cnv::dword>(ReadBuffer.at(i)) * pow(0x100, i);
		}
		ReadBuffer.erase(ReadBuffer.begin(), ReadBuffer.begin() + 4);
		DecompBuffer.resize(UncompressedSize + 16);//Adding some space just in case

		TOCList.at(Count).UncompressSize = static_cast<cnv::dword>(LZDecompress(DecompBuffer.data(), ReadBuffer.data(), TOCList.at(Count).CompressSize - 4));

		OpenReceiver.write(reinterpret_cast<const char*>(DecompBuffer.data()), TOCList.at(Count).UncompressSize);
	}
	else
	{
		//Loading the required data segment from the archive
		ReadBuffer.resize(TOCList.at(Count).UncompressSize);
		OpenArchive.seekg(TOCList.at(Count).DataOffset, std::ios_base::beg);
		StreamRead(OpenArchive, reinterpret_cast<char*>(ReadBuffer.data()), static_cast<int>(TOCList.at(Count).UncompressSize));

		OpenReceiver.write(reinterpret_cast<const char*>(ReadBuffer.data()), TOCList.at(Count).UncompressSize);
	}

	OpenReceiver.close();
	OpenArchive.close();

	if(chf::GetFileSize(Receiver) == 0)
	{
		SendAlert(23, Receiver);//The created file does not exist or has size of zero.
		return 1;
	}
	return 0;
}
/// \brief Unpacking entire contents of the archive
/// \return Error code: 0 - no errors, 1 - operation error
int SolUnpack::UnpackEntireArchive()
{
	for(cnv::dword i = 0; i < AmountEntries; i++)
	{
		if(TOCList.at(i).CompressFlag != 2)
		{
			if(ExtractFromArchive(i))
				return 1;
		}
	}
	return 0;
}
/// \brief Creating a list-file with contents of the archive
/// \return Error code: 0 - no errors, 1 - operation error
int SolUnpack::CreateListFile()
{
	//Checking for loading the archive into memory
	if(ArchivePath.empty())
	{
		SendAlert(24, nullptr);//Archive not loaded into memory.
		return 1;
	}

	//Formation of variables for generating folder and file names when unpacking
	std::string SymbolOfDisk; SymbolOfDisk.resize(MAX_PATH);
	std::string PathOfDirectory; PathOfDirectory.resize(MAX_PATH);
	std::string NameOfFile; NameOfFile.resize(MAX_PATH);
	std::string ExtensionOfFile; ExtensionOfFile.resize(MAX_PATH);
	std::string DirectoryForList; DirectoryForList.resize(MAX_PATH);
	chf::SplitPath(ArchivePath, SymbolOfDisk, PathOfDirectory, NameOfFile, ExtensionOfFile);

	//Create list-file of the same name in this folder
	std::string ListFileName; ListFileName.reserve(MAX_PATH);
	ListFileName = SymbolOfDisk + PathOfDirectory + NameOfFile + ExtensionOfFile + ".list";
	ofstream ListFromArchiveFile(ListFileName, ios::out);

	//Forming the list-file
	if(ListFromArchiveFile.is_open())
	{
		bool EndlCheck = false;
		for(cnv::unsint i = 0; i < AmountEntries; i++)
		{
			if(EndlCheck)
			{
				ListFromArchiveFile << std::endl;
			}
			else
			{
				EndlCheck = true;
			}
			if(TOCList.at(i).CompressFlag == CF_UNCOMPRESS)
			{
				ListFromArchiveFile << "u " << NameOfFile << ExtensionOfFile << "_unpack\\" << NameOfFile << ExtensionOfFile << "_" << i << "." << GetExtFromFileType(static_cast<cnv::unsint>(TOCList.at(i).Extension));
			}
			else if(TOCList.at(i).CompressFlag == CF_COMPRESS)
			{
				ListFromArchiveFile << "c " << NameOfFile << ExtensionOfFile << "_unpack\\" << NameOfFile << ExtensionOfFile << "_" << i << "." << GetExtFromFileType(static_cast<cnv::unsint>(TOCList.at(i).Extension));
			}
			else if(TOCList.at(i).CompressFlag == CF_EMPTY)
			{
				ListFromArchiveFile << "e";
			}
		}
		ListFromArchiveFile.close();
	}
	else
	{
		remove(ListFileName.data());
		SendAlert(25, ListFileName);//Attempt to create list failed. File does not exist or is locked. (Or the file path contains Unicode characters.)
		return 1;
	}
	return 0;
}


/// \brief Comparing file checksums
/// \param[in] local_Input Path to the file
/// \param[in] InputCheckSum CheckSum for comparing
/// \return Error code: 0 - no errors, 1 - operation error
int ComparingCheckSums(char* local_Input, cnv::dword InputCheckSum)
{
	cnv::dword OutputCheckSum = 0;
	if(CalcCheckSum(local_Input, OutputCheckSum))
	{
		return 1;
	}
	if(InputCheckSum == OutputCheckSum)
		return 0;
	else
	{
		SendAlert(26, local_Input);//The checksums don't match. The file is corrupted or is not an archive.
		return 1;
    }
}
/// \brief Calculate file checksum
/// \param[in] local_Input Path to the file
/// \param[out] OutputCheckSum DWORD variable for storing file checksum
/// \return Error code: 0 - no errors, 1 - operation error
int CalcCheckSum(char* local_Input, cnv::dword& OutputCheckSum)
{
	//Trying to open the archive
	ifstream OpenArchive;
	OpenArchive.open(local_Input, ios::in|ios::binary);
	if(!OpenArchive.is_open())
	{
		OpenArchive.close();
		SendAlert(27, local_Input);//Can't open input archive file. File does not exist or is locked. (Or the file path contains Unicode characters.)
		return 1;
	}

	//Calculate the checksum
	OpenArchive.seekg(4, std::ios_base::beg);
	OutputCheckSum = 0;
	char SingleCharacter;
	while(OpenArchive.get(SingleCharacter))
		OutputCheckSum += static_cast<cnv::dword>(static_cast<cnv::byte>(SingleCharacter));
	if(OpenArchive.eof())
	{
		OpenArchive.close();
		return 0;
	}
	else
	{
		OpenArchive.close();
		SendAlert(28, local_Input);//Unexpected end of file.
		return 1;
	}
}
/// \brief Read block of data from file as DWORD
/// \param[in] local_Input File stream from which DWORD data is read
/// \param[out] local_Output DWORD vector for storing processed data
/// \param[in] Amount Number of DWORDs to read from the file stream
/// \return Error code: 0 - no errors, 1 - operation error
int DwordRead(ifstream& local_Input, std::vector<cnv::dword>& local_Output, cnv::unsint Amount)
{
	if(local_Output.size() < Amount)
	{
		SendAlert(29, nullptr);//The requested read size exceeds the buffer allocated for storage.
		return 1;
	}
	else
	{
		std::vector<char> Buffer(4 * Amount);
		local_Input.read(Buffer.data(), 4 * Amount);
		if(!local_Input.good())
		{
			if(local_Input.eof())
			{
				SendAlert(30, nullptr);//The file data ended before data block was read. The file is corrupted.
				return 1;
			}
			else if(local_Input.fail())
			{
				SendAlert(31, nullptr);//ReadStream - Logical error on input operation.
				return 1;
			}
			else if(local_Input.bad())
			{
				SendAlert(32, nullptr);//ReadStream - Read error on input operation.
				return 1;
			}
		}
		for(cnv::dword j = 0; j < Amount; j++)
		{
			cnv::dword BufferDword = 0;
			for(cnv::dword i = 0; i < 4; i++)
			{
				BufferDword += static_cast<cnv::dword>(static_cast<cnv::byte>(Buffer.at(i + (j * 4)))) * pow(0x100, i);
			}
			local_Output.at(j) = BufferDword;
		}
	}
	return 0;
}
/// \brief Read block of data from file as DWORD
/// \param[in] local_Input File stream from which DWORD data is read
/// \param[out] local_Output DWORD variable for storing processed data
/// \return Error code: 0 - no errors, 1 - operation error
int DwordRead(ifstream& local_Input, cnv::dword& local_Output)
{
	std::vector<char> Buffer(4);
	local_Input.read(Buffer.data(), 4);
	if(!local_Input.good())
	{
		if(local_Input.eof())
		{
			SendAlert(33, nullptr);//The file data ended before data block was read. The file is corrupted.
			return 1;
		}
		else if(local_Input.fail())
		{
			SendAlert(34, nullptr);//ReadStream - Logical error on input operation.
			return 1;
		}
		else if(local_Input.bad())
		{
			SendAlert(35, nullptr);//ReadStream - Read error on input operation.
			return 1;
		}
	}
	cnv::dword BufferDword = 0;
	for(cnv::unsint i = 0; i < 4; i++)
	{
		BufferDword += static_cast<cnv::dword>(static_cast<cnv::byte>(Buffer.at(i))) * pow(0x100, i);
	}
	local_Output = BufferDword;
	return 0;
}
/// \brief Read block of data from file
/// \param[in] InputStream File stream
/// \param[out] Buffer Buffer for outputting read data
/// \param[in] ReadSize Read data size
/// \return Error code: 0 - no errors, 1 - operation error
int StreamRead(ifstream& InputStream, char* Buffer, int ReadSize)
{
	InputStream.read(Buffer, ReadSize);
	if(!InputStream.good())
	{
		if(InputStream.eof())
		{
			SendAlert(36, nullptr);//The file data ended before data block was read. The file is corrupted.
			return 1;
		}
		else if(InputStream.fail())
		{
			SendAlert(37, nullptr);//StreamRead - Logical error on input operation.
			return 1;
		}
		else if(InputStream.bad())
		{
			SendAlert(38, nullptr);//StreamRead - Read error on input operation.
			return 1;
		}
	}
	return 0;
}
