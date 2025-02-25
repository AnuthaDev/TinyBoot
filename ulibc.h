#ifndef ULIBC_H
#define ULIBC_H

#include "efi.h"
#include <stdarg.h>

extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cout; // Console output
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *cin;		// Console input
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *cerr; // Console output - stderr

#define DEFAULT_FG_COLOR EFI_LIGHTGRAY
#define DEFAULT_BG_COLOR EFI_BLACK

#define ERROR_FG_COLOR EFI_RED
#define ERROR_BG_COLOR EFI_BLACK

#define HIGHLIGHT_FG_COLOR EFI_BLACK
#define HIGHLIGHT_BG_COLOR EFI_CYAN

// ================================
// Print a number to stderr
// ================================
BOOLEAN eprint_number(UINTN number, UINT8 base, BOOLEAN is_signed)
{
	const CHAR16 *digits = u"0123456789ABCDEF";
	CHAR16 buffer[24]; // Hopefully enough for UINTN_MAX (UINT64_MAX) + sign character
	UINTN i = 0;
	BOOLEAN negative = FALSE;

	if (base > 16)
	{
		cerr->OutputString(cerr, u"Invalid base specified!\r\n");
		return FALSE; // Invalid base
	}

	// Only use and print negative numbers if decimal and signed True
	if (base == 10 && is_signed && (INTN)number < 0)
	{
		number = -(INTN)number; // Get absolute value of correct signed value to get digits to print
		negative = TRUE;
	}

	do
	{
		buffer[i++] = digits[number % base];
		number /= base;
	} while (number > 0);

	switch (base)
	{
	case 2:
		// Binary
		buffer[i++] = u'b';
		buffer[i++] = u'0';
		break;

	case 8:
		// Octal
		buffer[i++] = u'o';
		buffer[i++] = u'0';
		break;

	case 10:
		// Decimal
		if (negative)
			buffer[i++] = u'-';
		break;

	case 16:
		// Hexadecimal
		buffer[i++] = u'x';
		buffer[i++] = u'0';
		break;

	default:
		// Maybe invalid base, but we'll go with it (no special processing)
		break;
	}

	// NULL terminate string
	buffer[i--] = u'\0';

	// Reverse buffer before printing
	for (UINTN j = 0; j < i; j++, i--)
	{
		// Swap digits
		UINTN temp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = temp;
	}

	// Print number string
	cerr->OutputString(cerr, buffer);

	return TRUE;
}

// ================================
// Print a number to stdout
// ================================
BOOLEAN print_number(UINTN number, UINT8 base, BOOLEAN is_signed)
{
	const CHAR16 *digits = u"0123456789ABCDEF";
	CHAR16 buffer[24]; // Hopefully enough for UINTN_MAX (UINT64_MAX) + sign character
	UINTN i = 0;
	BOOLEAN negative = FALSE;

	if (base > 16)
	{
		cerr->OutputString(cerr, u"Invalid base specified!\r\n");
		return FALSE; // Invalid base
	}

	// Only use and print negative numbers if decimal and signed True
	if (base == 10 && is_signed && (INTN)number < 0)
	{
		number = -(INTN)number; // Get absolute value of correct signed value to get digits to print
		negative = TRUE;
	}

	do
	{
		buffer[i++] = digits[number % base];
		number /= base;
	} while (number > 0);

	switch (base)
	{
	case 2:
		// Binary
		buffer[i++] = u'b';
		buffer[i++] = u'0';
		break;

	case 8:
		// Octal
		buffer[i++] = u'o';
		buffer[i++] = u'0';
		break;

	case 10:
		// Decimal
		if (negative)
			buffer[i++] = u'-';
		break;

	case 16:
		// Hexadecimal
		buffer[i++] = u'x';
		buffer[i++] = u'0';
		break;

	default:
		// Maybe invalid base, but we'll go with it (no special processing)
		break;
	}

	// NULL terminate string
	buffer[i--] = u'\0';

	// Reverse buffer before printing
	for (UINTN j = 0; j < i; j++, i--)
	{
		// Swap digits
		UINTN temp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = temp;
	}

	// Print number string
	cout->OutputString(cout, buffer);

	return TRUE;
}

// ====================================
// Print formatted strings to stderr
// ====================================
bool eprintf(CHAR16 *fmt, ...)
{
	bool result = true;
	CHAR16 charstr[2]; // TODO: Replace initializing this with memset and use = { } initializer
	va_list args;

	va_start(args, fmt);

	// Initialize buffers
	charstr[0] = u'\0', charstr[1] = u'\0';
	cout->SetAttribute(cout, EFI_TEXT_ATTR(ERROR_FG_COLOR, ERROR_BG_COLOR));

	// Print formatted string values
	for (UINTN i = 0; fmt[i] != u'\0'; i++)
	{
		if (fmt[i] == u'%')
		{
			i++;

			// Grab next argument type from input args, and print it
			switch (fmt[i])
			{
			case u'c':
			{
				// Print CHAR16 value; printf("%c", char)
				charstr[0] = va_arg(args, int); // Compiler warning says to do this
				cerr->OutputString(cerr, charstr);
			}
			break;

			case u's':
			{
				// Print CHAR16 string; printf("%s", string)
				CHAR16 *string = va_arg(args, CHAR16 *);
				cerr->OutputString(cerr, string);
			}
			break;

			case u'd':
			{
				// Print INT32; printf("%d", number_int32)
				INT32 number = va_arg(args, INT32);
				eprint_number(number, 10, TRUE);
			}
			break;

			case u'u':
			{
				// Print UINT32; printf("%u", number_uint32)
				UINT32 number = va_arg(args, UINT32);
				eprint_number(number, 10, FALSE);
			}
			break;

			case u'b':
			{
				// Print UINTN as binary; printf("%b", number_uintn)
				UINTN number = va_arg(args, UINTN);
				eprint_number(number, 2, FALSE);
			}
			break;

			case u'o':
			{
				// Print UINTN as octal; printf("%o", number_uintn)
				UINTN number = va_arg(args, UINTN);
				eprint_number(number, 8, FALSE);
			}
			break;

			case u'x':
			{
				// Print hex UINTN; printf("%x", number_uintn)
				UINTN number = va_arg(args, UINTN);
				eprint_number(number, 16, FALSE);
			}
			break;

			default:
				cerr->OutputString(cerr, u"Invalid format specifier: %");
				charstr[0] = fmt[i];
				cerr->OutputString(cerr, charstr);
				cerr->OutputString(cerr, u"\r\n");
				result = false;
				goto end;
				break;
			}
		}
		else
		{
			// Not formatted string, print next character
			charstr[0] = fmt[i];
			cerr->OutputString(cerr, charstr);
		}
	}

end:
	va_end(args);
	cout->SetAttribute(cout, EFI_TEXT_ATTR(DEFAULT_FG_COLOR, DEFAULT_BG_COLOR));

	return result;
}

// ===================================
// Print formatted strings to stdout
// ===================================
bool printf(CHAR16 *fmt, ...)
{
	bool result = true;
	CHAR16 charstr[2]; // TODO: Replace initializing this with memset and use = { } initializer
	va_list args;

	va_start(args, fmt);

	// Initialize buffers
	charstr[0] = u'\0', charstr[1] = u'\0';

	// Print formatted string values
	for (UINTN i = 0; fmt[i] != u'\0'; i++)
	{
		if (fmt[i] == u'%')
		{
			i++;

			// Grab next argument type from input args, and print it
			switch (fmt[i])
			{
			case u'c':
			{
				// Print CHAR16 value; printf("%c", char)
				charstr[0] = va_arg(args, int); // Compiler warning says to do this
				cout->OutputString(cout, charstr);
			}
			break;

			case u's':
			{
				// Print CHAR16 string; printf("%s", string)
				CHAR16 *string = va_arg(args, CHAR16 *);
				cout->OutputString(cout, string);
			}
			break;

			case u'd':
			{
				// Print INT32; printf("%d", number_int32)
				INT32 number = va_arg(args, INT32);
				// print_int(number);
				print_number(number, 10, TRUE);
			}
			break;

			case u'x':
			{
				// Print hex UINTN; printf("%x", number_uintn)
				UINTN number = va_arg(args, UINTN);
				// print_hex(number);
				print_number(number, 16, FALSE);
			}
			break;

			case u'u':
			{
				// Print UINT32; printf("%u", number_uint32)
				UINT32 number = va_arg(args, UINT32);
				print_number(number, 10, FALSE);
			}
			break;

			case u'b':
			{
				// Print UINTN as binary; printf("%b", number_uintn)
				UINTN number = va_arg(args, UINTN);
				eprint_number(number, 2, FALSE);
			}
			break;

			case u'o':
			{
				// Print UINTN as octal; printf("%o", number_uintn)
				UINTN number = va_arg(args, UINTN);
				eprint_number(number, 8, FALSE);
			}
			break;

			default:
				cout->OutputString(cout, u"Invalid format specifier: %");
				charstr[0] = fmt[i];
				cout->OutputString(cout, charstr);
				cout->OutputString(cout, u"\r\n");
				result = false;
				goto end;
				break;
			}
		}
		else
		{
			// Not formatted string, print next character
			charstr[0] = fmt[i];
			cout->OutputString(cout, charstr);
		}
	}

end:
	va_end(args);

	return result;
}

// ================================
// CHAR16 strcpy:
//   Copy src string into dst
//   Returns dst
// ================================
CHAR16 *strcpy_u16(CHAR16 *dst, CHAR16 *src)
{
	if (!dst)
		return NULL;
	if (!src)
		return dst;

	CHAR16 *result = dst;
	while (*src)
		*dst++ = *src++;

	*dst = u'\0'; // Null terminate

	return result;
}

// ================================
// CHAR16 strncmp:
//   Compare 2 strings, each character, up to at most len bytes
//   Returns difference in strings at last point of comparison:
//   0 if strings are equal, <0 if s2 is greater, >0 if s1 is greater
// ================================
INTN strncmp_u16(CHAR16 *s1, CHAR16 *s2, UINTN len)
{
	if (len == 0)
		return 0;

	while (len > 0 && *s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
		len--;
	}

	return *s1 - *s2;
}

// ================================
// CHAR16 strrchr:
//   Return last occurrence of C in string
// ================================
CHAR16 *strrchr_u16(CHAR16 *str, CHAR16 c)
{
	CHAR16 *result = NULL;

	while (*str)
	{
		if (*str == c)
			result = str;
		str++;
	}

	return result;
}

// ================================
// CHAR16 strcat:
//   Concatenate src string onto the end of dst string, at
//   dst's original null terminator position.
//  Returns dst
// ================================
CHAR16 *strcat_u16(CHAR16 *dst, CHAR16 *src)
{
	CHAR16 *s = dst;

	while (*s)
		s++; // Go until null terminator

	while (*src)
		*s++ = *src++;

	*s = u'\0'; // I guess this isn't normal libc behavior? But seems better to null terminate

	return dst;
}

// ====================================
// memset for compiling with clang/gcc:
// Sets len bytes of dst memory with int c
// Returns dst buffer
// ================================
VOID *memset(VOID *dst, UINT8 c, UINTN len)
{
	UINT8 *p = dst;
	for (UINTN i = 0; i < len; i++)
		p[i] = c;

	return dst;
}

// ====================================
// memcpy for compiling with clang/gcc:
// Sets len bytes of dst memory from src.
// Assumes memory does not overlap!
// Returns dst buffer
// ================================
VOID *memcpy(VOID *dst, VOID *src, UINTN len)
{
	UINT8 *p = dst;
	UINT8 *q = src;
	for (UINTN i = 0; i < len; i++)
		p[i] = q[i];

	return dst;
}

// =============================================================================
// memcmp:
// Compare up to len bytes of m1 and m2, stop at first
//   point that they don't equal.
// Returns 0 if equal, >0 if m1 is greater than m2, <0 if m2 is greater than m1
// =============================================================================
INTN memcmp(VOID *m1, VOID *m2, UINTN len)
{
	UINT8 *p = m1;
	UINT8 *q = m2;
	for (UINTN i = 0; i < len; i++)
		if (p[i] != q[i])
			return (INTN)(p[i]) - (INTN)(q[i]);

	return 0;
}

// =====================================================================
// (ASCII) strlen:
// Returns: length of string not including NULL terminator
// =====================================================================
UINTN strlen(const char *s) {
    UINTN len = 0;
    while (*s) {
        len++;
        s++;
    }

    return len;
}

#endif
