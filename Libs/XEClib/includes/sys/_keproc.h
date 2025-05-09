/**
* BSD 2-Clause License
*
* Copyright (c) 2022-2023, Manas Kamal Choudhury
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

#ifndef __KE_PROC_H__
#define __KE_PROC_H__

#include <stdint.h>
#include <_xeneva.h>
#include <sys\_kesignal.h>

#ifdef __cplusplus
XE_EXTERN{
#endif

	/*
	 * _KePauseThread -- pause currently running
	 * thread
	 */
	XE_LIB int _KePauseThread();

	/*
	 * _KeGetThreadID -- get currently running
	 * thread id
	 */
	XE_LIB uint16_t _KeGetThreadID();

	/*
	 * _KeGetProcessID -- get currently running
	 *  process id
	 */
	XE_LIB int _KeGetProcessID();

	/*
	 * _KeProcessExit -- exits current process
	 */
	XE_LIB int _KeProcessExit();

	/*
	 * _KeProcessWaitForTermination -- suspends
	 * currently running process until another
	 * process state changes
	 * @param pid -- process id,-1 for all
	 */
	XE_LIB int _KeProcessWaitForTermination(int pid);

	/*
	 * _KeCreateProcess -- create a new process
	 * slot by taking
	 * @param parent_id -- process parent id
	 * @param name -- process name
	 * @ret -- process id of newly create process
	 * slot
	 */
	XE_LIB int _KeCreateProcess(int parent_id, char* name);

	/*
	 * _KeProcessLoadExec -- loads an executable to a
	 * process slot
	 * @param proc_id -- process slot id
	 * @param filename -- filename of the process
	 * @param argc -- number of arguments
	 * @param argv -- argument array to pass
	 *
	 */
	XE_LIB int _KeProcessLoadExec(int proc_id, char* filename, int argc, char** argv);

	/*
	* _KeGetProcessHeapMem -- Grabs some memory from
	* heap memory of current process slot
	* @param sz -- size that is needed, should be page-aligned
	*/
	XE_LIB uint64_t _KeGetProcessHeapMem(size_t sz);


	/*
	 * _KeProcessSleep -- put the current process main thread
	 *  to sleep mode
	 *  1s = 10000 ms
	 * @param ms -- millisecond to sleep
	 */
	XE_LIB int _KeProcessSleep(uint64_t ms);

	/*
	 * _KeSetSignal -- register a new signal handler for this
	 * process
	 * @param signo -- signal number
	 * @param handler -- handler function
	 */
	XE_LIB int _KeSetSignal(int signo, XESigHandler handler);

	/*
	 * _KeGetSystemTimerTick -- returns the current system
	 * timer tick
	 */
	XE_LIB size_t _KeGetSystemTimerTick();

	/*
	 * _KeCreateThread -- creates a new thread
	 * inside current process slot
	 */
	XE_LIB int _KeCreateThread(void(*entry) (), char *name);

	/*
	* _KeSetFileToProcess -- copies a file from one process
	* to other
	* @param fileno -- file number of the current process
	* @param dest_fdidx -- destination process file index
	* @param proc_id -- destination process id
	*/
	XE_LIB int _KeSetFileToProcess(int fileno, int dest_fidx, int proc_id);

	/*
	 * _KeSendSignal -- send a signal to desired process
	 * @param pid -- thread id
	 * @param signum -- signal number
	 */
	XE_LIB int _KeSendSignal(int pid, int signum);

	/*
	 * _KeProcessGetFileDesc -- returns file descriptor for
	 * given filename
	 * @param filename -- name of the file if it's already
	 * opened
	 */
	XE_LIB int _KeProcessGetFileDesc(const char* filename);

	/*
	 * _KeGetEnvironmentBlock -- returns the current environment
	 * block of the process
	 */
	XE_LIB uint64_t _KeGetEnvironmentBlock();

#ifdef __cplusplus
}
#endif


#endif