/**
* BSD 2-Clause License
*
* Copyright (c) 2023-2024, Manas Kamal Choudhury
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

#include <stdint.h>
#include <_xeneva.h>
#include <stdio.h>
#include <sys\_keproc.h>
#include <sys\_kefile.h>
#include <sys/mman.h>>
#include <sys\iocodes.h>
#include <string.h>
#include <stdlib.h>
#include <sys\socket.h>
#include <sys\_keipcpostbox.h>
#include <widgets/list.h>

/*
 * DeodhaiAudioProtocol -- Client sends DeodhaiAudioMessage
 * to DeodhaiAudioServer with specific request
 * DeodhaiAudioServer in response sends requested information
 * through PostBox because it is reliable
 * 
 *              _________________
 *        ----> || C L I E N T ||
 *        |     -----------------
 *        |           | |
 * PostBox|           | | -- Request through Named Pipe
 *        |      _____|_|________
 *        ------|| S E R V E R  ||
 *              ------------------
 * 
 */

/*
 * TODO : bypass sound layer of Kernel
 */

int postbox;
int sound;
uint64_t* mainOutput;
list_t* audioBoxList;
uint16_t shControlPanelKey;
uint16_t shSampleBufferKey;

/* Standard Deodhai Request message defined*/
#define DEODHAI_GET_AUDIO_CONNECTION "DeodhaiGetAudioConnection"
#define DEODHAI_GET_GLOBAL_CONNECTION "DeodhaiAudioGetGlobalConnection"

#define DEODHAI_AUDIO_CONNECTION_HANDSHAKE 11
#define DEODHAI_AUDIO_CONNECTION_CLOSED 12

#pragma pack(push,1)
typedef struct _deodhai_audio_msg_ {
	char message[60];
	uint16_t fromProcessId;
	uint16_t toProcessId;
}DeodhaiAudioMessage;
#pragma pack(pop)

/*
 * AudioControlPanel is the main way to 
 * control each individual stream, feautres
 * like attack, release, Eq, compression,
 * reverb to be added in future
 */
#pragma pack(push,1)
typedef struct _audio_control_panel_ {
	uint8_t numChannel;
	float gain;
	float leftSpeakerScale;
	float rightSpeakerScale;
	bool Samplefull;
	bool ready;
	bool close;
	bool global;
	bool dirty;
}AudioControlPanel;
#pragma pack(pop)

typedef struct _audio_box_ {
	AudioControlPanel* ctlPanel;
	uint16_t ownerThreadID;
	void* sampleBuffer;
	int shSampleBuffKey;
	int shControlPanelKey;
}DeodhaiAudioBox;


DeodhaiAudioBox* globalBox;


/*
 * DeodhaiGetNewControlPanelKey -- get a new key
 * for shared control panel
 */
uint16_t DeodhaiGetNewControlPanelKey() {
	uint16_t newKey = shControlPanelKey;
	shControlPanelKey += 1;
	return newKey;
}

/*
 * DeodhaiGetNewSampleBuffKey -- get a new key
 * for sample buffer
 */
uint16_t DeodhaiGetNewSampleBuffKey() {
	uint16_t sampleKey = shSampleBufferKey;
	shSampleBufferKey += 1;
	return sampleKey;
}


/*
 * DeodhaiCreateAudioBox -- create a new audio box
 * @param ownerThreadID -- request process thread id
 */
DeodhaiAudioBox* DeodhaiCreateAudioBox(uint16_t ownerThreadID) {
	DeodhaiAudioBox* audioBox = (DeodhaiAudioBox*)malloc(sizeof(DeodhaiAudioBox));
	memset(audioBox, 0, sizeof(DeodhaiAudioBox));
	audioBox->ownerThreadID = ownerThreadID;

	/* create the control panel */
	uint16_t controlPanelKey = DeodhaiGetNewControlPanelKey();
	int id = _KeCreateSharedMem(controlPanelKey, sizeof(AudioControlPanel), 0);
	void* addr = _KeObtainSharedMem(id, NULL, 0);
	AudioControlPanel* panel = (AudioControlPanel*)addr;
	memset(panel, 0, sizeof(AudioControlPanel));
	audioBox->ctlPanel = panel;
	panel->gain = 1.0f;
	panel->ready = false;
	panel->numChannel = 2;

	/* now create the sample buffer */
	uint16_t sampleKey = DeodhaiGetNewSampleBuffKey();
	int id2 = _KeCreateSharedMem(sampleKey, 4096, 0);
	void* sampleBuff = _KeObtainSharedMem(id2, NULL, 0);

	/* fill up the information to audio box */
	audioBox->sampleBuffer = sampleBuff;
	audioBox->shControlPanelKey = controlPanelKey;
	audioBox->shSampleBuffKey = sampleKey;
	list_add(audioBoxList, audioBox);
	return audioBox;
}

/*
 * DeodhaiAudioBoxClose -- close audio box
 */
void DeodhaiAudioBoxClose(DeodhaiAudioBox* box) {
	_KeUnmapSharedMem(box->shControlPanelKey);
	_KeUnmapSharedMem(box->shSampleBuffKey);
}
/* 
 * DeodhaiAudioHandleMessage -- handle incoming message requests
 * @param message -- Pointer to incoming message
 */
void DeodhaiAudioHandleMessage(DeodhaiAudioMessage* message) {
	if (strcmp(message->message, DEODHAI_GET_AUDIO_CONNECTION) == 0) {
		printf("DeodhaiAudio: GetConnection request received from -> %d \n", message->fromProcessId);
		DeodhaiAudioBox* audioBox = DeodhaiCreateAudioBox(message->fromProcessId);
		audioBox->ctlPanel->global = false;
		PostEvent e;
		memset(&e, 0, sizeof(PostEvent));
		e.type = DEODHAI_AUDIO_CONNECTION_HANDSHAKE;
		e.to_id = message->fromProcessId;
		e.dword = audioBox->shControlPanelKey;
		e.dword2 = audioBox->shSampleBuffKey;
		_KeFileIoControl(postbox, POSTBOX_PUT_EVENT, &e);
	}

	if (strcmp(message->message, DEODHAI_GET_GLOBAL_CONNECTION) == 0) {
		printf("DeodhaiAudio: Global Connection request received from -> %d \n", message->fromProcessId);
		if (globalBox == NULL) {
			DeodhaiAudioBox* audioBox = DeodhaiCreateAudioBox(message->fromProcessId);
			audioBox->ctlPanel->global = true;
			globalBox = audioBox;
		}
		PostEvent e;
		memset(&e, 0, sizeof(PostEvent));
		e.type = DEODHAI_AUDIO_CONNECTION_HANDSHAKE;
		e.to_id = message->fromProcessId;
		e.dword = globalBox->shControlPanelKey;
		e.dword2 = globalBox->shSampleBuffKey;
		_KeFileIoControl(postbox, POSTBOX_PUT_EVENT, &e);
	}
}

void DeodhaiAudioComposeFrame() {
	memset(mainOutput, 0, 4096);
	int16_t* output = (int16_t*)mainOutput;
	for (int i = 0; i < audioBoxList->pointer; i++) {
		DeodhaiAudioBox* box = (DeodhaiAudioBox*)list_get_at(audioBoxList, i);
		if (!box->ctlPanel->ready)
			continue;

		int16_t* sample = (int16_t*)box->sampleBuffer;
		float rightSpeakerScale = box->ctlPanel->rightSpeakerScale;
		float leftSpeakerScale = box->ctlPanel->leftSpeakerScale;

		/* handle panning and gaining for each channel type*/

		if (box->ctlPanel->numChannel == 1) {   /* MONO Channel */

			/* normalize and control the gain */
			for (int j = 0; j < 4096 / sizeof(int16_t); j++) {
				sample[j] /= 2;
				sample[j] *= box->ctlPanel->gain;
			}
			for (int j = 0; j < 4096 / (2* sizeof(int16_t)); j++) {

				sample[j] /= 2;
				/* adjust the sample with desired gain */
				sample[j] *= box->ctlPanel->gain;
				/* left channel */
				output[2 * j] += sample[j] * leftSpeakerScale;
				/* right channel */
				output[2 * j + 1] += sample[j] * rightSpeakerScale;
			}
		}
		if (box->ctlPanel->numChannel == 2) { /* STEREO Channel */

			/* Normalize and control the gain*/
			for (int j = 0; j < 4096 / sizeof(int16_t); j++) {
				sample[j] /= 2;
				sample[j] *= box->ctlPanel->gain;
			}

			for (int j = 0; j < 4096 / (2 * sizeof(int16_t)); j++) {
			
				int16_t leftSample = sample[2 * j];
				int16_t rightSample = sample[2 * j + 1];
				/* left channel */
				output[2 * j] += (int16_t)(leftSample * leftSpeakerScale);
				/* right channel */
				output[2 * j + 1] += (int16_t)(rightSample * rightSpeakerScale);
			}
		}
		box->ctlPanel->Samplefull = false;

		///* handle global controls */
		//if (box->ctlPanel->global) {
		//	if (box->ctlPanel->dirty) {
				/* Normalize and control the gain*/
	/*	}
		_KePrint("[DEODHAI AUDIO]: gian Control %f\r\n", box->ctlPanel->gain);
		box->ctlPanel->dirty = false;
	}*/
		
		/* close an opened deodhai box */
		if (box->ctlPanel->close) {
			list_remove(audioBoxList, i);
			DeodhaiAudioBoxClose(box);
			free(box);
			printf("DeodhaiAudio: an audio box closed \n");
		}
	}
	if (globalBox) {
		for (int j = 0; j < 4096 / sizeof(int16_t); j++) {
			output[j] *= globalBox->ctlPanel->gain;
		}
	}
	_KeWriteFile(sound, mainOutput, 4096);
}
/*
* main -- main entry
*/
int main(int argc, char* argv[]) {
	int process_ID = _KeGetProcessID();

	/* open all important files and configurations */
	int pipe = _KeCreatePipe("DeodhaiAudio", (sizeof(DeodhaiAudioMessage)*4));
	if (pipe != -1)
		printf("Pipe creation successful %d\n", pipe);
	else
		return 1;
	
	postbox = _KeOpenFile("/dev/postbox", FILE_OPEN_READ_ONLY);

	_KeFileIoControl(postbox, POSTBOX_CREATE, NULL);

	sound = _KeOpenFile("/dev/sound", FILE_OPEN_READ_ONLY);

	globalBox = NULL;

	XEFileIOControl ioctl;
	memset(&ioctl, 0, sizeof(XEFileIOControl));

	/* uint_1 holds the millisecond to sleep after
	* one frame playback */
	ioctl.uint_1 = 0;
	ioctl.syscall_magic = AURORA_SYSCALL_MAGIC;
	_KeFileIoControl(sound, SOUND_REGISTER_SNDPLR, &ioctl);

	mainOutput = (uint64_t*)malloc(4096);
	memset(mainOutput, 0, 4096);

	audioBoxList = initialize_list();
	shControlPanelKey = 0x01;
	shSampleBufferKey = 0x32;

	//int threadIdx = _KeCreateThread(FrameThread, "AudioThread");

	int sz = 0;
	char* buff = (char*)malloc(sizeof(DeodhaiAudioMessage)+1);
	while (1) {
		DeodhaiAudioComposeFrame();
		sz = _KeReadFile(pipe, buff, sizeof(DeodhaiAudioMessage)+1);
		if (sz > 0) {
			DeodhaiAudioMessage* msg = (DeodhaiAudioMessage*)buff;
			DeodhaiAudioHandleMessage(msg);
			memset(buff, 0, sizeof(DeodhaiAudioMessage));
		}
		_KeProcessSleep(1);
	}
}