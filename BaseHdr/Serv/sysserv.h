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

#ifndef __SYSSERV_H__
#define __SYSSERV_H__

#include <stdint.h>
#include <stdarg.h>
#include <aurora.h>
#ifdef ARCH_X64
#include <Hal\x86_64_signal.h>
#endif
#include <Net\socket.h>

/* maximum supported system calls */
#define AURORA_MAX_SYSCALL  58
#define AURORA_SYSCALL_MAGIC  0x15062023 

/* ==========================================
 *  Threading
 * ==========================================
 */

/*
* PauseThread -- pause the currently running
* thread
*/
extern int PauseThread();

/*
* GetThreadID -- returns currently running thread id
*/
extern uint16_t GetThreadID();

/*
* GetProcessID -- returns currently running process
* id
*/
extern int GetProcessID();

/*
* ProcessExit -- marks a process as died
*/
extern int ProcessExit();

/*
* ProcessWaitForTermination -- wait for termination of
* a child process
* @param pid -- child process id, if -1 then any process
*/
extern int ProcessWaitForTermination(int pid);

/*
* CreateProcess -- creates a new process slot
* @param parent_id -- parent process id
* @param name -- name of the current process slot
*/
extern int CreateProcess(int parent_id, char *name);

/*
* ProcessLoadExec -- loads an executable to a
* process slot
*/
extern int ProcessLoadExec(int proc_id, char* filename, int argc, char** argv);

/*
* ProcessSleep -- put the current thread to sleep and process
* to busy wait state
* @param ms -- millisecond
*/
extern int ProcessSleep(uint64_t ms);

/*
* SignalReturn -- returns from a signal handler
*/
extern void SignalReturn(int num);

/*
* SetSignal -- register a signal handler
* @param signo -- signal number
* @param handler -- handler to register
*/
extern int SetSignal(int signo, AuSigHandler handler);
/*
* CreateSharedMem -- create a shared memory chunk
* @param key -- key to use
* @param sz -- memory size
* @param flags -- shared memory flags
*/
extern int CreateSharedMem(uint16_t key, size_t sz, uint8_t flags);

/*
* ObtainSharedMem -- obtain a shared memory
* @param id -- segment id
* @param shmaddr -- user specified address
* @param shmflg -- flags to use for protection
*/
extern void* ObtainSharedMem(uint16_t id, void* shmaddr, int shmflg);

/*
* UnmapSharedMem -- unmap shared memory segment
* @param key -- key to search
*/
extern void UnmapSharedMem(uint16_t key);

/*
* GetProcessHeapMem -- get a memory from
* process heap
*/
extern uint64_t GetProcessHeapMem(size_t sz);

/*
* OpenFile -- opens a file for user process
* @param file -- file path
* @param mode -- mode of the file
*/
extern int OpenFile(char* filename, int mode);

/*
* ReadFile -- reads a file into given buffer
* @param fd -- file descriptor
* @param buffer -- buffer where to put the data
* @param length -- length in bytes
*/
extern size_t ReadFile(int fd, void* buffer, size_t length);

/*
* WriteFile -- write system call
* @param fd -- file descriptor
* @param buffer -- buffer to write
* @param length -- length in bytes
*/
extern size_t WriteFile(int fd, void* buffer, size_t length);

/*
* CreateDir -- creates a directory
* @param filename -- name of the directory
*/
extern int CreateDir(char* filename);

/*
* RemoveFile -- remove a directory or file
* @param dirname -- directory name
*/
extern int RemoveFile(char* pathname);

/*
* CloseFile -- closes a general file
* @param fd -- file descriptor to close
*/
extern int CloseFile(int fd);

/*
* FileIoControl -- controls the file through I/O code
* @param fd -- file descriptor
* @param code -- code to pass
* @param arg -- argument to pass
*/
extern int FileIoControl(int fd, int code, void* arg);

/*
* FileStat -- writes information related
* to file
* @param fd -- file descriptor
* @param buf -- Pointer to file structure
*/
extern int FileStat(int fd, void* buf);

/*
* GetSystemTimerTick -- returns the current system
* tick
*/
extern size_t GetSystemTimerTick();

/*
* CreateUserThread -- creates an user mode thread
*/
extern int CreateUserThread(void(*entry) (), char *name);

/*
* SetFileToProcess -- copies a file from one process
* to other
* @param fileno -- file number of the current process
* @param dest_fdidx -- destination process file index
* @param proc_id -- destination process id
*/
extern int SetFileToProcess(int fileno, int dest_fdidx, int proc_id);

/*
* ProcessHeapUnmap -- unmaps previosly allocated
* heap memory
* @param ptr -- Pointer to freeable address
* @param sz -- size in bytes to unallocate
*/
extern int ProcessHeapUnmap(void* ptr, size_t sz);

/*
* SendSignal -- sends a signal to desired process
* note here pid means thread id
* @param pid -- thread id
* @param signo -- signal number
*/
extern int SendSignal(int pid, int signo);

/*
* GetCurrentTime -- get current time
* @param ptr -- pointer to time struct
*/
extern int GetCurrentTime(void* ptr);

/*
* KillProcess -- forcefully kills a process
* @param proc_id -- process id
*/
extern int KillProcess(int proc_id);

/*
* OpenDir -- opens a directory
* @param filename -- name of the directory
*/
extern int OpenDir(char* filename);

/*
* ReadDir -- reads a directory entry
* @param dirfd -- directory file descriptor
* @param dirent -- aurora directory entry struct
*/
extern int ReadDir(int dirfd, void* dirent);


/*
* CreateTimer -- create timer service
* @param maxTickLimit -- maximum tick limit
* @param updatemode -- Timer update mode
*/
extern int CreateTimer(int threadID,int maxTickLimit, uint8_t updatemode);

/*
* StartTimer -- starts the timer
*/
extern int StartTimer(int threadID);

/*
* StopTimer -- stop the timer
*/
extern int StopTimer(int threadID);

/*
* DestroyTimer -- remove the timer
*/
extern int DestroyTimer(int threadID);

/*
* ProcessGetFileDesc -- Searches all process file
* descriptor entries for
* specific filename fd
*/
extern int ProcessGetFileDesc(const char* filename);

/*
* FileSetOffset -- set a offset inorder to read the
* specific position of the file
* @param fd -- File descriptor
* @param offset -- offset in bytes
*/
extern int FileSetOffset(int fd, size_t offset);

/*
* GetTimeOfDay -- returns the time format 
* in unix format
* @param ptr -- Pointer to timeval 
* structure
*/
extern int GetTimeOfDay(void* ptr);

extern int NetSend(int sockfd, msghdr* msg, int flags);

extern int NetReceive(int sockfd, msghdr *msg, int flags);

extern int NetConnect(int sockfd, sockaddr* addr, socklen_t addrlen);

extern int NetBind(int sockfd, sockaddr *addr, socklen_t addrlen);

extern int NetAccept(int sockfd, sockaddr *addr, socklen_t * addrlen);

extern int NetListen(int sockfd, int backlog);

/*
 * GetEnvironmentBlock -- returns environment
 * block of this process
 */
extern size_t GetEnvironmenBlock();
#endif