// flash the forth binary

#include <stdio.h>
#include "stm32h7xx_hal.h"

#define FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)
#define FLASH_END_ADDR       (uint32_t)(0x081FFFFF)

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) /* Base @ of Sector 7, 128 Kbytes */

// flash from the file to the Start of FLASH BANK 1
int do_flash(FILE *fp)
{
	uint32_t FirstSector = 0, NbOfSectors = 0;
	uint32_t Address = 0, SECTORError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct= {0};

	HAL_FLASH_Unlock();

	// erase flash bank2 sector 6 and 7
	/* Get the 1st sector to erase */
	FirstSector = FLASH_SECTOR_6;
	NbOfSectors = 2;

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Banks         = FLASH_BANK_2;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;
	printf("DEBUG: forth flash: Sector erase of Bank2 Sectors 6 and 7\n");

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK) {
		return 0;
	}

	printf("DEBUG: forth flash: starting flash of file\n");

	// Program the user Flash area word by word
	uint64_t FlashWord[4];
	Address = ADDR_FLASH_SECTOR_6_BANK2;
	while (Address < ADDR_FLASH_SECTOR_7_BANK2) {
		/* Read a chunk of file */
		size_t rc = fread((void *)FlashWord, 1, sizeof(FlashWord), fp);
		if(rc == 0) break;

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, Address, ((uint32_t)FlashWord)) == HAL_OK) {
			//printf("DEBUG: flashed address: %p\n", (void*)Address);
			Address = Address + 32; /* increment for the next Flash word*/

		} else {
			printf("ERROR: forth flash: flash programming error\n");
			return 0;
		}
		if(rc < sizeof(FlashWord)) break; // hit EOF
	}

	/* -5- Lock the Flash to disable the flash control register access (recommended
	   to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	printf("DEBUG: forth flash: Flash completed\n\n");

#if 0
	/* -6- Check if the programmed data is OK
	    MemoryProgramStatus = 0: data programmed correctly
	    MemoryProgramStatus != 0: number of words not programmed correctly ******/
	__IO uint32_t MemoryProgramStatus = 0;
	__IO uint64_t data64 = 0;
	while (Address < FLASH_USER_END_ADDR) {
		for(Index = 0; Index < 4; Index++) {
			data64 = *(uint64_t*)Address;
			__DSB();
			if(data64 != FlashWord[Index]) {
				MemoryProgramStatus++;
			}
			Address += 8;
		}
	}

	/* -7- Check if there is an issue to program data*/
	if (MemoryProgramStatus == 0) {
		return 1;
	}
#endif
	return 1;
}

