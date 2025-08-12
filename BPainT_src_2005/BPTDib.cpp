// BPTDib.cpp: implementation of the BPTDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bpaint.h"
#include "BPTDib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(BPTDIB_TRACE_DIB_CREATION)

namespace BPT {

	int g_ExistingDibSections = 0;

};

#endif

