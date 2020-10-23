/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for MK22DX256xxx5
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v11.1.1 [Build 3241] [2020-03-02] on 23-ott-2020 16.19.01
 */

MEMORY
{
  /* Define each memory region */
  PROGRAM_FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x40000 /* 256K bytes (alias Flash) */  
  SRAM_UPPER (rwx) : ORIGIN = 0x20000000, LENGTH = 0x4000 /* 16K bytes (alias RAM) */  
  SRAM_LOWER (rwx) : ORIGIN = 0x1fffc000, LENGTH = 0x4000 /* 16K bytes (alias RAM2) */  
  FLEX_RAM (rwx) : ORIGIN = 0x14000000, LENGTH = 0x1000 /* 4K bytes (alias RAM3) */  
}

  /* Define a symbol for the top of each memory region */
  __base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_PROGRAM_FLASH = 0x0 + 0x40000 ; /* 256K bytes */  
  __top_Flash = 0x0 + 0x40000 ; /* 256K bytes */  
  __base_SRAM_UPPER = 0x20000000  ; /* SRAM_UPPER */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM_UPPER = 0x20000000 + 0x4000 ; /* 16K bytes */  
  __top_RAM = 0x20000000 + 0x4000 ; /* 16K bytes */  
  __base_SRAM_LOWER = 0x1fffc000  ; /* SRAM_LOWER */  
  __base_RAM2 = 0x1fffc000 ; /* RAM2 */  
  __top_SRAM_LOWER = 0x1fffc000 + 0x4000 ; /* 16K bytes */  
  __top_RAM2 = 0x1fffc000 + 0x4000 ; /* 16K bytes */  
  __base_FLEX_RAM = 0x14000000  ; /* FLEX_RAM */  
  __base_RAM3 = 0x14000000 ; /* RAM3 */  
  __top_FLEX_RAM = 0x14000000 + 0x1000 ; /* 4K bytes */  
  __top_RAM3 = 0x14000000 + 0x1000 ; /* 4K bytes */  
