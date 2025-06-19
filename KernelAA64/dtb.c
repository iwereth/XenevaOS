/**
* BSD 2-Clause License
*
* Copyright (c) 2022-2025, Manas Kamal Choudhury
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
**/

#include <dtb.h>
#include <aucon.h>
#include <_null.h>
#include <string.h>
#include <Mm/vmmngr.h>

#define FDT_MAGIC  0xd00dfeed
#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE   0x2
#define FDT_PROP       0x3
#define FDT_NOP        0x4
#define FDT_END        0x9

uint64_t* dtbAddress;

/*
 * AuDTBSwap32 -- swaps 32 bit value
 * @param from -- value to swap
 */
uint32_t AuDTBSwap32(uint32_t from) {
	uint8_t a = from >> 24;
	uint8_t b = from >> 16;
	uint8_t c = from >> 8;
	uint8_t d = from;
	return (d << 24) | (c << 16) | (b << 8) | a;
}

/*
 * AuDTBSwap64 -- swaps 64 bit value
 * @param from -- value to swap
 */
uint64_t AuDTBSwap64(uint64_t from) {
	uint8_t a = from >> 56;
	uint8_t b = from >> 48;
	uint8_t c = from >> 40;
	uint8_t d = from >> 32;
	uint8_t e = from >> 24;
	uint8_t f = from >> 16;
	uint8_t g = from >> 8;
	uint8_t h = from;
	return ((uint64_t)h << 56) | ((uint64_t)g << 48) | ((uint64_t)f << 40) |
		((uint64_t)e << 32) | (d << 24) | (c << 16) | (b << 8) | a;
}

/*
 * AuDTBSwap16 -- swaps 16 bit value
 * @param from -- value to swap
 */
uint16_t AuDTBSwap16(uint16_t from) {
	uint8_t a = from >> 8;
	uint8_t b = from;
	return (b << 8) | a;
}

uint32_t* AuDeviceTreeGetSubNode(uint32_t* node, char* strings, const char* name, uint32_t** node_out,
	int (*cmp)(const char* a, const char* b)) {
	while (AuDTBSwap32(*node) == 4)node++;
	if (AuDTBSwap32(*node) == 9) return NULL;
	if (AuDTBSwap32(*node) != 1) return NULL;
	node++;

	/*char* nodename = (char*)node;
	AuTextOut("NODE FOUND : %s \n", nodename);*/
	if (cmp((char*)node, name)) {
		*node_out = node;
		return NULL;
	}

	while ((*node & 0xFF000000) && (*node & 0xFF0000) && (*node & 0xFF00) && (*node & 0xFF))node++;
	node++;
	while (1) {
		while (AuDTBSwap32(*node) == 4) node++;
		if (AuDTBSwap32(*node) == 2) return node + 1;
		if (AuDTBSwap32(*node) == 3) {
			uint32_t len = AuDTBSwap32(node[1]);
			node += 3;
			node += (len + 3) / 4;
		}
		else if (AuDTBSwap32(*node) == 1) {
			node = AuDeviceTreeGetSubNode(node, strings, name, node_out, cmp);
			if (!node) return NULL;
		}
	}
}

uint32_t* AuDeviceTreeFindPropertyInternal(uint32_t* node, char* strings, const char* property,
	uint32_t** out) {
	while ((*node & 0xFF000000) && (*node & 0xFF0000) && (*node & 0xFF00) && (*node & 0xFF))node++;
	node++;

	while (1) {
		while (AuDTBSwap32(*node) == 4) node++;
		if (AuDTBSwap32(*node) == 2) return node + 1;
		if (AuDTBSwap32(*node) == 3) {
			uint32_t len = AuDTBSwap32(node[1]);
			uint32_t nameoff = AuDTBSwap32(node[2]);
			if (!strcmp(strings + nameoff, property)) {
				*out = &node[1];
				return NULL;
			}
			node += 3;
			node += (len + 3) / 4;

		}
		else if (AuDTBSwap32(*node) == 1) {
			node = AuDeviceTreeFindPropertyInternal(node + 1, strings, property, out);
			if (!node) return NULL;
		}
	}
}

static int prefix_cmp(const char* a, const char* b) {
	return !memcmp(a, b, strlen(b));
}

/*
 * AuDeviceTreeGetNode -- searches and get the offset of the node
 * @param name -- Node name
 */
uint32_t* AuDeviceTreeGetNode(const char* name) {
	if (!dtbAddress)
		return 0;
	fdt_header_t* fdt = (fdt_header_t*)dtbAddress;
	char* dtb_strings = (char*)((uint64_t)dtbAddress + AuDTBSwap32(fdt->off_dt_strings));
	uint32_t* dtb_struct = (uint32_t*)((uint64_t)dtbAddress + AuDTBSwap32(fdt->off_dt_struct));
	uint32_t* out = NULL;
	AuDeviceTreeGetSubNode(dtb_struct, dtb_strings, name, &out, prefix_cmp);
	return out;
}


/*
 * AuDeviceTreeFindProperty -- searches for a property within a node
 * @param node -- Pointer to node offset
 * @param property -- Property name
 */
uint32_t* AuDeviceTreeFindProperty(uint32_t* node, const char* property) {
	if (!dtbAddress)
		return NULL;
	fdt_header_t* fdt = (fdt_header_t*)dtbAddress;
	char* dtb_strings = (char*)((uint64_t)dtbAddress + AuDTBSwap32(fdt->off_dt_strings));
	uint32_t* out = NULL;
	AuDeviceTreeFindPropertyInternal(node, dtb_strings, property, &out);
	return out;
}
/*
 * AuDeviceTreeInitialize -- initialize the device tree
 * @param fdt_address -- device tree address passed by
 * bootloader
 */
void AuDeviceTreeInitialize(KERNEL_BOOT_INFO* info) {
	if (info->boot_type != BOOT_LITTLEBOOT_ARM64)
		return;
	AuLittleBootProtocol* lb = (AuLittleBootProtocol*)info->driver_entry1;
	if (!lb)
		return;
	void* fdt_address = lb->device_tree_base;

	if (!fdt_address) {
		AuTextOut("Device Tree Blob not found \n");
		return;
	}
	fdt_header_t* dtb = (fdt_header_t*)fdt_address;
	if (AuDTBSwap32(dtb->magic) != FDT_MAGIC) {
		AuTextOut("[Aurora]:Device Tree invalid magic \n");
		return;
	}
	AuTextOut("DTB Magic : %x \n", AuDTBSwap32(dtb->magic));
	dtbAddress = fdt_address;
}

/*
 * AuDeviceTreeMapMMIO -- Maps the physical device tree address
 * to MMIO address range
 */
void AuDeviceTreeMapMMIO() {
	if (!dtbAddress)
		return;
	fdt_header_t* fdt = (fdt_header_t*)dtbAddress;
	int pageCount = AuDTBSwap32(fdt->totalSize) / 0x1000;
	void* mmio = AuMapMMIO((uint64_t)dtbAddress, pageCount);
	dtbAddress = mmio;
	AuTextOut("[aurora]: Device tree mmio %x mapped with page count : %d \n",mmio, pageCount);
}