#include "pch.h"

static int CheckELFHeader(ElfEhdr* hdr)
{
    if (
        (hdr->e_ident[EI_MAG0] != 0x7f) ||
        (hdr->e_ident[EI_MAG1] != 'E') ||
        (hdr->e_ident[EI_MAG2] != 'L') ||
        (hdr->e_ident[EI_MAG3] != 'F'))
        return 0;

    if (hdr->e_ident[EI_CLASS] != ELFCLASS32)
        return 0;

    return 1;
}

static ElfAddr(*Elf_SwapAddr)(ElfAddr);
static ElfOff(*Elf_SwapOff)(ElfOff);
static ElfWord(*Elf_SwapWord)(ElfWord);
static ElfHalf(*Elf_SwapHalf)(ElfHalf);
static ElfSword(*Elf_SwapSword)(ElfSword);

static ElfAddr     Elf_NoSwapAddr(ElfAddr data) { return data; }
static ElfOff      Elf_NoSwapOff(ElfOff data) { return data; }
static ElfWord     Elf_NoSwapWord(ElfWord data) { return data; }
static ElfHalf     Elf_NoSwapHalf(ElfHalf data) { return data; }
static ElfSword    Elf_NoSwapSword(ElfSword data) { return data; }

static ElfWord     Elf_YesSwapWord(ElfWord data)
{
    unsigned char
        b1 = (unsigned char)(data) & 0xff,
        b2 = (unsigned char)(data >> 8) & 0xff,
        b3 = (unsigned char)(data >> 16) & 0xff,
        b4 = (unsigned char)(data >> 24) & 0xff;

    return
        ((ElfWord)b1 << 24) |
        ((ElfWord)b2 << 16) |
        ((ElfWord)b3 << 8) | b4;
}

static ElfAddr     Elf_YesSwapAddr(ElfAddr data)
{
    return (ElfAddr)Elf_YesSwapWord((ElfWord)data);
}

static ElfOff      Elf_YesSwapOff(ElfOff data)
{
    return (ElfOff)Elf_YesSwapWord((ElfWord)data);
}

static ElfHalf     Elf_YesSwapHalf(ElfHalf data)
{
    return ((data & 0xff) << 8) | ((data & 0xff00) >> 8);
}

static ElfSword    Elf_YesSwapSword(ElfSword data)
{
    return (ElfSword)Elf_YesSwapWord((ElfWord)data);
}

static void Elf_SwapInit(int is_little)
{
    if (is_little)
    {
        Elf_SwapAddr = Elf_NoSwapAddr;
        Elf_SwapOff = Elf_NoSwapOff;
        Elf_SwapWord = Elf_NoSwapWord;
        Elf_SwapHalf = Elf_NoSwapHalf;
        Elf_SwapSword = Elf_NoSwapSword;
    }
    else
    {
        Elf_SwapAddr = Elf_YesSwapAddr;
        Elf_SwapOff = Elf_YesSwapOff;
        Elf_SwapWord = Elf_YesSwapWord;
        Elf_SwapHalf = Elf_YesSwapHalf;
        Elf_SwapSword = Elf_YesSwapSword;
    }
}

bool LoadElfImage(uint8_t* image, size_t image_size)
{
    ElfEhdr     *hdr;
    ElfPhdr     *phdr;

    // check header
    hdr = (ElfEhdr*)image;
    if (CheckELFHeader(hdr) == 0)
    {
        return false;
    }

    Elf_SwapInit((hdr->e_ident[EI_DATA] == ELFDATA2LSB ? 1 : 0));

    // check file type (must be exec)
    if (Elf_SwapHalf(hdr->e_type) != ET_EXEC)
    {
        return false;
    }

    //
    // load all segments
    //

    size_t hoffs = Elf_SwapOff(hdr->e_phoff);
    for (int i = 0; i < Elf_SwapHalf(hdr->e_phnum); i++)
    {
        phdr = (ElfPhdr*)(image + hoffs);

        // load one segment
        {
            uint32_t vend, vaddr;
            uint32_t size;

            if (Elf_SwapWord(phdr->p_type) == PT_LOAD)
            {
                vaddr = Elf_SwapAddr(phdr->p_vaddr);

                size = Elf_SwapWord(phdr->p_filesz);
                if (size == 0) continue;

                vend = vaddr + size;

                size_t poffs = Elf_SwapOff(phdr->p_offset);

                for (uint32_t n = 0; n < size; n++)
                {
                    HWWriteByte(vaddr + n, image[poffs + n]);
                }
            }
        }

        hoffs += sizeof(ElfPhdr);
    }

    return true;
}

bool LoadElfFile(const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");
	if (!f)
		return false;

	fseek(f, 0, SEEK_END);
	auto size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* image = new uint8_t[size];

	fread(image, 1, size, f);
	fclose(f);

	auto res = LoadElfImage(image, size);
    if (!res)
        return false;

	return true;
}
