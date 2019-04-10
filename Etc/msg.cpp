//
// msg.cpp
//
#include "stdafx.h"
#include "msg.h"



static CEdit *edt_msg_stdout = 0;

CEdit *  msgSetStdout(CEdit * tgt)
{
	CEdit *e_prev = edt_msg_stdout;
	edt_msg_stdout = tgt;
	return e_prev;
}


void msgCEdit(CEdit *e, char const *a)
{
	CString aa;
	e->GetWindowText(aa);
	aa += "\r\n";
	aa += a;
	e->SetWindowTextA(aa);
}

static void msgLo(eDiagLevel lvl, const char *a)
{
	if(edt_msg_stdout)
	{
		msgCEdit(edt_msg_stdout, a);
	}else{
		MessageBox(NULL, a, "Message", MB_OK);
	}
}

void OneTrueOops(const char *a, const char *b, const char *c)
{
	CString aa;
	CString sep = "  ";
	aa = a;
	if(b)
		aa += sep + b;
	if(c)
		aa += sep + c;

	MessageBox(NULL,aa,"FATAL ERROR (Sorry)", MB_OK);
	TerminateProcess(GetCurrentProcess(), -1);
}

void diag(eDiagLevel lvl, char const *a, char const *b, char const *c, char const *d, char const *e, char const *f)
{
	CString aa;
	CString sep = "  ";
	aa = a;
	if(b)
		aa += sep + b;
	if(c)
		aa += sep + c;

	if(d)
		aa += sep + d;

	if(e)
		aa += sep + e;

	if(f)
		aa += sep + f;

	msgLo(lvl, aa);
}


void msg(char const *a, char const *b, char const *c, char const *d, char const *e, char const *f)
{
	diag(DiagLevelInfo, a,b,c,d,e,f);
}

void warn(char const *a, char const *b, char const *c, char const *d, char const *e, char const *f)
{
	diag(DiagLevelWarn, a,b,c,d,e,f);
}


void msg(const char *a, int b)
{
	const char *fmt = "%d";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}


void msg(const char *a, long b)
{
	const char *fmt = "%ld";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}

void msg(const char *a, unsigned b)
{
	const char *fmt = "%ld";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}

void msg(const char *a, DWORD b)
{
	const char *fmt = "0x%08x";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}

void msg(const char *a, float b)
{
	const char *fmt = "%.4f";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}

void msg(const char *a, double b)
{
	const char *fmt = "%.8lf";
	CString bb;
	bb.Format(fmt,b);
	msg(a,bb);
}


void warn(const char *a, int b)
{
	const char *fmt = "%d";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}

void warn(const char *a, long b)
{
	const char *fmt = "%ld";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}

void warn(const char *a, unsigned b)
{
	const char *fmt = "%d";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}

void warn(const char *a, DWORD b)
{
	const char *fmt = "0x%08x";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}

void warn(const char *a, float b)
{
	const char *fmt = "%.4f";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}

void warn(const char *a, double b)
{
	const char *fmt = "%.8lf";
	CString bb;
	bb.Format(fmt,b);
	warn(a,bb);
}


