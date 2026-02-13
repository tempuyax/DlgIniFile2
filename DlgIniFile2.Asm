; ==================================================================================================
; Title:   DlgIniFile.asm
; Author:  Pahor.m  @ Mei 2020
; Version: 1.0.0
; Purpose: Demonstration program 1 of ObjAsm32.
; ==================================================================================================

;Include & initialize standard modules
%include @Environ(OA32_PATH)\\Code\\Macros\\Model.inc

;Loads OOP files and OS related objects       
SysSetup OOP_WINDOWS , DEBUG(WND)                          

; For COMMON CONTROL Function
IncludeAPIs comdlg32      ;Includes COMMONCTRL these APIs

; Load or build the following objects
LoadObjects Dialog, DialogModal
LoadObjects IniFile
LoadObjects Collection
LoadObjects Stream			; Load & Stor Funtion

;LoadObjects WinApp, SdiApp                                  
;LoadObjects Window
;LoadObjects Button, ColorButton

;Includes Demonstration object (insert .code)
;include skindata.inc                                      
;include skinregion.inc                                      
;include skinpaint.inc 
;include skinTittle.inc 
;include skinMenu.inc 
include skinkey.inc
include skincollection.inc

;Include Main App                                
include DlgIniFile2.inc   
                                   
;The application is an object called "DlgIniFile"
start:                          ;Program entry point
	mov  hREdDll, $invoke(LoadLibrary, addr RichEdDLL) ; Load the Riched20.dll
    SysInit						;Runtime initialization of OOP model (Required)

    OCall TPL_DlgIniFile::DlgIniFile.Init	;Initialize the object data
    OCall TPL_DlgIniFile::DlgIniFile.Run
    OCall TPL_DlgIniFile::DlgIniFile.Done

    SysDone						;Runtime finalization of the OOP model (Required)
	invoke FreeLibrary, hREdDll
    invoke ExitProcess, 0		;Exits program returning 0 to the OS
end start						;Code end and defines prg entry point



