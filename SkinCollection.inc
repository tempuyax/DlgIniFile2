; ==================================================================================================
; Title:   Skincollection.inc
; Author:  Pahor.m  @ Mei 2020
; Version: 1.0.0
; Purpose: Demonstration program 1 of ObjAsm32.
; ==================================================================================================

CallPrint   proto 
CallDone    proto
SearchNullStr proto :Pointer, :Pointer
NullStrMatch  proto :Pointer, :Pointer

SkinCollectionID equ     0002     

;===========================================================
Object SkinCollection, SkinCollectionID, Collection
  RedefineMethod    Init,          DWord, DWord   ;Limit, Faktor
  RedefineMethod    Done
  VirtualMethod     SearchNullStr, Pointer, Pointer
  VirtualMethod     Print

  DefineVariable    NullStrToFind, Pointer, NULL
ObjectEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    SkinCollection.Done
; Purpose:   
; Arguments: None.
; Return:    
; Note:      

Method SkinCollection.Done, uses esi
    SetObject esi
    ACall Done
MethodEnd


; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    SkinCollection.Init
; Purpose:   
; Arguments: Arg1: 
;            Arg2: 
; Return:    

Method SkinCollection.Init, uses esi, nLimit:DWord, nFaktor:DWord
    DbgText "Method: SkinCollection.Init"
    SetObject esi
    ACall Init, esi, nLimit, nFaktor, NULL ; Call init Accsesor of Collection


	; Setup Section & Key string of INI file	
   	;mov [esi].pSkinCollection, $New(Collection, Init, 20, 25)
;    OCall esi.Insert, $New(SkinKey, Init,"SKIN", "MaskSkinbmp", "NULL_1", 0)
;    OCall esi.Insert, $New(SkinKey, Init,"SKIN", "Skinbmp", "NULL_2", 1)
;    
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nTopLftWidth", "NULL_3", 2)
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nTopLftHeight", "NULL_4", 3)
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nTopRgtWidth", "NULL_5", 4)
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nBotLftWidth", "NULL_6", 5)
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nBotLftHeight", "NULL_7", 6)
;    OCall esi.Insert, $New(SkinKey, Init,"CORNER", "nBotRgtWidth", "NULL_8", 7)
;
;    OCall esi.Insert, $New(SkinKey, Init,"FRAME", "nTopFrameHeight", "NULL_9", 8)
;    OCall esi.Insert, $New(SkinKey, Init,"FRAME", "nLeftFrameWidth", "NULL_10", 9)
;    OCall esi.Insert, $New(SkinKey, Init,"FRAME", "nRightFrameWidth", "NULL_11", 10)
;    
;    OCall esi.Insert, $New(SkinKey, Init,"FRAMETILE", "nTopFrameWidth", "NULL_12", 11)
;    OCall esi.Insert, $New(SkinKey, Init,"FRAMETILE", "nLeftFrameHeight", "NULL_13", 12)
;    OCall esi.Insert, $New(SkinKey, Init,"FRAMETILE", "nRightFrameHeight", "NULL_14", 13)
;    OCall esi.Insert, $New(SkinKey, Init,"FRAMETILE", "nBottomFrameWidth", "NULL_15", 14)
    
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    SkinCollection.SearchNullStr
; Purpose:   
; Arguments: Arg1: Pointer SkinCollection
;            Arg2: Pointer string NullStrToFind
; Return:    NONE
Method SkinCollection.SearchNullStr, uses esi, pCollect:Pointer, NullStrToFind:Pointer
    SetObject esi
    
    m2m [esi].NullStrToFind, $invoke(StrNew, NullStrToFind)
    invoke SearchNullStr, pCollect, [esi].NullStrToFind
    invoke StrDispose, [esi].NullStrToFind
  
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    SkinCollection.Print
; Purpose:   
; Arguments: None.
; Return:    
; Note:      

Method SkinCollection.Print, uses esi
    SetObject esi
    DbgText "Method: SkinCollection.Print"
    
MethodEnd

; ——————————————————————————————————————————————————————————————————————————————————————————————————
; Method:    SkinCollection.Append
; Purpose:   
; Arguments: Arg1: 
; Return:    
; Note:      

Method SkinCollection.Append, uses esi, pObject:Pointer
    
    SetObject esi
    ;OCall esi.Insert, $New(SkinKey, Init,"SKIN", "MaskSkinbmp", "NULL_1")
    OCall esi.Insert, pObject
    OCall esi.ItemAt, 0
    mov ebx, [eax].SkinKey.pOwner
	m2m [ebx].SkinKey.Idx, 0
        
MethodEnd



; ——————————————————————————————————————————————————————————————————————————————————————————————————

CallPrint proc 
    LOCAL p:Pointer
    DbgText "Go Print"
    m2m p, [esi]
    invoke GetObjectID, [esi]     ;eax = ID, ecx -> ID
    .if eax == SkinKeyID
       OCall p::SkinKey.Print   
    .elseif eax == SkinCollectionID
       OCall p::SkinCollection.Print   
    .endif
	ret 4 ; NONE parameters
CallPrint endp

CallDone proc 
    LOCAL p:Pointer
    DbgText "Go Destroy"
    m2m p, [esi]
    Destroy p
	ret 4 ; NONE parameters
CallDone endp

; methode memanggil procedure ini. utk mencari string
SearchNullStr proc pCollect:Pointer, NullStrToFind:Pointer
LOCAL FoundNullStr:Pointer
	OCall pCollect::Collection.ItemAt, 0 ; first index
	m2m FoundNullStr, $OCall(pCollect::Collection.FirstThat, $addr(NullStrMatch),eax,NullStrToFind)
	.if eax == NULL
	  DbgText "No SkinKey met the search requirement"
	.else
	  DbgWarning "Found SkinKey:"
	  OCall FoundNullStr::SkinKey.Print
	.endif
	ret
SearchNullStr endp

; Collection.FirstThat di atas menggunakan ini
;Catatan
;[esp + 12] adalah statis parameter 1 (yang awal)
;[esp + 16] adalah statis parameter 2
;[esp + 20] adalah statis parameter 3 (paling ujung)
;dan seterusnya
;ret 4  tampa parmeter
;ret 8  1 parameter
;ret 12 2 parameter
NullStrMatch proc pSkinKey:Pointer, NullStrToFind:Pointer
    invoke GetObjectID, [esi] ;eax = ID, ecx -> ID
    .if eax == SkinKeyID
	    mov ecx, pSkinKey
		DbgText "NullStrMatch"
		.if $invoke(StrPos, [ecx].SkinKey.NullStr, [esp + 16]) 
			mov eax, TRUE
		.else
			mov eax, FALSE
	    .endif
	.else   
	    mov eax, FALSE
    .endif
	ret 12 ; two parameters
NullStrMatch endp
