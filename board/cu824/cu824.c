/*
 * (C) Copyright 2001
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * (C) Copyright 2001-2006
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>

 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <mpc824x.h>
#include <asm/processor.h>
#include <pci.h>
#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

#define BOARD_REV_REG 0xFE80002B

int checkboard (void)
{
	char  revision = *(volatile char *)(BOARD_REV_REG);
	char  buf[32];

	puts ("Board: CU824 ");
	printf("Revision %d ", revision);
	printf("Local Bus at %s MHz\n", strmhz(buf, gd->bus_clk));

	return 0;
}

phys_size_t initdram(int board_type)
{
	long size;
	long new_bank0_end;
	long mear1;
	long emear1;

	size = get_ram_size(CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_MAX_RAM_SIZE);

	new_bank0_end = size - 1;
	mear1 = mpc824x_mpc107_getreg(MEAR1);
	emear1 = mpc824x_mpc107_getreg(EMEAR1);
	mear1 = (mear1  & 0xFFFFFF00) |
		((new_bank0_end & MICR_ADDR_MASK) >> MICR_ADDR_SHIFT);
	emear1 = (emear1 & 0xFFFFFF00) |
		((new_bank0_end & MICR_ADDR_MASK) >> MICR_EADDR_SHIFT);
	mpc824x_mpc107_setreg(MEAR1,  mear1);
	mpc824x_mpc107_setreg(EMEAR1, emear1);

	return (size);
}

/*
 * Initialize PCI Devices, report devices found.
 */
#ifndef CONFIG_PCI_PNP
static struct pci_config_table pci_sandpoint_config_table[] = {
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0x0f, PCI_ANY_ID,
	  pci_cfgfunc_config_device, { PCI_ENET0_IOADDR,
				       PCI_ENET0_MEMADDR,
				       PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER }},

	{ }
};
#endif

struct pci_controller hose = {
#ifndef CONFIG_PCI_PNP
	config_table: pci_sandpoint_config_table,
#endif
};

void pci_init_board(void)
{
	pci_mpc824x_init(&hose);
}

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}
