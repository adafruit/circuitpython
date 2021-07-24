// *****************************************************************************
// crp.c
//
// Source file to create CRP word expected by LPCXpresso IDE linker
// *****************************************************************************
//
// Copyright(C) NXP Semiconductors, 2013, 2020
// All rights reserved.
//
// NXP Confidential. This software is owned or controlled by NXP and may only be
// used strictly in accordance with the applicable license terms.
//
// By expressly accepting such terms or by downloading, installing, activating
// and/or otherwise using the software, you are agreeing that you have read, and
// that you agree to comply with and are bound by, such license terms.
//
// If you do not agree to be bound by the applicable license terms, then you may not
// retain, install, activate or otherwise use the software.
// *****************************************************************************

#if defined(__CODE_RED)
#include <NXP/crp.h>
// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;
#endif
