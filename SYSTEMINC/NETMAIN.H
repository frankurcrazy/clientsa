#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// ゲ`ムサ`バ`の管理用造体
struct gameserver
{
	char ipaddr[128];		// IPアドレス
	char port[64];			// ポ`ト
};


#define SUCCESSFULSTR	"successful"        // プロトコルで使う文字定数群
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  ネットワ`クのすべてのところでつかうあきらめrg

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


// エラ`メッセ`ジ
#define NET_ERRMSG_SOCKLIBERROR		"SOCKLIBERROR" //"ネットワ`クの浃隼搐皮い蓼护蟆" 网络的准备都没有
#define NET_ERRMSG_BADNAME          "服务器名不正确" //"サ`バ名が不正です。" 服务器名不正确。“
#define NET_ERRMSG_SOCKETERROR      "SOCKETERROR" //"ネットワ`クソケット(TCP)を作成することができません。"
#define NET_ERRMSG_NOTGETADDR		"服务器的IP地址没能够得到(%s)" //"サ`バのIPアドレスを得ることができません。(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"服务器连接错误" //"サ`バに接AするI理を_始することができません。"
#define NET_ERRMSG_NOTCONNECT		"不能连接到服务器" //"サ`バに接Aすることができません。 "
#define NET_ERRMSG_CONNECTTIMEOUT	"连接服务器超时" //"サ`バ接A中にタイムアウトしました。" 
#define NET_ERRMSG_LOGINTIMEOUT		"进入游戏超时" //"ログインI理中にタイムアウトしました。"
#define NET_ERRMSG_CHARLISTTIMEOUT	"人物列表获取超时" //"キャラクタリストを取得中にタイムアウトしました。"
#define NET_ERRMSG_LOGOUTTIMEOUT	"退出处理超时" //"ログアウトI理中にタイムアウトしました。"
#define NET_ERRMSG_LOGINFAIL		"登陆失败" //"ログイン出来ませんでした。"
#define NET_ERRMSG_CREATECHARTIMEOUT	"角色创造超时" //"キャラクタを作成中にタイムアウトしました。"
#define NET_ERRMSG_DELETECHARTIMEOUT    "人物删除超时" //"キャラクタを削除中にタイムアウトしました。"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "退出失败" //"ログアウトI理がサ`バに拒否されました。"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "服务器的名字没有指定" //"サ`バの名前が指定されていません。"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "NEEDCHOOSE" //"サ`バをxkしてからでないとこのI理はg行できません。" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "磁盘不足信息保存失败" //"ユ`ザ情螭颔榨ˉぅ毪吮４妞扦蓼护蟆％钎％攻慰栅萘郡悚辘蓼护蟆"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "登陆失败" //"サ`バにログインできません。"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "登陆超时" //"サ`バにログインするI理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "CANNOTLOADPASSWD" //"まだユ`ザ情螭肓Δ丹欷皮い胜い韦恰靴攻铹`ドを涓工毪长趣悉扦蓼护蟆￥蓼亥姗`ザ`情螭蛉肓Δ筏皮坤丹ぁ"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "CANNOTSAVEPASSWD" //"ユ`ザ情螭颔榨ˉぅ毪吮４妞扦胜盲郡韦恰靴攻铹`ド涓耸・筏蓼筏俊"
#define GAMESTATE_ERRMSG_CHPASSERROR      "修改密码错误" //"パスワ`ドの涓扦蓼护螭扦筏俊"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "修改密码超时" //"パスワ`ド涓I理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "删除角色被拒绝" //"キャラ削除I理がサ`バに拒否されました。"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "角色列表获取被拒绝" //"キャラクタ`表取得I理がサ`バに拒否されました。"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"登陆超时" //"ログインI理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "功能没实现" //"まだそのC能はg装されてないなり"
#endif


#endif


/*
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
*/

