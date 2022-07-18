## SolFile
#### by SpinelDusk
---
#### Summary
SolFile is console utility for working with MechCommander game archives -- SOL format.
The program supports: archive viewing, selective extraction, full archive unpacking, creation of a list of contents, and packing into archive by list-file.

#### Syntax
The program supports the commands for selecting the operating mode, modifiers for editing display mode, and parameters will for file paths involved in operations.
Commands and modifiers are written together. Use of space is not allowed and be interpreted by the program as invalid parameter. The command must be preceded by a dash character.
It is possible to open a file for viewing by adding path to the file as first parameter.

```
SolFile <command+modificator> <first parameter> <second parameter>
```

#### Operating modes
|Command            |Symbol |Mods Parameters                    |Result|
|-------------------|-------|-----------------------------------|------|
|**S**how info      |s      |p      <Source arc.>               |The program will show all contents of archive.|
|**U**npack all     |u      |api    <Source arc.>               |Unpack contents of archive into a separate folder.|
|**E**xtract single |e      |api    <Source arc.> <EntryNumber> |Single extraction of a file from archive.|
|create **L**ist    |l      |api    <Source arc.>               |Listing contents of archive in a list-file.|
|**P**ack           |p      |api    <Source list> <Target arc.> |Packing files into a new archive by list-file.|
|**C**omplex        |c      |api    <Source arc.>               |Complex unpacking of archive simultaneously with creation of list-file (Unpack all + create List).|

#### Display modifiers
|Modifier           |Symbol |Result|
|-------------------|-------|------|
|ignore **A**ll     |a      |This modifier uses the following two at the same time. (Cannot be used with 'Show info' mode).|
|ignore **P**ause   |p      |Pause at end of the program will be ignored, and after task is completed, the utility will terminate its work. This parameter is useful for batch operation of the utility.|
|ignore **I**nfo    |i      |All information output while the program is running will be skipped, only the exit message (and errors) will be shown. This modifier does not work with 'Show info' mode.|

Absence of modifiers will display all information available for the mode and the program will wait for any key to be pressed after end of work for its completion.

The case of the command and modifier characters is not important. Running the utility without parameters displays this help. Violation of the number, order of input or nature of the arguments will display corresponding error.

#### Examples
|Command                            |Description|
|-----------------------------------|-----------|
|SolFile -s EXAMPLE.SOL             |The program will display all information about archive and will wait for any button to be pressed.|
|SolFile -ui EXAMPLE.SOL            |The utility will unpack the specified archive and will not output table of contents to console.|
|SolFile -ep EXAMPLE.SOL 0          |The first entry will be extracted (numbering starts from zero). The program will terminate immediately after the task is completed.|
|SolFile -p EXAMPLE.LIST TARGET.SOL |EXAMPLE.LIST is used as source for new archive - TARGET.SOL. After completion of work, you can view information and manually close program.|
|SolFile -ca EXAMPLE.SOL            |Unpacking the archive and creating list-file. No information output and no pause at end of the work.|

#### Dependencies and requirements
- OS: any Windows OS from XP to 11.
- Compilation requires Embarcadero C++ Builder, preferably version 11.0 Alexandria (28.0.42600.6491). Compilation of the program under alternative versions of this IDE is not guaranteed.
- The application requires the author's dynamic library 'LzSubroutine.dll' to work with compression and decompression functions (supplied with the project).
- Doxygen version 1.9.4 was used to generate documentation for the code.

#### License
The source code of the program is distributed under the MIT license. The author allows free distribution, copying, modification of the code and use in other projects, including proprietary ones, without notifying the author about it. The author also disclaims responsibility for the use of this source code and software based on it.