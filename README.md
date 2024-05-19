# TinyBoot
A tiny UEFI bootloader (Made without GNU-EFI or EDK-II)

It is in an extremely early stage and only supports loading a hardcoded Windows EFI Image using the EFI_IMAGE_START interface.

Planned Features:
 - Support for reading configuration options from a text file
 - Windows loading via EFI_IMAGE_START as well as custom built PE32+ loading code
 - Linux EFISTUB loading via both the above ways
 - Graphical User Interface
 - Linux non-EFISTUb loading
 - Loading ELF executables

I am building this project as a learning exercise to increase my understanding of C, UEFI and Linux

License is GPLv3
