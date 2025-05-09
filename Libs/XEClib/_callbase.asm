;;==================================================================================
;;
;; BSD 2-Clause License
;;
;; Copyright (c) 2022-2023, Manas Kamal Choudhury
;; All rights reserved.
;;
;; Redistribution and use in source and binary forms, with or without
;; modification, are permitted provided that the following conditions are met:
;;
;; 1. Redistributions of source code must retain the above copyright notice, this
;;    list of conditions and the following disclaimer.
;;
;; 2. Redistributions in binary form must reproduce the above copyright notice,
;;    this list of conditions and the following disclaimer in the documentation
;;    and/or other materials provided with the distribution.
;;
;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;;
;;===================================================================================

section .text
[BITS 64]

global __chkstk
__chkstk:
ret

%define YES_DYNAMIC 0

;;----------------------------------
;; _KePrint is a demo call to kernel
;; using kernel print interface to
;; print a demo message
;; @param rcx -- the text to pass
;;----------------------------------
global _KePrint
%ifdef YES_DYNAMIC
export _KePrint
%endif
_KePrint:
      mov r12, 1
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, r9
	  syscall
	  ret

;;----------------------------
;; _KePauseThread: Pause the
;; currently running thread
;;----------------------------
global _KePauseThread
%ifdef YES_DYNAMIC
export _KePauseThread
%endif
_KePauseThread:
      mov r12, 2
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret


;;---------------------------------
;; _KeGetThreadID -- get the id
;; of currently running thread
;;---------------------------------
global _KeGetThreadID
%ifdef YES_DYNAMIC
export _KeGetThreadID
%endif
_KeGetThreadID:
      mov r12, 3
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret
      

;;-------------------------------
;; _KeGetProcessID -- returns the
;; current process id 
;;--------------------------------
global _KeGetProcessID
%ifdef YES_DYNAMIC
export _KeGetProcessID
%endif
_KeGetProcessID:
      mov r12, 4
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;;----------------------------------------
;; _KeProcessExit -- exits the currently
;; running process
;;----------------------------------------
global _KeProcessExit
%ifdef YES_DYNAMIC
export _KeProcessExit
%endif
_KeProcessExit:
      mov r12, 5
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret


;;===================================
;; _KeProcessWaitForTermination: suspends
;; currently running process until another
;; process state changes
;; @param pid -- process id in rcx
;;=========================================

global _KeProcessWaitForTermination
%ifdef YES_DYNAMIC
export _KeProcessWaitForTermination
%endif
_KeProcessWaitForTermination:
      mov r12, 6
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;;=======================================
;; _KeCreateProcess -- create a new process
;; slot by taking
;; @param parent_id -- process parent id
;; @param name -- process name
;; @ret -- process id of newly create process
;; slot
;;========================================
global _KeCreateProcess
%ifdef YES_DYNAMIC
export _KeCreateProcess
%endif
_KeCreateProcess:
      mov r12, 7
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeProcessLoadExec
%ifdef YES_DYNAMIC
export _KeProcessLoadExec
%endif
_KeProcessLoadExec:
      mov r12, 8
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, r9
	  syscall
	  ret

global _KeProcessSleep
%ifdef YES_DYNAMIC
export _KeProcessSleep
%endif
_KeProcessSleep:
      mov r12, 23
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret


;;===============================
;; _KeSetSignal -- Register a signal
;; handler
;;===============================
global _KeSetSignal
%ifdef YES_DYNAMIC
export _KeSetSignal
%endif
_KeSetSignal:
      xor rax, rax
	  mov r12, 25
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeCreateSharedMem
%ifdef YES_DYNAMIC
export _KeCreateSharedMem
%endif
_KeCreateSharedMem:
      mov r12, 9
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

global _KeObtainSharedMem
%ifdef YES_DYNAMIC
export _KeObtainSharedMem
%endif
_KeObtainSharedMem:
      mov r12, 10
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

global _KeUnmapSharedMem
%ifdef YES_DYNAMIC
export _KeUnmapSharedMem
%endif
_KeUnmapSharedMem:
      mov r12, 11
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeMemMap
%ifdef YES_DYNAMIC
export _KeMemMap
%endif
_KeMemMap:
      xor rax, rax
	  mov r12, 13
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, r9
	  syscall
	  ret

global _KeMemUnmap
%ifdef YES_DYNAMIC
export _KeMemUnmap
%endif
_KeMemUnmap:
      xor rax,rax
	  mov r12, 14
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;====================================
; _KeGetProcessHeapMem -- Grabs some
; memory from heap area of process
; slot
; @param rcx -- size, number of pages
; multiplied by PAGE_SIZE
;====================================
global _KeGetProcessHeapMem
%ifdef YES_DYNAMIC
export _KeGetProcessHeapMem
%endif
_KeGetProcessHeapMem:
      xor rax,rax
	  mov r12, 15
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;=====================================
; _KeOpenFile -- opens a file and
; return a file descriptor
; @param rcx -- file name
; @param rdx -- open mode
;=====================================
global _KeOpenFile
%ifdef YES_DYNAMIC
export _KeOpenFile
%endif
_KeOpenFile:
      xor rax, rax
	  mov r12, 12
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;====================================
; _KeReadFile -- reads a file into
; buffer
; @param rcx -- file descriptor
; @param rdx -- buffer address
; @param r8 -- length to read in
;====================================
global _KeReadFile
%ifdef YES_DYNAMIC
export _KeReadFile
%endif
_KeReadFile:
      xor rax,rax
	  mov r12, 16
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

;====================================
; _KeWriteFile -- write a file onto
; buffer
; @param rcx -- file descriptor
; @param rdx -- buffer address
; @param r8 -- length to write out
;====================================
global _KeWriteFile
%ifdef YES_DYNAMIC
export _KeWriteFile
%endif
_KeWriteFile:
      xor rax,rax
	  mov r12, 17
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

;==================================
; _KeCreateDir -- create a new
; directory
; @param rcx -- pathname
;==================================
global _KeCreateDir
%ifdef YES_DYNAMIC
export _KeCreateDir
%endif
_KeCreateDir:
      xor rax,rax
	  mov r12, 18
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret


;====================================
; _KeRemoveFile -- remove a file
; from file system
; @param rcx -- filename
;====================================
global _KeRemoveFile
%ifdef YES_DYNAMIC
export _KeRemoveFile
%endif
_KeRemoveFile:
      xor rax,rax
	  mov r12, 19
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;====================================
; _KeCloseFile -- closes a file
; @param rcx -- file descriptor
;====================================
global _KeCloseFile
%ifdef YES_DYNAMIC
export _KeCloseFile
%endif
_KeCloseFile:
      xor rax, rax
	  mov r12, 20
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeFileIoControl
%ifdef YES_DYNAMIC
export _KeFileIoControl
%endif
_KeFileIoControl:
      xor rax, rax
	  mov r12, 21
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

;;===============================
;; _KeFileStat -- writes the status
;; of a file to a buffer
;;===============================
global _KeFileStat
%ifdef YES_DYNAMIC
export _KeFileStat
%endif
_KeFileStat:
      xor rax, rax
	  mov r12, 22
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;;===============================
;; _KeGetSystemTimerTick -- get the
;; current timer tick value from
;; kernel
;;===============================
global _KeGetSystemTimerTick
%ifdef YES_DYNAMIC
export _KeGetSystemTimerTick
%endif
_KeGetSystemTimerTick:
      xor rax, rax
	  mov r12, 26
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;;===============================
;; _KeGetFontID -- get a system 
;; font id
;;===============================
global _KeGetFontID
%ifdef YES_DYNAMIC
export _KeGetFontID
%endif
_KeGetFontID:
      xor rax, rax
	  mov r12, 27
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

;;===============================
;; _KeGetNumFonts -- get total 
;; number of fonts installed
;;===============================
global _KeGetNumFonts
%ifdef YES_DYNAMIC
export _KeGetNumFonts
%endif
_KeGetNumFonts:
      xor rax, rax
	  mov r12, 28
	  mov r13, 0
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret
      
;;===============================
;; _KeGetFontSize -- get font size
;; by its name
;;===============================
global _KeGetFontSize
%ifdef YES_DYNAMIC
export _KeGetFontSize
%endif
_KeGetFontSize:
      xor rax, rax
	  mov r12, 29
	  mov r13, rcx
	  mov r14, 0
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret


global _KeMemMapDirty
%ifdef YES_DYNAMIC
export _KeMemMapDirty
%endif
_KeMemMapDirty:
      xor rax, rax
	  mov r12, 30
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, r9
	  syscall
	  ret

global _KeCreateTTY
%ifdef YES_DYNAMIC
export _KeCreateTTY
%endif
_KeCreateTTY:
      xor rax, rax
	  mov r12, 31
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeCreateThread
%ifdef YES_DYNAMIC
export _KeCreateThread
%endif
_KeCreateThread:
      xor rax, rax
	  mov r12, 32
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeSetFileToProcess
%ifdef YES_DYNAMIC
export _KeSetFileToProcess
%endif
_KeSetFileToProcess:
      xor rax, rax
	  mov r12, 33
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, r8
	  mov rdi, 0
	  syscall
	  ret

global _KeProcessHeapUnmap
%ifdef YES_DYNAMIC
export _KeProcessHeapUnmap
%endif
_KeProcessHeapUnmap:
      xor rax, rax
	  mov r12, 34
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeSendSignal
%ifdef YES_DYNAMIC
export _KeSendSignal
%endif
_KeSendSignal:
      xor rax, rax
	  mov r12, 35
	  mov r13, rcx
	  mov r14, rdx
	  mov r15, 0
	  mov rdi, 0
	  syscall
	  ret

global _KeGetCurrentTime
%ifdef YES_DYNAMIC
export _KeGetCurrentTime
%endif
_KeGetCurrentTime:
     xor rax, rax
	 mov r12, 36
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeOpenDir
%ifdef YES_DYNAMIC
export _KeOpenDir
%endif
_KeOpenDir:
     xor rax,rax
	 mov r12, 37
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeReadDir
%ifdef YES_DYNAMIC
export _KeReadDir
%endif
_KeReadDir:
     xor rax, rax
	 mov r12, 38
	 mov r13, rcx
	 mov r14, rdx
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeCreateTimer
%ifdef YES_DYNAMIC
export _KeCreateTimer
%endif
_KeCreateTimer:
     xor rax, rax
	 mov r12, 39
	 mov r13, rcx
	 mov r14, rdx
	 mov r15, r8
	 mov rdi, 0
	 syscall
	 ret

global _KeStartTimer
%ifdef YES_DYNAMIC
export _KeStartTimer
%endif
_KeStartTimer:
     xor rax, rax
	 mov r12, 40
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeStopTimer
%ifdef YES_DYNAMIC
export _KeStopTimer
%endif
_KeStopTimer:
     xor rax, rax
	 mov r12, 41
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeDestroyTimer
%ifdef YES_DYNAMIC
export _KeDestroyTimer
%endif
_KeDestroyTimer:
     xor rax, rax
	 mov r12, 42
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeProcessGetFileDesc
%ifdef YES_DYNAMIC
export _KeProcessGetFileDesc
%endif
_KeProcessGetFileDesc:
     xor rax, rax
	 mov r12, 43
	 mov r13, rcx
	 mov r14, 0
	 mov r15, 0
	 mov rdi, 0
	 syscall
	 ret

global _KeFileSetOffset
%ifdef YES_DYNAMIC
export _KeFileSetOffset
%endif
_KeFileSetOffset:
    xor rax, rax
	mov r12, 44
	mov r13, rcx
	mov r14, rdx
	mov r15, 0
	mov rdi, 0
	syscall
	ret


global gettimeofday
%ifdef YES_DYNAMIC
export gettimeofday
%endif
gettimeofday:
    xor rax, rax
	mov r12, 45
	mov r13, rcx
	mov r14, 0
	mov r15, 0
	mov rdi, 0
	syscall
	ret

global socket
%ifdef YES_DYNAMIC
export socket
%endif
socket:
   xor rax,rax
   mov r12, 46
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global connect
%ifdef YES_DYNAMIC
export connect
%endif
connect:
   xor rax, rax
   mov r12, 47
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global send
%ifdef YES_DYNAMIC
export send
%endif
send:
   xor rax, rax
   mov r12, 48
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global receive
%ifdef YES_DYNAMIC
export receive
%endif
receive:
   xor rax,rax
   mov r12, 49
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global socket_setopt
%ifdef YES_DYNAMIC
export socket_setopt
%endif
socket_setopt:
   xor rax, rax
   mov r12, 50
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, r9
   ; rest will be passed through stack
   syscall
   ret

global bind
%ifdef YES_DYNAMIC
export bind
%endif
bind:
   xor rax, rax
   mov r12, 51
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global accept
%ifdef YES_DYNAMIC
export accept
%endif
accept:
   xor rax, rax
   mov r12, 52
   mov r13, rcx
   mov r14, rdx
   mov r15, r8
   mov rdi, 0
   syscall
   ret

global listen
%ifdef YES_DYNAMIC
export listen
%endif
listen:
    xor rax, rax
	mov r12, 53
	mov r13, rcx
	mov r14, rdx
	mov r15, 0
	mov rdi, 0
	syscall
	ret

global _KeCreatePipe
%ifdef YES_DYNAMIC
export _KeCreatePipe
%endif
_KeCreatePipe:
    xor rax, rax
	mov r12, 54
	mov r13, rcx
	mov r14, rdx
	mov r15, 0
	mov rdi, 0
	syscall
	ret

global _KeGetStorageDiskInfo
%ifdef YES_DYNAMIC
export _KeGetStorageDiskInfo
%endif
_KeGetStorageDiskInfo:
    xor rax,rax
	mov r12, 55
	mov r13, rcx
	mov r14, rdx
	mov r15, 0,
	mov rdi, 0
	syscall
	ret

global _KeGetStoragePartitionInfo
%ifdef YES_DYNAMIC
export _KeGetStoragePartitionInfo
%endif
_KeGetStoragePartitionInfo:
    xor rax, rax
	mov r12, 56
	mov r13, rcx
	mov r14, rdx
	mov r15, r8
	mov rdi, 0
	syscall
	ret

global _KeGetEnvironmentBlock
%ifdef YES_DYNAMIC
export _KeGetEnvironmentBlock
%endif
_KeGetEnvironmentBlock:
    xor rax, rax
	mov r12, 57
	mov r13, 0
	mov r14, 0
	mov r15, 0
	mov rdi, 0
	syscall
	ret





      
      

      





      

     