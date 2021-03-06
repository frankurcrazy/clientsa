#ifndef _NETMAIN_H_
#define _NETMAIN_H_

// ゲームサーバーの管理用構造体
struct gameserver
{
	char ipaddr[128];		// IPアドレス
	char port[64];			// ポート
};


#define SUCCESSFULSTR	"successful"        // プロトコルで使う文字定数群
#define FAILEDSTR		"failed" 
#define OKSTR           "ok"

#define TIMEOUT (600*1000)                   //  ネットワークのすべてのところでつかうあきらめ時間

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


// エラーメッセージ
#define NET_ERRMSG_SOCKLIBERROR		"ｺ�ｸ�ｩ|･ｼｷﾇｳﾆｦn｡C" //"ネットワークの準備が出来ていません。"
#define NET_ERRMSG_BADNAME          "ｦ�ｪAｾｹｦWｺﾙ､｣･ｿｽT｡C" //"サーバ名が不正です。"
#define NET_ERRMSG_SOCKETERROR      "ｵLｪkｰｵｦｨTCP｡C" //"ネットワークソケット(TCP)を作成することができません。"
#define NET_ERRMSG_NOTGETADDR		"ｵLｪkﾀ�ｱoｦ�ｪAｾｹｪｺ｢ﾗ｢ﾞｦ�ｸm｡C(%s)" //"サーバのIPアドレスを得ることができません。(%s)"
#define NET_ERRMSG_NOTCONNECT_S		"ｵLｪkｶ}ｩlｶiｦ讎�ｪAｾｹｪｺｳsｱｵ｡C" //"サーバに接続する処理を開始することができません。"
#define NET_ERRMSG_NOTCONNECT		"ｵLｪkｳsｱｵｦ�ｪAｾｹ｡C" //"サーバに接続することができません。 "
#define NET_ERRMSG_CONNECTTIMEOUT	"ｦ�ｪAｾｹｳsｱｵｮﾉｶ｡､wｨ�｡C" //"サーバ接続中にタイムアウトしました。" 
#define NET_ERRMSG_LOGINTIMEOUT		"ｵn･XｳBｲzｮﾉｶ｡､wｨ�｡C" //"ログイン処理中にタイムアウトしました。"
#define NET_ERRMSG_CHARLISTTIMEOUT	"ﾅｪｨ�､Hｪｫｪ�ｮﾉｶ｡､wｨ�｡C" //"キャラクタリストを取得中にタイムアウトしました。"
#define NET_ERRMSG_LOGOUTTIMEOUT	"ｵn･XｳBｲzｮﾉｶ｡､wｨ�｡C" //"ログアウト処理中にタイムアウトしました。"
#define NET_ERRMSG_LOGINFAIL		"ｵLｪkｵn､Jｦ�ｪAｾｹ｡C" //"ログイン出来ませんでした。"
#define NET_ERRMSG_CREATECHARTIMEOUT	"､Hｪｫｻsｧ@ｮﾉｶ｡､wｨ�｡C" //"キャラクタを作成中にタイムアウトしました。"
#define NET_ERRMSG_DELETECHARTIMEOUT    "､Hｪｫｫdｰ｣ｮﾉｶ｡､wｨ�｡C" //"キャラクタを削除中にタイムアウトしました。"



#define GAMESTATE_ERRMSG_LOGOUTDENY        "ｦ�ｪAｾｹｩﾚｵｴｳBｲzｵn･X｡C" //"ログアウト処理がサーバに拒否されました。"

#if 0
#define GAMESTATE_ERRMSG_NEEDSERVERNAME   "ｨSｦｳｫ�ｩwｦ�ｪAｾｹｦWｺﾙ｡C" //"サーバの名前が指定されていません。"
#define GAMESTATE_ERRMSG_NEEDCHOOSE       "､｣･�ｿ�ｾﾜｦ�ｪAｾｹｫhｵLｪkｶiｦ貂ﾓｶｵｳBｲz｡C" //"サーバを選択してからでないとこの処理は実行できません。" 
#define GAMESTATE_ERRMSG_CANTSAVEUSERINFO "ｵLｪkｫOｦsｨﾏ･ﾎｪﾌｪｺｱ｡ｳ�｡Cｺﾏｺﾐｮeｶq､｣ｨｬ｡C" //"ユーザ情報をファイルに保存できません。ディスクの空き容量が足りません。"
#define GAMESTATE_ERRMSG_CLIENTLOGINERROR "ｵLｪkｵn､Jｦ�ｪAｾｹ｡C" //"サーバにログインできません。"
#define GAMESTATE_ERRMSG_CLIENTLOGINTIMEOUT "ｵn､Jｦ�ｪAｾｹｮﾉｶ｡､wｨ�｡C" //"サーバにログインする処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_CANNOTLOADPASSWD "･ﾑｩ�ｩ|･ｼｿ鬢Jｨﾏ･ﾎｪﾌｱ｡ｳ�｡AｵLｪkﾅﾜｧ�ｱKｽX｡Cｽﾐ･�ｿ鬢Jｨﾏ･ﾎｪﾌｱ｡ｳ�｡C" //"まだユーザ情報が入力されていないので、パスワードを変更することはできません。まずユーザー情報を入力してください。"
#define GAMESTATE_ERRMSG_CANNOTSAVEPASSWD "ｵLｪkｫOｦsｨﾏ･ﾎｪﾌｱ｡ｳ�｡AｱKｽXﾅﾜｧ�･｢ｱﾑ｡C" //"ユーザ情報をファイルに保存できなかったので、パスワード変更に失敗しました。"
#define GAMESTATE_ERRMSG_CHPASSERROR      "ｵLｪkﾅﾜｧ�ｱKｽX｡C" //"パスワードの変更ができませんでした。"
#define GAMESTATE_ERRMSG_CHPASSTIMEOUT    "ｱKｽXﾅﾜｧ�ｮﾉｶ｡､wｨ�｡C" //"パスワード変更処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_DELETECHARSERVERDENY    "ｦ�ｪAｾｹｩﾚｵｴｳBｲzｫdｰ｣､Hｪｫ｡C" //"キャラ削除処理がサーバに拒否されました。"
#define GAMESTATE_ERRMSG_CHARLISTDENY      "ｦ�ｪAｾｹｩﾚｵｴｳBｲzﾅｪｨ�､Hｪｫｪ�｡C" //"キャラクター表取得処理がサーバに拒否されました。"
#define GAMESTATE_ERRMSG_LOGINTIMEOUT			"ｵn､JｳBｲzｮﾉｶ｡､wｨ�｡C" //"ログイン処理がタイムアウトしました。"
#define GAMESTATE_ERRMSG_NOTIMPLEMENTED   "ｩ|･ｼｸﾋｸmｸﾓ･\ｯ爍C" //"まだその機能は実装されてないなり"
#endif


#endif
