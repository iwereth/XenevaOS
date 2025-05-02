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

#include "xhci.h"
#include "usb.h"
#include <Mm/vmmngr.h>
#include <aucon.h>
#include <Hal/serial.h>

/*
 * XHCIEvaluateContextCmd -- evaluate context command
 * @param dev -- Pointer to usb device
 * @param input_ctx_ptr -- input context physical address
 * @param slot_id -- slot number
 */
void XHCIEvaluateContextCmd(XHCIDevice* dev, uint64_t input_ctx_ptr, uint8_t slot_id) {
	XHCISendCmdToHost(dev, input_ctx_ptr & UINT32_MAX, (input_ctx_ptr & UINT32_MAX) >> 32, 0, (slot_id << 24) | (TRB_CMD_EVALUATE_CTX << 10));
	XHCIRingDoorbellHost(dev);
}


/*
 * XHIConfigureEndpoint -- configure the endpoint
 */
void XHCIConfigureEndpoint(XHCIDevice* dev, uint64_t input_ctx_ptr, uint8_t slot_id) {
	XHCISendCmdToHost(dev, input_ctx_ptr & UINT32_MAX, (input_ctx_ptr & UINT32_MAX) >> 32, 0, ((slot_id & 0xff) << 24) | (TRB_CMD_CONFIG_ENDPOINT << 10) | (0 << 9));
	XHCIRingDoorbellHost(dev);
}


/*
* XHCISendAddressDevice -- issues address device command
* @param dev -- pointer to usb device structure
* @param bsr -- block set address request (BSR)
* @param input_ctx_ptr -- address of input context
* @param slot_id -- slot id number
*/
void XHCISendAddressDevice(XHCIDevice* dev, XHCISlot* slot, uint8_t bsr, uint64_t input_ctx_ptr, uint8_t slot_id) {
	XHCISendCmdToHost(dev, input_ctx_ptr & UINT32_MAX, (input_ctx_ptr & UINT32_MAX) >> 32, 0, ((slot_id & 0xff) << 24) | (TRB_CMD_ADDRESS_DEV << 10) | (bsr << 9));
	XHCIRingDoorbellHost(dev);
}

/*
* XHCICreateSetupTRB -- creates a setup stage trb
*/
void XHCICreateSetupTRB(XHCISlot* slot, uint8_t rType, uint8_t bRequest, uint16_t value, uint16_t wIndex, uint16_t wLength, uint8_t trt) {
	XHCISendCmdDefaultEP(slot, rType | bRequest << 8 | value << 16, wIndex | wLength << 16, (0 << 22) | 8, (trt << 16) | (2 << 10) | (1 << 5) | (1 << 6));
}

/*
* XHCICreateDataTRB -- creates data stage trb
* @param dev -- pointer to usb structure
* @param buffer -- pointer to memory area
* @param size -- size of the buffer
* @param in_direction -- direction
*/
void XHCICreateDataTRB(XHCISlot* slot, uint64_t buffer, uint16_t size, bool in_direction) {
	uint8_t dir = 0;
	if (in_direction)
		dir = 1;
	else
		dir = 0;
	XHCISendCmdDefaultEP(slot, buffer & UINT32_MAX, (buffer >> 32) & UINT32_MAX, size, ((dir & 0x1f) << 16) | (3 << 10) | (1 << 1));
}

/*
* XHCICreateStatusTRB -- creates status stage trb
* @param dev -- pointer to usb strucutue
* @param in_direction -- direction
*/
void XHCICreateStatusTRB(XHCISlot* slot, bool in_direction) {
	uint8_t dir = 0;
	if (in_direction)
		dir = 1;
	else
		dir = 0;
	XHCISendCmdDefaultEP(slot, 0, 0, 0, ((dir & 0x1f) << 16) | (4 << 10) | (1 << 5) | (1 << 1));
}

/*
* XHCIEnableSlot -- sends enable slot command to xHC
* @param dev -- Pointer to usb device structure
* @param slot_type -- type of slot mainly in between 0-31
*/
void XHCIEnableSlot(XHCIDevice* dev, uint8_t slot_type) {
	/* Send Enable slot command */
	XHCISendCmdToHost(dev, 0, 0, 0, (TRB_CMD_ENABLE_SLOT << 10));
	XHCIRingDoorbellHost(dev);
}

/*
* XHCIDisableSlot -- sends disable slot command to xHC
* @param dev -- Pointer to usb device structure
* @param slot_num -- slot id to disable
*/
void XHCIDisableSlot(XHCIDevice* dev, uint8_t slot_num) {
	/* Send Enable slot command */
	XHCISendCmdToHost(dev, 0, 0, 0, (slot_num << 24) | (TRB_CMD_DISABLE_SLOT << 10));
	XHCIRingDoorbellHost(dev);
}


/* XHCISendNoopCmd -- Send No operation command
* @param dev -- Pointer to USB structure
*/
void XHCISendNoopCmd(XHCIDevice* dev) {
	XHCISendCmdToHost(dev, 0, 0, 0, TRB_CMD_NO_OP << 10);
	XHCIRingDoorbellHost(dev);
}

/*
* XHCISendControlCmd -- Sends control commands to USB Device's
* default control pipe
* @param dev -- pointer to usb device structure
* @param slot_id -- slot number
* @param request -- USB request packet structure
* @param buffer_addr -- input buffer address
* @param len -- length of the buffer
*/
void XHCISendControlCmd(XHCIDevice* dev, XHCISlot* slot, uint8_t slot_id, const USB_REQUEST_PACKET* request, uint64_t buffer_addr, const size_t len,
	uint8_t trt) {
	if (len == 0)
		trt = 0;
	bool data_required = true;
	bool _data_in = true;
	bool _status_in = true;

	switch (trt) {
	case CTL_TRANSFER_TRT_NO_DATA:
		data_required = false;
		break;
	case CTL_TRANSFER_TRT_OUT_DATA:
		data_required = true;
		_data_in = false;
		_status_in = true;
		break;
	case CTL_TRANSFER_TRT_IN_DATA:
		data_required = true;
		_data_in = true;
		_status_in = true;
		break;
	}

	XHCICreateSetupTRB(slot, request->request_type, request->request, request->value, request->index, request->length, trt);
	if (data_required)
		XHCICreateDataTRB(slot, buffer_addr, len, _data_in);
	XHCICreateStatusTRB(slot, _status_in);
	XHCIRingDoorbellSlot(dev, slot_id, XHCI_DOORBELL_ENDPOINT_0);
}

/*
 * XHCISendNormalTRB -- sends a normal trb
 */
void XHCISendNormalTRB(XHCIDevice* dev, XHCISlot* slot, uint64_t data_buffer, uint16_t data_len, XHCIEndpoint* ep) {
	size_t pos = 0;
	while (pos != data_len) {
		size_t cnt = PAGE_SIZE - ((data_buffer + pos) & (PAGE_SIZE - 1));
		bool last = cnt >= data_len - pos;
		if (last) cnt = data_len - pos;
		
		size_t remaining_pack = (data_len - pos + ep->max_packet_sz - 1) / ep->max_packet_sz;

		uint32_t ctrl = (TRB_TRANSFER_NORMAL << 10) | (1 << 6) | (1 << 1);
		if (last)
			ctrl |= (1 << 5);


		if (ep != 0) {
			XHCISendCmdOtherEP(slot, ep->endpoint_num, data_buffer & UINT32_MAX, (data_buffer >> 32) & UINT32_MAX,
				((remaining_pack & 0xFFFF) << 17) | cnt & UINT16_MAX,
				ctrl);
		}
		else {
			XHCISendCmdToHost(dev, data_buffer & UINT32_MAX, (data_buffer >> 32) & UINT32_MAX, ((remaining_pack & 0xFFFF) << 17) | cnt & UINT16_MAX,
				(TRB_TRANSFER_NORMAL << 10) | (1 << 6) | (1 << 5));
		}

		pos += cnt;
	}

	uint32_t ep_num = 0;
	if (ep)
		ep_num = ep->dci;
	else
		ep_num = XHCI_DOORBELL_ENDPOINT_0;
	XHCIRingDoorbellSlot(dev, slot->slot_id, ep_num);
}

/*
 * XHCIBulkTransfer -- Bulk transfer callback
 * @param dev -- Pointer to host device structure
 * @param slot -- Pointer to device slot
 * @param buffer -- Pointer to memory buffer
 * @param data_len -- total data length
 * @param ep_ -- Pointer to endpoint structure
 */
void XHCIBulkTransfer(XHCIDevice* dev, XHCISlot* slot, uint64_t buffer, uint16_t data_len, XHCIEndpoint* ep_) {
	size_t pos = 0;
	while (pos != data_len) {
		size_t cnt = PAGE_SIZE - ((buffer + pos) & (PAGE_SIZE - 1));
		bool last = cnt >= data_len - pos;
		if (last) cnt = data_len - pos;
		size_t remaining_pack = (data_len - pos + ep_->max_packet_sz - 1) / ep_->max_packet_sz;

		uint32_t ctrl = (TRB_TRANSFER_NORMAL << 10);
		
		/*
		 * Put the chain bit to make it a TD
		 */
		if (!last) {
			ctrl |= (1 << 4);
		}

		if (last)
			ctrl |= (1 << 5);


		/*
		 * put the immediate data bit if it's in
		 */
		if (ep_->dir) {
			//SeTextOut("This ep -> %d , is IN \r\n", ep_->endpoint_num);
			ctrl |= (1 << 6);
		}


		if (ep_ != 0) {
			XHCISendCmdOtherEP(slot, ep_->endpoint_num, buffer & UINT32_MAX, (buffer >> 32) & UINT32_MAX,
				((remaining_pack & 0xFFFF) << 17) | cnt & UINT16_MAX,
				ctrl);
		}
		else {
			XHCISendCmdToHost(dev, buffer & UINT32_MAX, (buffer >> 32) & UINT32_MAX, ((remaining_pack & 0xFFFF) << 17) | cnt & UINT16_MAX,
				(TRB_TRANSFER_NORMAL << 10) | (1 << 6) | (1 << 5));
		}

		pos += cnt;
		buffer += ep_->max_packet_sz;
	}

	uint32_t ep_num = 0;
	if (ep_)
		ep_num = ep_->dci;
	else
		ep_num = XHCI_DOORBELL_ENDPOINT_0;
	XHCIRingDoorbellSlot(dev, slot->slot_id, ep_num);
}