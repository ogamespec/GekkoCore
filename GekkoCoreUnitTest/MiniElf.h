
#pragma once

// ELF format definitions (sufficient to load files of this format)

using ElfAddr = uint32_t;
using ElfOff = uint32_t;
using ElfHalf = uint16_t;
using ElfWord = uint32_t;
using ElfSword = int32_t;

struct ElfEhdr
{
    uint8_t e_ident[16];

    ElfHalf e_type;
    ElfHalf e_machine;
    ElfWord e_version;
    ElfAddr e_entry;
    ElfOff  e_phoff;

    ElfOff  e_shoff;
    ElfWord e_flags;
    ElfHalf e_ehsize;
    ElfHalf e_phentsize;
    ElfHalf e_phnum;
    ElfHalf e_shentsize;

    ElfHalf e_shnum;
    ElfHalf e_shstrndx;
};

struct ElfPhdr
{
    ElfWord p_type;
    ElfOff  p_offset;
    ElfAddr p_vaddr;
    ElfAddr p_paddr;
    ElfWord p_filesz;
    ElfWord p_memsz;
    ElfWord p_flags;
    ElfWord p_align;
};

enum ELF_IDENT
{
    EI_MAG0 = 0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_OSABI,
    EI_ABIVERSION,
    EI_PAD,
    EI_NIDENT = 16,
};

#define ELFCLASS32  1
#define ELFCLASS64  2

#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ET_NONE     0
#define ET_REL      1
#define ET_EXEC     2
#define ET_DYN      3
#define ET_CORE     4
#define ET_LOOS     0xfe00
#define ET_HIOS     0xfeff
#define ET_LOPROC   0xff00
#define ET_HIPROC   0xffff

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6
#define PT_LOOS     0x60000000
#define PT_HIOS     0x6fffffff
#define PT_LOPROC   0x70000000
#define PT_HIPROC   0x7fffffff

#define PF_X        0x1
#define PF_W        0x2
#define PF_R        0x4
#define PF_MASKOS   0x00ff0000
#define PF_MASKPROC 0xff000000
