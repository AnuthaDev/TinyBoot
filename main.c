#include "efi.h"
#include "ulibc.h"
#include "pe.h"

EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cout = NULL; // Console output
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cin = NULL;		// Console input
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cerr = NULL; // Console output - stderr
EFI_BOOT_SERVICES *bs;												// Boot services
EFI_RUNTIME_SERVICES *rs;											// Runtime services
EFI_HANDLE image = NULL;											// Image handle

void init_global_variables(EFI_HANDLE handle, EFI_SYSTEM_TABLE *systable)
{
	cout = systable->ConOut;
	cin = systable->ConIn;
	cerr = cout;
	bs = systable->BootServices;
	rs = systable->RuntimeServices;
	image = handle;
}

EFI_INPUT_KEY get_key(void)
{
	EFI_EVENT events[1];
	EFI_INPUT_KEY key;

	key.ScanCode = 0;
	key.UnicodeChar = u'\0';

	events[0] = cin->WaitForKey;
	UINTN index = 0;
	bs->WaitForEvent(1, events, &index);

	if (index == 0)
		cin->ReadKeyStroke(cin, &key);

	return key;
}

// VOID *load_pe(VOID *pe_buffer) {
//     // Print PE Signature
//     UINT8 pe_sig_offset = 0x3C; // From PE file format
//     UINT32 pe_sig_pos = *(UINT32 *)((UINT8 *)pe_buffer + pe_sig_offset);
//     UINT8 *pe_sig = (UINT8 *)pe_buffer + pe_sig_pos;

//     printf(u"\r\nPE Signature: [%x][%x][%x][%x]\r\n",
//            (UINTN)pe_sig[0], (UINTN)pe_sig[1], (UINTN)pe_sig[2], (UINTN)pe_sig[3]);

//     // Print Coff File Header Info
//     PE_Coff_File_Header_64 *coff_hdr = (PE_Coff_File_Header_64 *)(pe_sig + 4);
//     printf(u"Coff File Header:\r\n");
//     printf(u"Machine: %x, # of sections: %u, Size of Opt Hdr: %x\r\n"
//            u"Characteristics: %x\r\n",
//            coff_hdr->Machine, coff_hdr->NumberOfSections, coff_hdr->SizeOfOptionalHeader,
//            coff_hdr->Characteristics);

//     // Validate some data
//     if (coff_hdr->Machine != 0x8664) {
//         eprintf(u"Error: Machine type not AMD64.\r\n");
//         get_key();
//         return NULL;
//     }

//     if (!(coff_hdr->Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {
//         eprintf(u"Error: File not an executable image.\r\n");
//         get_key();
//         return NULL;
//     }

//     // Print Optional header info
//     PE_Optional_Header_64 *opt_hdr =
//         (PE_Optional_Header_64 *)((UINT8 *)coff_hdr + sizeof(PE_Coff_File_Header_64));

//     printf(u"\r\nOptional Header:\r\n");
//     printf(u"Magic: %x, Entry Point: %x\r\n"
//            u"Sect Align: %x, File Align: %x, Size of Image: %x\r\n"
//            u"Subsystem: %x, DLL Characteristics: %x, ImageBase: %x\r\n",
//            opt_hdr->Magic, opt_hdr->AddressOfEntryPoint,
//            opt_hdr->SectionAlignment, opt_hdr->FileAlignment, opt_hdr->SizeOfImage,
//            (UINTN)opt_hdr->Subsystem, (UINTN)opt_hdr->DllCharacteristics, opt_hdr->ImageBase);

//     // Validate info
//     if (opt_hdr->Magic != 0x20B) {
//         eprintf(u"Error: File not a PE32+ file.\r\n");
//         get_key();
//         return NULL;
//     }

//     // if (!(opt_hdr->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE)) {
//     //     eprintf(u"Error: File not a PIE file.\r\n");
//     //     get_key();
//     //     return NULL;
//     // }

//     // Allocate buffer to load sections into
//     EFI_PHYSICAL_ADDRESS program_buffer = opt_hdr->ImageBase;
//     //EFI_STATUS status = bs->AllocatePool(EfiLoaderData, opt_hdr->SizeOfImage, &program_buffer);
//     EFI_STATUS status = bs->AllocatePages(AllocateAnyPages, EfiLoaderData, ((opt_hdr->SizeOfImage)/4096) + 1, &program_buffer);

//     if (EFI_ERROR(status)) {
//         eprintf(u"Error %x; Could not allocate memory for PE file.\r\n", status);
//         get_key();
//         return NULL;
//     }

//     // Initialize buffer to 0, which should also take care of needing to 0-pad sections between
//     //   Raw Data and Virtual Size
//     memset((VOID *)program_buffer, 0, opt_hdr->SizeOfImage);

//     // Print section header info
//     PE_Section_Header_64 *shdr =
//         (PE_Section_Header_64 *)((UINT8 *)opt_hdr + coff_hdr->SizeOfOptionalHeader);

//     printf(u"\r\nSection Headers:\r\n");
//     for (UINT16 i = 0; i < coff_hdr->NumberOfSections; i++, shdr++) {
//         printf(u"Name: ");
//         char *pos = (char *)&shdr->Name;
//         for (UINT8 j = 0; j < 8; j++) {
//             CHAR16 str[2];
//             str[0] = *pos;
//             str[1] = u'\0';
//             if (*pos == '\0') break;
//             printf(u"%s", str);
//             pos++;
//         }

//         printf(u" VSize: %x, Vaddr: %x, DataSize: %x, DataPtr: %x\r\n",
//                shdr->VirtualSize, shdr->VirtualAddress,
//                shdr->SizeOfRawData, shdr->PointerToRawData);
//     }

//     // Load sections into new buffer
//     shdr = (PE_Section_Header_64 *)((UINT8 *)opt_hdr + coff_hdr->SizeOfOptionalHeader);
//     for (UINT16 i = 0; i < coff_hdr->NumberOfSections; i++, shdr++) {
//         if (shdr->SizeOfRawData == 0) continue;

//         VOID *dst = (UINT8 *)program_buffer + shdr->VirtualAddress;
//         VOID *src = (UINT8 *)pe_buffer + shdr->PointerToRawData;
//         UINTN len = shdr->SizeOfRawData;
//         memcpy(dst, src, len);
//     }

//     // Return entry point
//     VOID *entry_point = (UINT8 *)program_buffer + opt_hdr->AddressOfEntryPoint;

//     return entry_point;
// }



EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	// Initialize global variables
	init_global_variables(ImageHandle, SystemTable);

	// Reset Console Inputs/Outputs
	cin->Reset(cin, FALSE);
	cout->Reset(cout, FALSE);
	cout->Reset(cerr, FALSE);

	cout->SetAttribute(cout, EFI_TEXT_ATTR(DEFAULT_FG_COLOR, DEFAULT_BG_COLOR));

	eprintf(u"Namaste World!\r\n");
	get_key();

	EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_LOADED_IMAGE_PROTOCOL *lip = NULL;
	bs->OpenProtocol(image, &lip_guid, (VOID **)&lip, image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

	EFI_GUID sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = NULL;
	bs->OpenProtocol(lip->DeviceHandle, &sfsp_guid, (VOID **)&sfsp, image, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

	EFI_FILE_PROTOCOL *rootdir = NULL;
	sfsp->OpenVolume(sfsp, &rootdir);

	EFI_FILE_PROTOCOL *kernel;
	CHAR16 path[] = u"efi\\boot\\bootmgfw.efi";
	rootdir->Open(rootdir, &kernel, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

	EFI_FILE_INFO file_info;
	EFI_GUID figuid = EFI_FILE_INFO_ID;
	UINTN size;

	size = sizeof(file_info);
	kernel->GetInfo(kernel, &figuid, &size, (VOID *)&file_info);

	printf(u"File %s size %u (%u on disk) bytes\r\n", file_info.FileName, file_info.FileSize, file_info.PhysicalSize);
	UINT8 *file_buf = NULL;
	bs->AllocatePool(EfiLoaderData, sizeof(UINT8) * file_info.FileSize, (VOID **)&file_buf);
	UINTN file_size = file_info.FileSize;
	kernel->Read(kernel, &file_size, file_buf);

	EFI_DEVICE_PATH_PROTOCOL *devpath = NULL;
	EFI_GUID devguid = EFI_DEVICE_PATH_PROTOCOL_GUID;
	EFI_STATUS status = bs->HandleProtocol(lip->DeviceHandle, &devguid, (VOID **)&devpath);

	if (status != EFI_SUCCESS)
	{
		eprintf(u"Unable to get Device Path\r\n");
		get_key();
	}

	EFI_HANDLE start_image = NULL;
	EFI_STATUS err = bs->LoadImage(FALSE, image, devpath, file_buf, file_info.FileSize, &start_image);

	if (err != EFI_SUCCESS){
		eprintf(u"Unable to load Image\r\n");
	}
	
	err = bs->StartImage(start_image, NULL, NULL);

	if (err != EFI_SUCCESS){
		eprintf(u"Unable to start Image\r\n");
	}

	// rootdir->Close(kernel);

	get_key();

	return 0;
}
