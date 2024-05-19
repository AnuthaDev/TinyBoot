// PE Structs/types
// PE32+ COFF File Header
typedef struct
{
	UINT16 Machine;					 // 0x8664 = x86_64
	UINT16 NumberOfSections; // # of sections to load for program
	UINT32 TimeDateStamp;
	UINT32 PointerToSymbolTable;
	UINT32 NumberOfSymbols;
	UINT16 SizeOfOptionalHeader;
	UINT16 Characteristics;
} __attribute__((packed)) PE_Coff_File_Header_64;

// COFF File Header Characteristics
typedef enum
{
	IMAGE_FILE_EXECUTABLE_IMAGE = 0x0002,
} PE_COFF_CHARACTERISTICS;

// PE32+ Optional Header
typedef struct
{
	UINT16 Magic; // 0x10B = PE32, 0x20B = PE32+
	UINT8 MajorLinkerVersion;
	UINT8 MinorLinkerVersion;
	UINT32 SizeOfCode;
	UINT32 SizeOfInitializedData;
	UINT32 SizeOfUninitializedData;
	UINT32 AddressOfEntryPoint; // Entry Point address (RVA from image base in memory)
	UINT32 BaseOfCode;
	UINT64 ImageBase;
	UINT32 SectionAlignment;
	UINT32 FileAlignment;
	UINT16 MajorOperatingSystemVersion;
	UINT16 MinorOperatingSystemVersion;
	UINT16 MajorImageVersion;
	UINT16 MinorImageVersion;
	UINT16 MajorSubsystemVersion;
	UINT16 MinorSubsystemVersion;
	UINT32 Win32VersionValue;
	UINT32 SizeOfImage; // Size in bytes of entire file (image) incl. headers
	UINT32 SizeOfHeaders;
	UINT32 CheckSum;
	UINT16 Subsystem;
	UINT16 DllCharacteristics;
	UINT64 SizeOfStackReserve;
	UINT64 SizeOfStackCommit;
	UINT64 SizeOfHeapReserve;
	UINT64 SizeOfHeapCommit;
	UINT32 LoaderFlags;
	UINT32 NumberOfRvaAndSizes;
} __attribute__((packed)) PE_Optional_Header_64;

// Optional Header DllCharacteristics
typedef enum
{
	IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE = 0x0040, // PIE executable
} PE_OPTIONAL_HEADER_DLLCHARACTERISTICS;

// PE32+ Section Headers - Immediately following the Optional Header
typedef struct
{
	UINT64 Name;						 // 8 byte, null padded UTF-8 string
	UINT32 VirtualSize;			 // Size in memory; If >SizeOfRawData, the difference is 0-padded
	UINT32 VirtualAddress;	 // RVA from image base in memory
	UINT32 SizeOfRawData;		 // Size of actual data (similar to ELF FileSize)
	UINT32 PointerToRawData; // Address of actual data
	UINT32 PointerToRelocations;
	UINT32 PointerToLinenumbers;
	UINT16 NumberOfRelocations;
	UINT16 NumberOfLinenumbers;
	UINT32 Characteristics;
} __attribute__((packed)) PE_Section_Header_64;