;############################################################################
;
; FILE:   DSP28xxx_SectionCopy_BIOS.asm
;
; DESCRIPTION:  Provides functionality for copying intialized sections from 
;				flash to ram at runtime before entering the _c_int00 startup
;				routine
;############################################################################
; Author: Tim Love
; Release Date: March 2008	
;############################################################################


	.ref _c_int00
	.global copy_sections
	.global _cinit_loadstart, _cinit_loadend, _cinit_runstart
	.global _const_loadstart, _const_loadend, _const_runstart
	.global _econst_loadstart, _econst_loadend, _econst_runstart
	.global _pinit_loadstart, _pinit_loadend, _pinit_runstart
	.global _switch_loadstart, _switch_loadend, _switch_runstart
	.global _text_loadstart, _text_loadend, _text_runstart
	.global _bios_loadstart, _bios_loadend, _bios_runstart
	.global _gblinit_loadstart, _gblinit_loadend, _gblinit_runstart
	.global _hwi_loadstart, _hwi_loadend, _hwi_runstart
	.global _hwi_vec_loadstart, _hwi_vec_loadend, _hwi_vec_runstart
	.global _rtdx_text_loadstart, _rtdx_text_loadend, _rtdx_text_runstart
	.global _sysinit_loadstart, _sysinit_loadend, _sysinit_runstart
	.global _trcdata_loadstart, _trcdata_loadend, _trcdata_runstart
	
***********************************************************************
* Function: copy_sections
*
* Description: Copies initialized sections from flash to ram
***********************************************************************

	.sect "copysections"

copy_sections:
	MOVW DP,#_cinit_loadstart			; Set DP
    MOVL ACC,@_cinit_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_cinit_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_cinit_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_cinit_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_const_loadstart			; Set DP
    MOVL ACC,@_const_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_const_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_const_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_const_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_econst_loadstart			; Set DP
    MOVL ACC,@_econst_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_econst_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_econst_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_econst_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_pinit_loadstart			; Set DP
    MOVL ACC,@_pinit_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_pinit_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_pinit_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_pinit_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero 

	MOVW DP,#_switch_loadstart			; Set DP
    MOVL ACC,@_switch_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_switch_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_switch_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_switch_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_text_loadstart			; Set DP
    MOVL ACC,@_text_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_text_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_text_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_text_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero
	
	MOVW DP,#_bios_loadstart			; Set DP
    MOVL ACC,@_bios_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_bios_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_bios_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_bios_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_gblinit_loadstart			; Set DP
    MOVL ACC,@_gblinit_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_gblinit_loadstart		; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_gblinit_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_gblinit_runstart		; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_hwi_loadstart				; Set DP
    MOVL ACC,@_hwi_loadend     			; Move Load End Address in ACC  
    SUBL ACC,@_hwi_loadstart			; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_hwi_loadstart			; Store Load Starting Address in XAR6
    MOVL XAR7,#_hwi_runstart			; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_hwi_vec_loadstart			; Set DP
    MOVL ACC,@_hwi_vec_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_hwi_vec_loadstart		; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_hwi_vec_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_hwi_vec_runstart		; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_rtdx_text_loadstart		; Set DP
    MOVL ACC,@_rtdx_text_loadend     	; Move Load End Address in ACC  
    SUBL ACC,@_rtdx_text_loadstart		; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_rtdx_text_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_rtdx_text_runstart		; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_sysinit_loadstart			; Set DP
    MOVL ACC,@_sysinit_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_sysinit_loadstart		; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_sysinit_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_sysinit_runstart		; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero

	MOVW DP,#_trcdata_loadstart			; Set DP
    MOVL ACC,@_trcdata_loadend     		; Move Load End Address in ACC  
    SUBL ACC,@_trcdata_loadstart		; Subtract Load Start Address to obtain Load Size
	MOVL XAR6,#_trcdata_loadstart		; Store Load Starting Address in XAR6
    MOVL XAR7,#_trcdata_runstart		; Store Run Address in XAR7
    BF 	 copy,NEQ						; Branch to Copy if ACC is not Zero
    LB _c_int00				 			; Branch to start of boot.asm in RTS library

copy:	
	RPT AL								; Repeat Copy Instruction Until Complete Section is Copied
    || PWRITE  *XAR7, *XAR6++			; Copy From Load Address to Run Address
	LRETR								; Return 

	.end
	
;//===========================================================================
;// End of file.
;//===========================================================================