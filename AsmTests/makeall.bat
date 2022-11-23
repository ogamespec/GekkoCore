"%CWFOLDER%/PowerPC_EABI_TOOLS/Command_Line_Tools/mwasmeppc.exe" -list -proc gekko boot.s
"%CWFOLDER%/PowerPC_EABI_TOOLS/Command_Line_Tools/mwldeppc.exe" -codeaddr 0xfff00000 -o boot.elf boot.o

"%CWFOLDER%/PowerPC_EABI_TOOLS/Command_Line_Tools/mwasmeppc.exe" -list -proc gekko simple.s
"%CWFOLDER%/PowerPC_EABI_TOOLS/Command_Line_Tools/mwldeppc.exe" -codeaddr 0x81300000 -o simple.elf simple.o
