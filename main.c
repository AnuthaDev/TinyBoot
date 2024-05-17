#include "efi.h"
#include "ulibc.h"

EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cout = NULL;	// Console output
EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *cin  = NULL;	// Console input
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cerr = NULL;	// Console output - stderr
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

// ====================
// Get key from user
// ====================
EFI_INPUT_KEY get_key(void) {
    EFI_EVENT events[1];
    EFI_INPUT_KEY key;

    key.ScanCode = 0;
    key.UnicodeChar = u'\0';

    events[0] = cin->WaitForKey;
    UINTN index = 0;
    bs->WaitForEvent(1, events, &index);

    if (index == 0) cin->ReadKeyStroke(cin, &key);

    return key;
}


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

	return 0;
}
