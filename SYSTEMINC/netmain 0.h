#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// Q[T[o[p\
struct gameserver
{
	char ipaddr[128];		// IPAhX
	char port[64];			// |[g
};


#define SUCCESSFULSTR	"successful"        // vgRgQ
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  lbg[N

#define NETBUFSIZ (1024*16)

extern char net_writebuf[];
extern char net_readbuf[];
extern int net_readbuflen;
extern int net_writebuflen;

extern unsigned int sockfd;
extern int server_choosed;
extern int init_net;

extern struct gameserver gmsv[];

#ifdef _LOG_MSG
extern char debugLogFileName[];
#endif


extern BOOL disconnectServerFlag;
extern BOOL oldDisconnectServerFlag;


void ipAddressAnalyze( LPSTR );
int getServerMax( void );
int getServerInfo( int, char *, short * );


void networkLoop(void);
void networkMycharWalk( int gx , int gy , char *direction );

int initNet(void);
void cleanupNetwork( void );
int appendReadBuf( char *buf , int size );
int appendWriteBuf( int index , char *buf , int size );
int shiftReadBuf(  int size );
int shiftWriteBuf( int size );
int getLineFromReadBuf( char *output  , int maxlen );
int sendn( unsigned int s , char *buffer , int len );



int getServerInfoByServerName( char *servername , char *hostname , short *port );


// G[bZ[W
#define NET_ERRMSG_SOCKLIBERROR		"呼隔|ゼ非称nC" //"lbg[NoB"
#define NET_ERRMSG_BADNAME          "A竟W嘿ぃタTC" //"T[osB"
#define NET_ERRMSG_SOCKETERROR      "Lk暗ΘTCPC" //"lbg[N\Pbg(TCP)B"
#define NET_ERRMSG_NOTGETADDR		"Lk莉oA竟⒆⑥mC(%s)" //"T[oIPAhXB(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"Lk}liA竟s钡C" //"T[oJnB"
#define NET_ERRMSG_NOTCONNECT		"Lks钡A竟C" //"T[oB "
#define NET_ERRMSG_CONNECTTIMEOUT	"A竟s钡丁wC" //"T[o^CAEgB" 
#define NET_ERRMSG_LOGINTIMEOUT		"nXBz丁wC" //"OC^CAEgB"
#define NET_ERRMSG_CHARLISTTIMEOUT	"弄H丁wC" //"LN^Xg^CAEgB"
#define NET_ERRMSG_LOGOUTTIMEOUT	"nXBz丁wC" //"OAEg^CAEgB"
#define NET_ERRMSG_LOGINFAIL		"LknJA竟C" //"OCoB"
#define NET_ERRMSG_CREATECHARTIMEOUT	"Hs@丁wC" //"LN^^CAEgB"
#define NET_ERRMSG_DELETECHARTIMEOUT    "Hd埃丁wC" //"LN^^CAEgB"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "A竟┶荡BznXC" //"OAEgT[oB"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "SΤwA竟W嘿C" //"T[oOwB"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "ぃ匡拒A竟hLki赣兜BzC" //"T[oIsB" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "LkOsㄏノ薄厨C合盒eqぃìC" //"[Ut@CBfBXNeB"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "LknJA竟C" //"T[oOCB"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "nJA竟丁wC" //"T[oOC^CAEgB"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "パ|ゼ块Jㄏノ薄厨ALk跑KXC叫块Jㄏノ薄厨C" //"[UApX[hXB[U[B"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "LkOsㄏノ薄厨AKX跑ア毖C" //"[Ut@CApX[hXsB"
#define GAMESTATE_ERRMSG_CHPASSERROR      "Lk跑KXC" //"pX[hXB"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "KX跑丁wC" //"pX[hX^CAEgB"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "A竟┶荡Bzd埃HC" //"LT[oB"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "A竟┶荡Bz弄HC" //"LN^[\T[oB"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"nJBz丁wC" //"OC^CAEgB"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "|ゼ杆m赣\C" //"@\"
#endif


#endif
