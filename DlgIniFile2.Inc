DlgIniFileID 		equ 1000

IDC_GRP1 equ 1005
IDC_Description equ 1011
IDC_Copyright equ 1012
IDC_Section equ 1001
IDC_Key equ 1003
IDC_Value equ 1007
IDC_BTN1_Open equ 1006
IDC_BTN2_Next equ 1009
IDC_BTN3_Prev equ 1010

IDC_EDT1_Section equ 1002
IDC_EDT2_Key equ 1004
IDC_EDT3_Value equ 1008
IDC_BTN1_Save equ 1014

IDC_Step equ 1013

IDC_LOCK_SECTION equ 1015
IDC_LOCK_KEY equ 1016

IDC_STC_LOCKSECTION equ 1017
IDC_STC_LOCKKEY equ 1018

EditID equ 2000

DlgOpenFile proto :Handle, :Pointer, :Pointer
DlgSaveFile proto :Handle, :Pointer, :Pointer
DlgSetStepFormat proto :Handle, :dword, :Pointer, :dword, :dword
	
	
.data
ini_filter	 		db 'INI Files', 0,'*';bersambung dgn...
ini_ext				db '.ini', 0;kata ini
					;db 'Png files', 0, '*.png', 0
					;db 'jpg files', 0, '*.jpg', 0
					db 0
BuffFileName		db MAX_PATH Dup (?) ;for OPENFILENAME
StepFormat			db "%d of %d", 0

szDescription 		DB "INI File Editor", 0DH, 0AH;enter or newline
            		DB "Ver. 1.0", 0DH, 0AH;enter or newline
	            	DB 0

szCopyright 		DB "Object Assambler 32 Bit (OBJASM32).", 0DH, 0AH
            		DB "(C)Pahor .M, Nunukan 2020", 0DH, 0AH
            		DB 0

RichEdit         	db  'RichEdit20A',0
RichEdDLL        	db  'RICHED20.DLL',0
FontNameC        	db  'Courier New',0
FontNameS        	db  'Tahoma',0

lf               	LOGFONT        <?>
charF            	CHARFORMAT2    <?>

hREdit				dd	0
hFont				dd  0
BufPrint			dd  0
Buf					dd  0
Line				dd	0
hREdDll 			dd 	0
            
.code
Object DlgIniFile, DlgIniFileID, DialogModal
  	RedefineMethod    	Init
 	VirtualMethod		Run					;virtual fungsi "Run" harus dibuatan dan di jalankan tampa turunan "Run" di atasnya
 	RedefineMethod    	Done

	VirtualMethod		GetVal, 			Pointer, Pointer, Pointer
	VirtualMethod		PutVal, 			Pointer, Pointer, Pointer
	VirtualMethod		SetDlgTextEdit,  	Handle
	VirtualMethod		GetDlgTextEdit,  	Handle
	VirtualMethod		PutNewSecKey,	  	Handle
	
  	RedefineMethod    	OnCommand,          dword, dword
  	RedefineMethod   	OnInitDialog,       dword, dword
  	VirtualEvent      	OnClose,    		WM_CLOSE
  	VirtualEvent      	OnSize,	    		WM_SIZE
	;VirtualEvent      	OnCtlColorStatic,   WM_CTLCOLORSTATIC
    
    DefineVariable		pIniFile,         	Pointer,    0
    DefineVariable		pSkinCollection,    Pointer,    0
    
	DefineVariable		fOpen,		 		dword, 		FALSE
	DefineVariable		ffSave,		 		dword, 		FALSE

	DefineVariable		ItemNum,	   	    dword,     	0
	DefineVariable		ItemIdx,	   	    dword,     	0
	DefineVariable		ItemSum,	   	    dword,     	0
	
	DefineVariable		fLockSection,  	    dword,     	0
	DefineVariable		fLockKey,	   	    dword,     	0
ObjectEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.Init
; Purpose:   
; Arguments: None.
; Return:    
; Note:      

Method DlgIniFile.Init, uses esi
    SetObject esi
    ;Commoncontrol
    invoke LoadCommonControls, 0  ;Activate Controls for XP controls
    
    ;Setup Dialog
    ACall esi.Init, esi, NULL, "DlgIniFile" ; hWnd is NULL = Desktop
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.Run
; Purpose:   
; Arguments: None.
; Return:    
; Note:      

Method DlgIniFile.Run, uses esi
    SetObject esi
	;Show Dialog 
    OCall esi.Show
MethodEnd


; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.Done
; Purpose:   
; Arguments: None.
; Return:    

Method DlgIniFile.Done, uses esi
    SetObject esi
	;DbgWarning "Jalan"
    ;OCall [esi].pSkinCollection::SkinCollection.ForEach, $addr(CallDone)
    ;OCall [esi].pSkinCollection::SkinCollection.DisposeAll
    Destroy [esi].pIniFile
	Destroy [esi].pSkinCollection
    ACall esi.Done   
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.OnInitDialog
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    
; Note:      

Method DlgIniFile.OnInitDialog, uses esi, wParam:dword, lParam:dword
    SetObject esi
    mov eax, (WS_CAPTION or WS_SYSMENU);WS_POPUP ;WS_OVERLAPPEDWINDOW
    ;BitClr eax, (WS_BORDER or WS_DLGFRAME)              ;Reset these bits
    invoke SetWindowLong, [esi].hWnd, GWL_STYLE, eax
    invoke SetWindowLong, [esi].hWnd, GWL_EXSTYLE, 0;WS_EX_DLGMODALFRAME	   ;Set GWL_EXSTYLE to zero 
    
;---------- [Create the Edit control] ----------
	invoke CreateWindowEx, NULL, addr RichEdit, NULL,
	       (WS_VISIBLE or WS_CHILD or ES_LEFT or ES_MULTILINE or WS_BORDER or ES_AUTOVSCROLL),
	        0, 0, 0, 0, [esi].hWnd, EditID, hInstance, NULL
	mov     hREdit, eax
;---------- [Create the Edit font] ----------
    invoke lstrcpy, addr lf.lfFaceName, addr FontNameC
    mov     lf.lfHeight, -9
    mov     lf.lfWidth, 0
    mov     lf.lfWeight, 600
    mov     hFont,$invoke(CreateFontIndirect, addr lf)

    invoke SendMessage, hREdit, EM_EXLIMITTEXT, 0, 100000

;---------- [Set back ground and text colors] ----------
    invoke SendMessage, hREdit, WM_SETFONT, hFont, 1
    invoke SendMessage, hREdit, EM_SETBKGNDCOLOR, 0, $RGB(0,0,0)
    invoke lstrcpy, addr charF.szFaceName, addr FontNameC
        mov     charF.cbSize, sizeof charF
        mov     charF.crTextColor, $RGB(255,255,0)
        mov     charF.crBackColor, $RGB(0,0,0)
        mov     charF.yHeight, 180
        mov     charF.dwEffects, 0 ;CFE_BOLD
        mov     charF.dwMask, (CFM_FACE or CFM_SIZE or CFM_COLOR or CFM_BOLD)
    invoke SendMessage, hREdit, EM_SETCHARFORMAT, SCF_ALL, addr charF

    ;invoke SendMessage, hREdit, WM_SETTEXT, 0, addr RichEdit
    mov Line, $invoke(SendMessage, hREdit, EM_GETLINECOUNT, 0, 0)
    
    ; Create Print Buffer
    mov Buf, $MemAlloc(255, MEM_INIT_ZERO)
    ;invoke MemZero, Buf, 255
    invoke StrCat, Buf, addr szCopyright
    invoke StrCat, Buf, addr szDescription
    mov BufPrint, $invoke(StrNew, Buf)
    MemFree Buf
	Invoke GetDlgItem, [esi].hWnd, EditID
    Invoke SendMessage, eax, WM_SETTEXT, 0, BufPrint
    invoke StrDispose, BufPrint

;---------- [Set select text colors] ----------
    invoke SendMessage, hREdit, EM_HIDESELECTION, 1, 0
    invoke SendMessage, hREdit, EM_LINEINDEX, 2, 0
       dec  eax
    invoke SendMessage, hREdit, EM_SETSEL, 0, eax
       mov charF.crTextColor, $RGB(0,0,255)
    invoke SendMessage,  hREdit, EM_SETCHARFORMAT, SCF_SELECTION, addr charF
    
    invoke SendMessage, hREdit, EM_LINEINDEX, Line, 0
       dec eax
    invoke SendMessage, hREdit, EM_SETSEL, eax, -1
       mov charF.crTextColor, $RGB(0,255,0)
    invoke SendMessage,  hREdit, EM_SETCHARFORMAT, SCF_SELECTION, addr charF
    invoke SendMessage, hREdit, EM_SETSEL, 0, 0
    invoke SendMessage, hREdit, EM_HIDESELECTION, 0, 0

	;Setup System Variable
   	mov [esi].ItemNum, 			NULL
   	mov [esi].ItemIdx, 			NULL
   	mov [esi].ItemSum, 			NULL
   	mov [esi].fOpen,   			FALSE
   	mov [esi].ffSave,  			FALSE
   	mov [esi].fLockSection,  	BST_CHECKED
   	mov [esi].fLockKey,  		BST_CHECKED

    ;SetUp Lock Checked
    invoke CheckDlgButton, [esi].hWnd, IDC_LOCK_SECTION, [esi].fLockSection
    invoke CheckDlgButton, [esi].hWnd, IDC_LOCK_KEY, [esi].fLockKey

	;Setup Hidden Edit Control
	invoke GetDlgItem, [esi].hWnd, IDC_EDT1_Section
	invoke ShowWindow, eax, SW_HIDE	
	invoke GetDlgItem, [esi].hWnd, IDC_EDT2_Key
	invoke ShowWindow, eax, SW_HIDE	

	; Setup Section & Key string of INI file	
   	mov [esi].pSkinCollection, $New(SkinCollection, Init, 20, 25)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"SKIN", "MaskSkinbmp", "NULL_1", 0)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"SKIN", "Skinbmp", "NULL_2", 1)
    
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nTopLftWidth", "NULL_3", 2)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nTopLftHeight", "NULL_4", 3)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nTopRgtWidth", "NULL_5", 4)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nBotLftWidth", "NULL_6", 5)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nBotLftHeight", "NULL_7", 6)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"CORNER", "nBotRgtWidth", "NULL_8", 7)

    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAME", "nTopFrameHeight", "NULL_9", 8)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAME", "nLeftFrameWidth", "NULL_10", 9)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAME", "nRightFrameWidth", "NULL_11", 10)
    
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAMETILE", "nTopFrameWidth", "NULL_12", 11)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAMETILE", "nLeftFrameHeight", "NULL_13", 12)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAMETILE", "nRightFrameHeight", "NULL_14", 13)
    OCall [esi].pSkinCollection::SkinCollection.Insert, $New(SkinKey, Init,"FRAMETILE", "nBottomFrameWidth", "NULL_15", 14)

	;Setup Counter Item
   	inc [esi].ItemNum

    mov ebx, [esi].pSkinCollection
	m2m [esi].ItemSum, [ebx].SkinCollection.dCount 

	invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	

MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.OnClose
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    
; Note:      

Method DlgIniFile.OnClose, uses esi, wParam:dword, lParam:dword
    SetObject esi
    OCall esi.DestroyDialog, wParam
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.OnSize
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    

Method DlgIniFile.OnSize, uses esi, wParam:dword, lParam:dword
	LOCAL rc:RECT, hCtl:Handle
    SetObject esi
    invoke HideCaret,hREdit
;---------- [Set window edit] ----------
	mov hCtl, $invoke(GetDlgItem, [esi].hWnd, IDC_Description)
	invoke GetClientRect,hCtl, addr rc
	; jika 1 titik sesuai & ukuran sesuai 
	;      2 titik sesuai tetapi ukuran mengikuti main windows	
	Invoke MapWindowPoints, hCtl, [esi].hWnd, Addr rc, 1 	
	invoke SetWindowPos,hREdit,HWND_TOP,rc.left, rc.top, rc.right, rc.bottom,SWP_SHOWWINDOW

;---------- [Set window hidden] ----------
	invoke GetDlgItem, [esi].hWnd, IDC_Description
	invoke SetWindowPos,eax,HWND_BOTTOM,rc.left, rc.top, rc.right, rc.bottom,SWP_HIDEWINDOW
    
MethodEnd



; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.OnCommand
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    
; Note:      

Method DlgIniFile.OnCommand, uses esi, wParam:dword, lParam:dword
    SetObject esi
    mov eax, wParam
   .If eax == (BN_CLICKED + IDC_LOCK_SECTION) 
    	.If $invoke(IsDlgButtonChecked,[esi].hWnd, $LoWord(wParam)) == BST_UNCHECKED
			invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKSECTION
			invoke ShowWindow, eax, SW_HIDE	
			invoke GetDlgItem, [esi].hWnd, IDC_EDT1_Section
			invoke ShowWindow, eax, SW_SHOW	
        .elseif $invoke(IsDlgButtonChecked,[esi].hWnd, $LoWord(wParam)) == BST_CHECKED
			invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKSECTION
			invoke ShowWindow, eax, SW_SHOW	
			invoke GetDlgItem, [esi].hWnd, IDC_EDT1_Section
			invoke ShowWindow, eax, SW_HIDE	
        .endif    
   .elseIf eax == (BN_CLICKED + IDC_LOCK_KEY) 
    	.If $invoke(IsDlgButtonChecked,[esi].hWnd, $LoWord(wParam)) == BST_UNCHECKED
			invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKKEY
			invoke ShowWindow, eax, SW_HIDE	
			invoke GetDlgItem, [esi].hWnd, IDC_EDT2_Key
			invoke ShowWindow, eax, SW_SHOW	
        .elseif $invoke(IsDlgButtonChecked,[esi].hWnd, $LoWord(wParam)) == BST_CHECKED
			invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKKEY
			invoke ShowWindow, eax, SW_SHOW	
			invoke GetDlgItem, [esi].hWnd, IDC_EDT2_Key
			invoke ShowWindow, eax, SW_HIDE	
        .endif    
    .elseif eax == (BN_CLICKED + IDC_BTN1_Open)
		.if [esi].fOpen == FALSE
			.if $invoke(DlgOpenFile, [esi].hWnd, addr ini_filter,addr BuffFileName)
				
				;Set Open System Variable
			   	mov [esi].ItemNum, NULL
			   	mov [esi].ItemIdx, NULL
			   	mov [esi].ItemSum, NULL
			   	mov [esi].fOpen,   TRUE
			   	mov [esi].ffSave,  TRUE
			   	
			  	Invoke GetDlgItem, [esi].hWnd, IDC_BTN1_Open
			  	invoke EnableWindow,eax,FALSE
			   	;invoke SetWindowText,eax, addr BuffFileName
			    invoke SendMessage, hREdit, WM_SETTEXT, 0, addr BuffFileName

			   	mov [esi].pIniFile, $New(IniFile,Init, esi, addr BuffFileName)
		   	    ;Skin BMP
				OCall esi.PutNewSecKey, 0
			    OCall esi.PutNewSecKey, 1

				;Corner
			    OCall esi.PutNewSecKey, 2
			    OCall esi.PutNewSecKey, 3
			    OCall esi.PutNewSecKey, 4
			    OCall esi.PutNewSecKey, 5
			    OCall esi.PutNewSecKey, 6
			    OCall esi.PutNewSecKey, 7

				;Frame
			    OCall esi.PutNewSecKey, 8
			    OCall esi.PutNewSecKey, 9
			    OCall esi.PutNewSecKey, 10
			
				;Frametile
			    OCall esi.PutNewSecKey, 11
			    OCall esi.PutNewSecKey, 12
			    OCall esi.PutNewSecKey, 13
			    OCall esi.PutNewSecKey, 14

				;Setup Counter Item
			   	inc [esi].ItemNum

			    mov ebx, [esi].pSkinCollection
        		m2m [esi].ItemSum, [ebx].SkinCollection.dCount 
        		
				invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
			.endif
		.endif
    	xor eax, eax
    	
    .ElseIf eax == (BN_CLICKED + IDC_BTN2_Next)
		.if [esi].ItemNum == 1
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
		   	OCall esi.SetDlgTextEdit, 0
		.elseif [esi].ItemNum == 2
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
		   	OCall esi.SetDlgTextEdit, 1

		.elseif [esi].ItemNum == 3
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 2
		.elseif [esi].ItemNum == 4
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 3
		.elseif [esi].ItemNum == 5
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 4
		.elseif [esi].ItemNum == 6
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 5
		.elseif [esi].ItemNum == 7
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 6
		.elseif [esi].ItemNum == 8
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 7

		.elseif [esi].ItemNum == 9
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 8
		.elseif [esi].ItemNum == 10
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 9
		.elseif [esi].ItemNum == 11
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 10

		.elseif [esi].ItemNum == 12
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 11
		.elseif [esi].ItemNum == 13
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 12
		.elseif [esi].ItemNum == 14
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 13
		.elseif [esi].ItemNum == 15
       		invoke DlgSetStepFormat,[esi].hWnd, IDC_Step, addr StepFormat, [esi].ItemNum, [esi].ItemSum
		   	inc [esi].ItemNum
			OCall esi.SetDlgTextEdit, 14
		.else
			;reset
			mov [esi].ItemNum, 1            ; dynamic 
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.endif
	    xor eax, eax
	    
    .ElseIf eax == (BN_CLICKED + IDC_BTN3_Prev)
    	dec [esi].ItemNum
		.if [esi].ItemNum == 1
			m2m [esi].ItemNum, [esi].ItemSum            ; dynamic 
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 2
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 3
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 4
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 5
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 6
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 7
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 8
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 9
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 10
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 11
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 12
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 13
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 14
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
		.elseif [esi].ItemNum == 15
	    	dec [esi].ItemNum
			invoke SendMessage,[esi].hWnd,WM_COMMAND,IDC_BTN2_Next, NULL 	
	    .EndIf
    	xor eax, eax
    	
    .ElseIf eax == (BN_CLICKED + IDC_BTN1_Save)
    	.if [esi].ffSave == FALSE
			.if $invoke(DlgSaveFile, [esi].hWnd, addr ini_filter,addr BuffFileName)
				mov [esi].ffSave, TRUE

			   	invoke lstrcat,addr BuffFileName,addr ini_ext
			  	;Invoke GetDlgItem, [esi].hWnd, IDC_Description
			   	;invoke SetWindowText,eax, addr BuffFileName
			    invoke SendMessage, hREdit, WM_SETTEXT, 0, addr BuffFileName
				mov [esi].pIniFile, $New(IniFile,Init, esi, addr BuffFileName)

			.endif
    	.endif

    	.if [esi].ffSave == TRUE
	    	dec [esi].ItemNum
			.if [esi].ItemNum == 1
	        	inc [esi].ItemNum
	        	OCall esi.GetDlgTextEdit, 0
	
			.elseif [esi].ItemNum == 2
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 1
	
			.elseif [esi].ItemNum == 3
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 2
	
			.elseif [esi].ItemNum == 4
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 3
	
			.elseif [esi].ItemNum == 5
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 4
	
			.elseif [esi].ItemNum == 6
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 5
	
			.elseif [esi].ItemNum == 7
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 6
	
			.elseif [esi].ItemNum == 8
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 7
	
			.elseif [esi].ItemNum == 9
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 8
	
			.elseif [esi].ItemNum == 10
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 9
	
			.elseif [esi].ItemNum == 11
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 10
	
			.elseif [esi].ItemNum == 12
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 11
	
			.elseif [esi].ItemNum == 13
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 12
	
			.elseif [esi].ItemNum == 14
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 13
	
			.elseif [esi].ItemNum == 15
	        	inc [esi].ItemNum
			    OCall esi.GetDlgTextEdit, 14
			.endif
		.endif
    	xor eax, eax
   .EndIf
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.GetVal
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    
; Note:      

Method DlgIniFile.GetVal, uses esi, pSec:Pointer, pKey:Pointer, val:Pointer

    SetObject esi

  	;Set Edit Control
  	Invoke GetDlgItem, [esi].hWnd, IDC_EDT1_Section
   	invoke SetWindowText,eax,pSec
  	Invoke GetDlgItem, [esi].hWnd, IDC_EDT2_Key
   	invoke SetWindowText,eax,pKey
  	Invoke GetDlgItem, [esi].hWnd, IDC_EDT3_Value
   	invoke SetWindowText,eax,val

  	;Set Static Control
  	Invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKSECTION
   	invoke SetWindowText,eax,pSec
  	Invoke GetDlgItem, [esi].hWnd, IDC_STC_LOCKKEY
   	invoke SetWindowText,eax,pKey
    
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.PutVal
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
;            Arg3: 
; Return:    
; Note:      

Method DlgIniFile.PutVal, uses esi, pSec:Pointer, pKey:Pointer, val:Pointer

    SetObject esi
	OCall [esi].pIniFile::IniFile.KeySetStr,, pSec, pKey, val
    
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.SetDlgTextEdit
; Purpose:   
; Arguments: Arg1: 
; Return:    
; Note:      

Method DlgIniFile.SetDlgTextEdit, uses esi, hSecKey:Handle
    SetObject esi
	OCall [esi].pSkinCollection::SkinCollection.ItemAt, hSecKey
    mov ebx, [eax].SkinKey.pOwner
   	OCall esi.GetVal, [ebx].SkinKey.Section, [ebx].SkinKey.Key, [ebx].SkinKey.NullStr
   
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.GetDlgTextEdit
; Purpose:   
; Arguments: Arg1: 
; Return:    
; Note:      

Method DlgIniFile.GetDlgTextEdit, uses esi, hSecKey:Handle
	LOCAL  sVal[128]:byte
    SetObject esi
	Invoke RtlZeroMemory, addr sVal, SizeOf sVal
  	mov ecx, $invoke(GetDlgItem, [esi].hWnd, IDC_EDT3_Value)
	invoke GetWindowText,ecx, addr sVal, SizeOf sVal

    OCall [esi].pSkinCollection::SkinCollection.ItemAt, hSecKey
    mov ebx, [eax].SkinKey.pOwner
	Destroy $OCall([esi].pSkinCollection::SkinCollection.PutAt, hSecKey, $New(SkinKey, Init, [ebx].SkinKey.Section, [ebx].SkinKey.Key, addr sVal, hSecKey))

    OCall [esi].pSkinCollection::SkinCollection.ItemAt, hSecKey
    mov ebx, [eax].SkinKey.pOwner
   	OCall esi.PutVal, [ebx].SkinKey.Section, [ebx].SkinKey.Key, [ebx].SkinKey.NullStr
    
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    DlgIniFile.PutNewSecKey
; Purpose:   
; Arguments: Arg1: 
; Return:    
; Note:      

Method DlgIniFile.PutNewSecKey, uses esi, hSecKey:Handle
	LOCAL  pVal:Pointer
    SetObject esi
	
	OCall [esi].pSkinCollection::SkinCollection.ItemAt, hSecKey
	mov ebx, [eax].SkinKey.pOwner
	mov pVal, $OCall([esi].pIniFile::IniFile.KeyGetStr,, [ebx].SkinKey.Section, [ebx].SkinKey.Key)

	OCall [esi].pSkinCollection::SkinCollection.ItemAt, hSecKey
	mov ebx, [eax].SkinKey.pOwner
	Destroy $OCall([esi].pSkinCollection::SkinCollection.PutAt, hSecKey, $New(SkinKey, Init, [ebx].SkinKey.Section, [ebx].SkinKey.Key, pVal, hSecKey))
MethodEnd

DlgOpenFile Proc hWnd:Handle, szFileFilter:Pointer, szFileName:Pointer
	Local ofn:OPENFILENAME
	;clear OPENFILENAME Strcture
	Invoke RtlZeroMemory, Addr ofn, SizeOf OPENFILENAME
	;initial  OPENFILENAME Strcture
	Mov ofn.lStructSize, SizeOf OPENFILENAME
	m2m ofn.hwndOwner, hWnd
	m2m ofn.hInstance, hInstance
	m2m ofn.lpstrFilter, szFileFilter
	Mov ofn.nFilterIndex, 1
	m2m ofn.lpstrFile, szFileName
	Mov ofn.nMaxFile, MAX_PATH
	Mov ofn.Flags, (OFN_EXPLORER Or OFN_FILEMUSTEXIST Or OFN_PATHMUSTEXIST Or OFN_HIDEREADONLY)
    Invoke GetOpenFileName, Addr ofn
	Ret
DlgOpenFile EndP

DlgSaveFile Proc hWnd:Handle, szFileFilter:Pointer, szFileName:Pointer
	Local ofn:OPENFILENAME
	;clear OPENFILENAME Strcture
	Invoke RtlZeroMemory, Addr ofn, SizeOf OPENFILENAME
	;initial  OPENFILENAME Strcture
	Mov ofn.lStructSize, SizeOf OPENFILENAME
	m2m ofn.hwndOwner, hWnd
	m2m ofn.hInstance, hInstance
	m2m ofn.lpstrFilter, szFileFilter
	Mov ofn.nFilterIndex, 1
	m2m ofn.lpstrFile, szFileName
	Mov ofn.nMaxFile, MAX_PATH
	Mov ofn.Flags, (OFN_EXPLORER Or OFN_FILEMUSTEXIST Or OFN_PATHMUSTEXIST Or OFN_HIDEREADONLY)
    Invoke GetSaveFileName, Addr ofn
	Ret
DlgSaveFile EndP

DlgSetStepFormat Proc hWnd:Handle, winID:dword, Charformat:Pointer, Data1:dword, Data2:dword
	Local CharOutPut[128]:CHAR
	Invoke RtlZeroMemory, Addr CharOutPut, SizeOf CharOutPut
	Invoke wsprintf, Addr CharOutPut, Charformat, Data1, Data2
	mov ecx, $invoke(GetDlgItem, hWnd, winID)
   	invoke SetWindowText,ecx, addr CharOutPut
	Ret
DlgSetStepFormat EndP
