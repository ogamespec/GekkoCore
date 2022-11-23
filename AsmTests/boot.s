.text

# It will actually be address 0xfff0'0000
.org 0

.byte "Dummy Gekko BootROM."

.align 256

.global boot

boot:
		lis         r4, 0x0011 
		addi        r4, r4, 0x0C64
		mtspr       HID0, r4

# This will initialize CPU program model.        
		lis         r4, 0x0000
		addi        r4, r4, 0x2000
		mtmsr       r4

# Enable data and instruction cache.
		mfspr       r3, HID0
		ori         r4, r3, 0xC000
		mtspr       HID0, r4
		nop
		nop
		nop
		isync

# Initialize CPU memory model. Clear BATs and segment registers.        
		li          r4, 0
		mtspr       DBAT0U, r4
		mtspr       DBAT1U, r4
		mtspr       DBAT2U, r4
		mtspr       DBAT3U, r4
		mtspr       IBAT0U, r4
		mtspr       IBAT1U, r4
		mtspr       IBAT2U, r4
		mtspr       IBAT3U, r4
		isync
		lis         r4, 0x8000
		addi        r4, r4, 0
		mtsr        0, r4
		mtsr        1, r4
		mtsr        2, r4
		mtsr        3, r4
		mtsr        4, r4
		mtsr        5, r4
		mtsr        6, r4
		mtsr        7, r4
		mtsr        8, r4
		mtsr        9, r4
		mtsr        10, r4
		mtsr        11, r4
		mtsr        12, r4
		mtsr        13, r4
		mtsr        14, r4
		mtsr        15, r4

# Configure memory model:
#
# DBAT0: 80001FFF 00000002    Write-back cached main memory, 256MB block.
# DBAT1: C0001FFF 0000002A    Write-through cached main memory, 256MB block.
# DBAT2: 00000000 xxxxxxxx    Dont care, reserved.
# DBAT3: FFF0001F FFF00001    Bootrom, 1MB block (temporary for BS)
#
# IBAT0: 80001FFF 00000002    Write-back cached main memory, 256MB block.
# IBAT1: 00000000 xxxxxxxx    Dont care, reserved.
# IBAT2: 00000000 xxxxxxxx    Dont care, reserved.
# IBAT3: FFF0001F FFF00001    Bootrom, 1MB block (temporary for BS)
		lis         r4, 0x0000
		addi        r4, r4, 2
		lis         r3, 0x8000
		addi        r3, r3, 0x1FFF
		mtspr       DBAT0L, r4
		mtspr       DBAT0U, r3
		isync
		mtspr       IBAT0L, r4
		mtspr       IBAT0U, r3
		isync
		lis         r4, 0x0000
		addi        r4, r4, 42
		lis         r3, 0xC000
		addi        r3, r3, 0x1FFF
		mtspr       DBAT1L, r4
		mtspr       DBAT1U, r3
		isync
		lis         r4, 0xFFF0
		addi        r4, r4, 1
		lis         r3, 0xFFF0
		addi        r3, r3, 31
		mtspr       DBAT3L, r4
		mtspr       DBAT3U, r3
		isync
		mtspr       IBAT3L, r4
		mtspr       IBAT3U, r3
		isync

# Enable instruction and data translation.
		mfmsr       r4
		ori         r4, r4, 0x0030      # Enable address translation.
		mtmsr       r4
		isync

		lis         r4, 0x8130
		ori         r4, r4, 0x0000
		mtlr        r4                  # LR = 0x81300000
		blr
