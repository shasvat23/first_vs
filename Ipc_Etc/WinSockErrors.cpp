//
// WinSockErrors.cpp
//

#include "stdafx.h"
#include "AfxSock.h"
#include "WinSockErrors.h"


LPCTSTR szWinSockErrName(DWORD dwErr)
{
	// SEE WINSOCK.H

#if 0

	FormatMessageA(dwErr, lpSource, dwErr, dwLangId, LPSTR buffer, nSize, va_list *arguments);


#else // Older code
	static char buf[55];
	static char *a = NULL;
	switch(dwErr)
	{
	default:	sprintf(buf,"Err=%d (0x%x)",dwErr,dwErr); a = buf; break;

	case 0: a = "<Success>"; break;

/*
 * Windows Sockets definitions of regular Microsoft C error constants
 */
	case WSAEINTR:	a = "WSAEINTR"; break;
	case WSAEBADF:	a = "WSAEBADF"; break;
	case WSAEACCES: a = "WSAEACCES"; break;
	case WSAEFAULT: a = "WSAEFAULT"; break;
	case WSAEINVAL: a = "WSAEINVAL"; break;
	case WSAEMFILE: a = "WSAEMFILE"; break;
		
/*
 * Windows Sockets definitions of regular Berkeley error constants
 */
	case WSAEWOULDBLOCK :	a = "WSAEWOULDBLOCK";		break;
	case WSAEINPROGRESS	:	a = "WSAEINPROGRESS   ";	break;
	case WSAEALREADY:		a = "WSAEALREADY";			break;
	case WSAENOTSOCK	:	a = "WSAENOTSOCK";			break;

	case WSAEDESTADDRREQ:	a = "WSAEDESTADDRREQ";		break;
	case WSAEMSGSIZE:		a = "WSAEMSGSIZE";			break;
	case WSAEPROTOTYPE:		a = "WSAEPROTOTYPE";		break;
	case WSAENOPROTOOPT:	a = "WSAENOPROTOOPT";		break;

	case WSAEPROTONOSUPPORT: a = "WSAEPROTONOSUPPORT";	break;
	case WSAESOCKTNOSUPPORT: a = "WSAESOCKTNOSUPPORT";	break;
	case WSAEOPNOTSUPP:		a = "WSAEOPNOTSUPP";		break;
	case WSAEPFNOSUPPORT:	a = "WSAEPFNOSUPPORT";		break;

	case WSAEAFNOSUPPORT:	a = "WSAEAFNOSUPPORT";		break;
	case WSAEADDRINUSE:		a = "WSAEADDRINUSE";		break;
	case WSAEADDRNOTAVAIL:	a = "WSAEADDRNOTAVAIL";		break;
	case WSAENETDOWN:		a = "WSAENETDOWN";			break;

	case WSAENETUNREACH:	a = "WSAENETUNREACH";		break;
	case WSAENETRESET:		a = "WSAENETRESET";			break;
	case WSAECONNABORTED:	a = "WSAECONNABORTED";		break;
	case WSAECONNRESET:		a = "WSAECONNRESET";		break;

	case WSAENOBUFS:		a = "WSAENOBUFS";			break;
	case WSAEISCONN:		a = "WSAEISCONN";			break;
	case WSAENOTCONN:		a = "WSAENOTCONN";			break;
	case WSAESHUTDOWN:		a = "WSAESHUTDOWN";			break;

	case WSAETOOMANYREFS:	a = "WSAETOOMANYREFS";	break;
	case WSAETIMEDOUT:		a = "WSAETIMEDOUT";			break;
	case WSAECONNREFUSED:	a = "WSAECONNREFUSED";		break;
	case WSAELOOP:			a = "WSAELOOP";				break;

	case WSAENAMETOOLONG:	a = "WSAENAMETOOLONG";		break;
	case WSAEHOSTDOWN:		a = "WSAEHOSTDOWN";			break;
	case WSAEHOSTUNREACH:	a = "WSAEHOSTUNREACH";		break;
	case WSAENOTEMPTY:		a = "WSAENOTEMPTY";			break;

	case WSAEPROCLIM:		a = "WSAEPROCLIM";		break;
	case WSAEUSERS:			a = "WSAEUSERS";		break;
	case WSAEDQUOT:			a = "WSAEDQUOT";		break;
	case WSAESTALE:			a = "WSAESTALE";		break;

	case WSAEREMOTE:		a = "WSAEREMOTE";		break;
	case WSAEDISCON:		a = "WSAEDISCON";		break;
	
/*
 * Extended Windows Sockets error constant definitions
 */

	case WSASYSNOTREADY:		a = "WSASYSNOTREADY";		break;
	case WSAVERNOTSUPPORTED:	a = "WSAVERNOTSUPPORTED";	break;
	case WSANOTINITIALISED:		a = "WSANOTINITIALISED";	break;

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (when using the resolver). Note that these errors are
 * retrieved via WSAGetLastError() and must therefore follow
 * the rules for avoiding clashes with error numbers from
 * specific implementations or language run-time systems.
 * For this reason the codes are based at WSABASEERR+1001.
 * Note also that [WSA]NO_ADDRESS is defined only for
 * compatibility purposes.
 */
	case WSAHOST_NOT_FOUND: a = "WSAHOST_NOT_FOUND  Host not found"; break;
	case WSATRY_AGAIN:		a = "WSATRY_AGAIN  Host not found, or SERVERFAIL"; break;
	case WSANO_RECOVERY:	a = "WSANO_RECOVERY Non recoverable errors, FORMERR, REFUSED, NOTIMP"; break;
	case WSANO_DATA:		a = "WSANO_DATA  Valid name, no data record of requested type"; break;
//	case WSANO_ADDRESS: a = "WSANO_ADDRESS  no address, look for MX record"; break;
		// alas WSANO_ADDRES is defined to WSANO_DATA see WInsock.h
#if 0
	case xx: a = "xx"; break;
	case xx: a = "xx"; break;
	case xx: a = "xx"; break;

	case xx: a = "xx"; break;
	case xx: a = "xx"; break;
	case xx: a = "xx"; break;

	case xx: a = "xx"; break;
	case xx: a = "xx"; break;
	case xx: a = "xx"; break; 
#endif
	}

#ifdef _UNICODE
	static CString aa;

	aa = a;

	return aa;
#else

 	return a;
#endif

#endif // Older code
	
}

// EOF 