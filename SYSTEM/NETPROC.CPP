#include <stdio.h>
#include <winsock.h>
#include <time.h>

#include"system.h"
#include "netmain.h"
#include "netproc.h"
#include "lssproto_cli.h"
#include "savedata.h"
#include "..\systeminc\process.h"
#include "pc.h"
#include "tool.h"
#include "map.h"
#include "character.h"
#include "action.h"
#include "battlemenu.h"
#include "battleProc.h"
#include "battleMap.h"
#include "menu.h"
#include "anim_tbl.h"
#include "login.h"
#include "handletime.h"
#include "field.h"
#include "t_music.h"

#define NETPROC_NOTSEND     0           // ?????? 发送前
#define NETPROC_SENDING     1           // ???????? 中
#define NETPROC_RECEIVED    2           //??????    接受

#define SET_SENDTIME( time )		time = GetTickCount()
#define CHECK_TIMEOUT( time, timeout )		\
			if( (GetTickCount() - (time)) > (timeout) )	\
			{											\
				return -1;								\
			}





#define SETSENDING  netproc_sending = NETPROC_SENDING;start_time=GetTickCount();
#define SETTIMEOUT( msg , state )  \
	if( ( GetTickCount() - start_time ) > TIMEOUT )	\
	{												\
		return;										\
	}

#define SETTIMEOUT2( msg )  \
	if( ( GetTickCount() - start_time ) > TIMEOUT )	\
	{												\
		sprintf( netprocErrmsg, msg );				\
		return -1;									\
	}

#define SETNEEDCHOOSE( mode )						\
	if( server_choosed == 0 )						\
	{												\
		return;										\
	}

/*
// ?????f??????@?StoneAge??1?????p?
short selectServerIndex = -1;
// ??????????????????..??
short clientLoginStatus = 0;
// ?????????????????...??/ 
short charListStatus = 0;
// /????ID ??????
char userId[32];
char userPassword[32];
// ???????????
short charDelStatus = 0;
// ????
short newCharStatus = 0;
// ????
short charLoginStatus = 0;
// ????
short charLogoutStatus = 0;
*/
// 指数所选择的服务器（StoneAge的1或0的情况下
short selectServerIndex = -1;
// 登录（登录一段时间...）
short clientLoginStatus = 0;
// 人物列表状态（收购一...）/ 
short charListStatus = 0;
// /用户ID ・密码
char userId[32];
char userPassword[32];
// 删除人物状态
short charDelStatus = 0;
// 新建
short newCharStatus = 0;
// 记录
short charLoginStatus = 0;
// 退出
short charLogoutStatus = 0;

// GAMESTATE_CHOOSESERVER//???????????   信息，必须配置
// "おりおん"とか"ぺがさす"をいれることになるだろう。
char gamestate_chooseserver_name[128];
// GAMESTATE_LOGIN???????????   信息，必须配置
char gamestate_login_charname[128];
// GAMESTATE_DELETECHAR???????????   信息，必须配置
char gamestate_deletechar_charname[128];


// ?F}?    错误消息
char netprocErrmsg[1024];

int netproc_sending = NETPROC_NOTSEND; 
DWORD start_time = 0;               // /?L????????h????????????? TIMEOUT
//进程启动时的所有时间。超时通过 TIMEOUT



// サ`バ`xkI理 ///////////////////////////////////////////////////////
//???n?????///
//?????????? /????
// サ`バ接A
// 初期化
//服务器选择过程///
//连接到服务器/ /初始化

int connectServerCounter = 0;

void initConnectServer( void )
{
	connectServerCounter = 0;
}


// 接AI理  /??????
//
// ?? selectServerIndex 、userId 、userPassword 
//
//  ????	     0 ... ?i???
//				 1 ... ????OK
//				-1 ... ????
//				-2 ... ???????????????`L????????P??S??盥??N???
//				-3 ... ??????????????
//				-4 ... IP??????????
//				-5 ... connect????????
//				-6 ... select??????
//				-7 ... ????????????????  ????????
//处理连接
//
// 设置 selectServerIndex AuserId AuserPassword 
//
//  返回	     0 ... 进行中
//				 1 ... 连接OK
//				-1 ... 超时
//				-2 ... 有没有指定的服务器名称（通常这种情况不会发生。）
//				-3 ... 无法创建套接字
//				-4 ... IP地址转换损失
//				-5 ... connect连接失败
//				-6 ... select选择失败
//				-7 ... 客户端登录失败。  超级重点
int connectServer( void )
{
	// 初期化が完了してなければ何もしない
	if( !init_net )
		return 0;

	if( connectServerCounter == 0 )
	{
		//???????????????             ????
		//记录开始连接时             看到
		start_time = GetTickCount();//获得机器启动到现在的时间
		//start_time = GetTickCount();//???c?????????????

			// 应该为非阻塞套接字，连接到该国 服务  
		// ?????????霾????????????????????  
		char hostname[128];
		short pt;
		if( getServerInfo( selectServerIndex, hostname, &pt ) < 0 )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_BADNAME );
			//connectServerCounter = 0;
			return -2;
		}

        // socketつくru
		sockfd = socket( AF_INET , SOCK_STREAM ,0 );
		if( sockfd == INVALID_SOCKET )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_SOCKETERROR );
			//connectServerCounter = 0;
			return -3;
		}

        // ノンブロッキングにする
#if 1
		unsigned long flg = 1;
		//修改套接字的阻塞模式 1为非阻塞 0阻塞
		ioctlsocket( sockfd, FIONBIO, &flg );//?????????????霾??? 1?????霾?0?霾?
#endif
		// NoDelay の龊悉稀setsockoptする。 by ringo  ??setsockopt??????
		//设置setsockopt的事件
		extern BOOL NoDelay;
		if( NoDelay ){
			int flag = 1;
			if( setsockopt( sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(int)) != 0 ){
				//MessageBox( NULL, "fuck", "ringo", MB_OK );
				return -100;
			}
		}
#if 0
#ifdef _DEBUG
		{
			extern int CheckNetErrror( void );
			unsigned char time = 1;
			int len = 1;
			char moji[256];
			// ??????????
			// 设置接收超时
			if( getsockopt( sockfd, IPPROTO_TCP, SO_RCVTIMEO, (char*) &time,  &len ) == SOCKET_ERROR ){
				CheckNetErrror();
				//return -100;
			}
			sprintf( moji, "时间:%u", time );
			MessageBox( NULL, moji, "ohta", MB_OK );
		}
#endif
#endif


		// ??????	// 设置地址
		struct sockaddr_in sin;
		struct hostent *h;

		sin.sin_family = AF_INET;
		sin.sin_port = htons( pt );
		sin.sin_addr.s_addr = inet_addr( hostname );     /* accept only dot notaion  */
		if( sin.sin_addr.s_addr == -1 )
		{
			h = gethostbyname( hostname );
			if( h == NULL )
			{
				sprintf( netprocErrmsg, NET_ERRMSG_NOTGETADDR, hostname );
				closesocket( sockfd ); 
				//connectServerCounter = 0;
				return -4;
			}
			memcpy( (void*) &sin.sin_addr.s_addr , h->h_addr, sizeof( struct in_addr ) );
		}

		// Non blocking Connect
		int ret = connect( sockfd, (struct sockaddr*)&sin , sizeof( sin ) );
		if( ret == SOCKET_ERROR )
		{
			if( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				// 本来はブロックするはずだったんやけど ?F????????? 错误应该应该阻止
            }
			else
			{
				closesocket( sockfd );
				sprintf( netprocErrmsg, NET_ERRMSG_NOTCONNECT_S );
				return -5;
			}
		}

		// ここまでできたらカウンタを1にする  ????????  连接计数
		connectServerCounter = 1;
	}
	else
	if( connectServerCounter >= 1 && connectServerCounter <= 70 )
	{
		connectServerCounter++;
		if( connectServerCounter == 70 )
		{
			connectServerCounter = 69;
		}
		// selectする  ????  	// select  选择
		fd_set rfds , wfds , efds;
		struct timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 0;
		FD_ZERO( &rfds );
		FD_ZERO( &wfds );
		FD_ZERO( &efds );
		FD_SET( sockfd , &rfds );
		FD_SET( sockfd , &wfds );
		FD_SET( sockfd , &efds );
		int a = select(  2 , &rfds , &wfds , &efds,  &tm );
		if( FD_ISSET( sockfd, &wfds ) )
		{
			connectServerCounter = 71;
			//?????????隆[?????c???これ以降はネットワ`クル`チンが生きる。そのためのフラグ立て
				//从现在的生活网络程序
			server_choosed = 1;
		}
		if( FD_ISSET( sockfd , &efds ) )
		{
			sprintf( netprocErrmsg, NET_ERRMSG_NOTCONNECT );
			closesocket( sockfd ); 
			//connectServerCounter = 0;
			return -6; 
		}
    }
	else
	if( connectServerCounter >= 71 && connectServerCounter <= 80 )
	{
		// ClientLoginする ????????   连接。。
		if( connectServerCounter == 71 )
		{
			// 关闭在线状态
			clientLoginStatus = 0;	// ????????????
			lssproto_ClientLogin_send( sockfd, userId, userPassword );
			netproc_sending = NETPROC_SENDING;
		}
		if( netproc_sending == NETPROC_RECEIVED )
		{
			//  recv ????????已经成功
			if( clientLoginStatus )
			{
				connectServerCounter = 81;
			}
			else
			{
				netproc_sending = NETPROC_NOTSEND;
				sprintf( netprocErrmsg, NET_ERRMSG_LOGINFAIL );
				closesocket( sockfd ); 
				return -7;
			}
		}

		connectServerCounter++;
		if( connectServerCounter == 81 )
			connectServerCounter = 80;
    }
	else
	if( connectServerCounter >= 81 && connectServerCounter <= 98 )
	{
		// まわすだけ  ????转动
		connectServerCounter ++;
    }
	else
	if( connectServerCounter == 99 )
	{
		//ChangeProc( PROC_CHAR_SELECT );
		// 切换回传送与否 我可以谈谈什么 

		// ??????????????????????????? また送信していないモ`ドに埂：位丐筏皮猡いい瑜Δ
		netproc_sending = NETPROC_NOTSEND;
		//connectServerCounter = 0;
		return 1;
	}

	// 制限rgがすぎたら、エラ`状Bに移行、そのあとエラ`K了
	//???犒????C??????恝????????F??F???
	//太多的时间限制，国家过渡的错误，则错误退出
	SETTIMEOUT2( NET_ERRMSG_CONNECTTIMEOUT );

	return 0;
}




// クライアントログイン ???????? ????
void clientLogin( void )
{
	char cdkey[64];
	char passwd[64];

	getCdkey( cdkey );
	getPassword( passwd );
	lssproto_ClientLogin_send( sockfd , cdkey , passwd );
	netproc_sending = NETPROC_SENDING;
}


// ログイン受信I理  客户登陆 重点
void lssproto_ClientLogin_recv( int fd, char *result )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;	

		if( strcmp( result, OKSTR ) == 0 )
		{
			clientLoginStatus = 1;

			// F在の日rを取得
			time( &serverAliveLongTime );
			serverAliveTime = localtime( &serverAliveLongTime );
		}
	}
}




// キャラリスト取得I理 ///////////////////////////////////////////////////

// キャラのリスト取得メッセ`ジを送る
void charListStart( void )
{
	int i;

	for( i = 0; i < MAXCHARACTER; i++ )
	{
		resetCharacterList( i );
	}

	charListStatus = 0;

	lssproto_CharList_send( sockfd );

	SETSENDING;
}


// 受信待ち
//     0 ... 受信待ち
//             1 ... 受信完了
//            -1 ... タイムアウト
//            -2 ... エラ`がってきた
//            -3 ... メンテナンス中
//            -4 ... ユ`ザJ^失
int charListProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charListStatus == 1 )
		{
			return 1;
		}
		else
		if( charListStatus == 2 )
		{
			return -3;
		}
#if 0
		else
		if( charListStatus == 3 )
		{
			return -4;
		}
#endif
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_CHARLISTTIMEOUT );

	return 0;
}


// サ`バ`に保存されているキャラクタのリストの受信
void lssproto_CharList_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) != 0 )
		{
			if( strcmp( data, "OUTOFSERVICE" ) == 0 )
			{
				// F在メンテナンス中
				charListStatus = 2;
			}
#if 0
			else
			if( strcmp( data, "???????" ) == 0 )
			{
				// ユ`ザJ^失
				charListStatus = 3;
			}
#endif
			return;
		}

		charListStatus = 1;

		// gHに情螭颔互氓趣工
		char nm[1024], opt[1024];
		int i;
		for( i=0; i < MAXCHARACTER; i++ )
		{
			strcpy( nm ,"" );
			strcpy( opt , "" );
			getStringToken( data, '|', i*2+1, sizeof( nm )-1 , nm );
			getStringToken( data, '|', i*2+2, sizeof( opt )-1, opt );

			setCharacterList( nm , opt );
		}
	}
}



// 指定キャラでログイン ///////////////////////////////////////////////////
// ログイン_始
void charLoginStart( void )
{
	charLoginStatus = 0;

	lssproto_CharLogin_send( sockfd, gamestate_login_charname );

	SETSENDING;
}


// ログイン中
//    0 ... 受信待ち / 1 ... 受信完了 / -1 ... タイムアウト / -2 ... エラ`がってきた
int charLoginProc( void )
{
	if( !server_choosed )
		return 0;

	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charLoginStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_LOGINTIMEOUT );

	return 0;
}


// キャラログイン受信
void lssproto_CharLogin_recv( int fd, char* result, char* data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charLoginStatus = 1;
        }
	}
}




// ログアウト /////////////////////////////////////////////////////////////
// ログアウト_始
void charLogoutStart( void )
{
	charLogoutStatus = 0;

	lssproto_CharLogout_send( sockfd );

	SETSENDING;
}


// ログアウト中
//   	 0 ... ログアウト中
//				 1 ... ログアウト完了
//				-1 ... タイムアウト
//				-2 ... エラ`がってきた
int charLogoutProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charLogoutStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
    }

	SETTIMEOUT2( NET_ERRMSG_LOGOUTTIMEOUT );

	return 0;
}


void lssproto_CharLogout_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charLogoutStatus = 1;
		}
	}
}




// ステ`タス情螭蚴苄扭工 ///////////////////////////////////////////////
//   ステ`タス情螭蚴苄扭工搿
//   このコ`ドはプロトコルファイルの内容を忠gにg装していなければならない。
//   コ`ドがちょっとでも`反していたらそれはバグであるとしてよい。
//状态信息接收////////
//状态信息/ /接收。/ /这代码协议文件的内容忠实地实施着必须。
//代码稍微违反了的话，那是错误的好。

#define S_DELIM '|'						// デリミタ  定界符

void lssproto_S_recv( int fd, char *data ) 
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	switch( data[0] )
	{
		// 座
		case 'C':
			{
			int fl;
			int maxx;
			int maxy;
			int gx;
			int gy;

			// フロア涓榨楗(マップをiみこむように)
			floorChangeFlag = TRUE;
			if( !loginFlag && ProcNo == PROC_GAME )	// ログインrはg行しない
			{
				// ワ`プイベントですでにg行されていれば何もしない。
				if( !warpEffectFlag )
				{
					// ワ`プ演出g行
					SubProcNo = 200;
					// Fr点の画面を作る
					warpEffectProc();
#if 1
					// マップウィンドウフラグバックアップ
					if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
#endif
				}

				// PCキャラリセット
				resetPc();
				warpEffectFlag = FALSE;
				warpEffectStart = TRUE;
			}


			// カテゴリ文字をwばす//数据分类提取
			data++;

			fl   = getIntegerToken( data, S_DELIM, 1 );
			maxx = getIntegerToken( data, S_DELIM, 2 );
			maxy = getIntegerToken( data, S_DELIM, 3 );
			gx   = getIntegerToken( data, S_DELIM, 4 );
			gy   = getIntegerToken( data, S_DELIM, 5 );

			// マップ位置O定  地图位置设定
			setMap( fl, gx, gy );
			// 新しいマップならファイルを作る  如果/新地图文件  制作/  
			createMap( fl, maxx, maxy );
			// マップサイズのO定  尺寸/ /地图的设定
			nowFloorGxSize = maxx;
			nowFloorGySize = maxy;


			// キャラ管理テ`ブルリセット  角色管理表复位定
			resetCharObj();

			// 空マップフラグ  空地图转折点？
			mapEmptyFlag = FALSE;

			// エンカウント率の初期化  遇敌率的初期化
			nowEncountPercentage = minEncountPercentage;
			nowEncountExtra = 0;

			resetMap();
			}
            break;

		// PCキャラのIDO定
		case 'D':
			// カテゴリ文字をwばす
			data++;

			setPcId( getIntegerToken( data, S_DELIM, 1 ) );
			serverTime = getIntegerToken( data, S_DELIM, 2 );
			clientTime = time( NULL );
			// ＳＡrgを取得
			RealTimeToSATime( &SaTime );
			//ＳＡrgで今のrg区分を得る
			SaTimeZoneNo = getLSTime ( &SaTime );
			// パレットチェンジ
			PaletteChange( SaTimeZoneNo, 0 );
			
			break;

		// パラメ`タ
		case 'P':
			{
			char name[256];
			char freeName[256];
			//char status[256];

			// カテゴリ文字をwばす
			data++;

			int i;
			int kubun;
			unsigned int mask;

			kubun		= getInteger62Token( data, S_DELIM, 1 );

			// 全パラメ`タ受信
			if( kubun == 1 )
			{
				pc.hp		= getIntegerToken( data, S_DELIM, 2 );		// 0x00000002
				pc.maxHp	= getIntegerToken( data, S_DELIM, 3 );		// 0x00000004
				pc.mp		= getIntegerToken( data, S_DELIM, 4 );		// 0x00000008
				pc.maxMp	= getIntegerToken( data, S_DELIM, 5 );		// 0x00000010
				pc.vital	= getIntegerToken( data, S_DELIM, 6 );		// 0x00000020
				pc.str		= getIntegerToken( data, S_DELIM, 7 );		// 0x00000040
				pc.tgh		= getIntegerToken( data, S_DELIM, 8 );		// 0x00000080
				pc.dex		= getIntegerToken( data, S_DELIM, 9 );		// 0x00000100
				pc.exp		= getIntegerToken( data, S_DELIM, 10);		// 0x00000200
				pc.maxExp	= getIntegerToken( data, S_DELIM, 11 );		// 0x00000400
				pc.level	= getIntegerToken( data, S_DELIM, 12 );		// 0x00000800
				pc.atk		= getIntegerToken( data, S_DELIM, 13 );		// 0x00001000
				pc.def		= getIntegerToken( data, S_DELIM, 14 );		// 0x00002000
				pc.quick	= getIntegerToken( data, S_DELIM, 15 );		// 0x00004000
				pc.charm	= getIntegerToken( data, S_DELIM, 16 );		// 0x00008000
				pc.luck		= getIntegerToken( data, S_DELIM, 17 );		// 0x00010000
				pc.earth	= getIntegerToken( data, S_DELIM, 18 );		// 0x00020000
				pc.water	= getIntegerToken( data, S_DELIM, 19 );		// 0x00040000
				pc.fire		= getIntegerToken( data, S_DELIM, 20 );		// 0x00080000
				pc.wind		= getIntegerToken( data, S_DELIM, 21 );		// 0x00100000
				pc.gold		= getIntegerToken( data, S_DELIM, 22 );		// 0x00200000
				pc.titleNo	= getIntegerToken( data, S_DELIM, 23 );		// 0x00400000
				pc.dp		= getIntegerToken( data, S_DELIM, 24 );		// 0x00800000

				getStringToken( data, S_DELIM, 25, sizeof( name )-1, name );// 0x01000000
				makeRecvString( name );
				if( strlen( name ) <= CHAR_NAME_LEN )
					strcpy( pc.name, name );
				getStringToken( data, S_DELIM, 26, sizeof( freeName )-1, freeName );// 0x02000000
				makeRecvString( freeName );
				if( strlen( freeName ) <= CHAR_FREENAME_LEN )
					strcpy( pc.freeName, freeName );
				//getStringToken( data, S_DELIM, 27, sizeof( status )-1, status );// 0x04000000
				//setCharStatus( &pc.status, status );	// ステ`タス
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							pc.hp = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							pc.maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							pc.mp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							pc.maxMp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							pc.vital = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							pc.str = getIntegerToken( data, S_DELIM, i );// 0x00000040
							i++;
						}
						else
						if( mask == 0x00000080 )
						{
							pc.tgh = getIntegerToken( data, S_DELIM, i );// 0x00000080
							i++;
						}
						else
						if( mask == 0x00000100 )
						{
							pc.dex = getIntegerToken( data, S_DELIM, i );// 0x00000100
							i++;
						}
						else
						if( mask == 0x00000200 )
						{
							pc.exp = getIntegerToken( data, S_DELIM, i );// 0x00000200
							i++;
						}
						else
						if( mask == 0x00000400 )
						{
							pc.maxExp = getIntegerToken( data, S_DELIM, i );// 0x00000400
							i++;
						}
						else
						if( mask == 0x00000800 )
						{
							pc.level = getIntegerToken( data, S_DELIM, i );// 0x00000800
							i++;
						}
						else
						if( mask == 0x00001000 )
						{
							pc.atk = getIntegerToken( data, S_DELIM, i );// 0x00001000
							i++;
						}
						else
						if( mask == 0x00002000 )
						{
							pc.def = getIntegerToken( data, S_DELIM, i );// 0x00002000
							i++;
						}
						else
						if( mask == 0x00004000 )
						{
							pc.quick = getIntegerToken( data, S_DELIM, i );// 0x00004000
							i++;
						}
						else
						if( mask == 0x00008000 )
						{
							pc.charm = getIntegerToken( data, S_DELIM, i );// 0x00008000
							i++;
						}
						else
						if( mask == 0x00010000 )
						{
							pc.luck = getIntegerToken( data, S_DELIM, i );// 0x00010000
							i++;
						}
						else
						if( mask == 0x00020000 )
						{
							pc.earth = getIntegerToken( data, S_DELIM, i );// 0x00020000
							i++;
						}
						else
						if( mask == 0x00040000 )
						{
							pc.water = getIntegerToken( data, S_DELIM, i );// 0x00040000
							i++;
						}
						else
						if( mask == 0x00080000 )
						{
							pc.fire = getIntegerToken( data, S_DELIM, i );// 0x00080000
							i++;
						}
						else
						if( mask == 0x00100000 )
						{
							pc.wind = getIntegerToken( data, S_DELIM, i );// 0x00100000
							i++;
						}
						else
						if( mask == 0x00200000 )
						{
							pc.gold = getIntegerToken( data, S_DELIM, i );// 0x00200000
							i++;
						}
						else
						if( mask == 0x00400000 )
						{
							pc.titleNo = getIntegerToken( data, S_DELIM, i );// 0x00400000
							i++;
						}
						else
						if( mask == 0x00800000 )
						{
							pc.dp = getIntegerToken( data, S_DELIM, i );// 0x00800000
							i++;
						}
						else
						if( mask == 0x01000000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x01000000
							makeRecvString( name );
							if( strlen( name ) <= CHAR_NAME_LEN )
								strcpy( pc.name, name );
							i++;
						}
						else
						if( mask == 0x02000000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( freeName )-1, freeName );// 0x02000000
							makeRecvString( freeName );
							if( strlen( freeName ) <= CHAR_FREENAME_LEN )
								strcpy( pc.freeName, freeName );
							i++;
						}
					}
				}
			}

			updataPcAct();

			if( (pc.status & CHR_STATUS_LEADER) != 0
			 && party[0].useFlag != 0 )
			{
				party[0].level = pc.level;
				party[0].maxHp = pc.maxHp;
				party[0].hp = pc.hp;
				strcpy( party[0].name, pc.name );
			}
			}
			break;

		// HP,MP,EXP
		case 'M':
			// カテゴリ文字をwばす
			data++;

			pc.hp  = getIntegerToken( data, '|', 1 );
			pc.mp  = getIntegerToken( data, '|', 2 );
			pc.exp = getIntegerToken( data, '|', 3 );
			updataPcAct();

			if( (pc.status & CHR_STATUS_LEADER) != 0
			 && party[0].useFlag != 0 )
			{
				party[0].hp = pc.hp;
			}
			break;

		// ペットのパラメ`タ
		case 'K':
			{
			char name[256];
			char freeName[256];
			//char status[256];
			int no;
			int kubun;
			no = data[1] - '0';

			// カテゴリ文字をwばす
			data += 3;

			// ペットいないrはK了
			kubun = getInteger62Token( data, S_DELIM, 1 );
			if( kubun == 0 )
			{
				pet[no].useFlag = 0;
				break;
			}

			pet[no].useFlag = 1;

			int i;
			unsigned int mask;

			if( kubun == 1 )
			{
				pet[no].graNo	= getIntegerToken( data, S_DELIM, 2 );		// 0x00000002

				pet[no].hp		= getIntegerToken( data, S_DELIM, 3 );		// 0x00000004
				pet[no].maxHp	= getIntegerToken( data, S_DELIM, 4 );		// 0x00000008
				pet[no].mp		= getIntegerToken( data, S_DELIM, 5 );		// 0x00000010
				pet[no].maxMp	= getIntegerToken( data, S_DELIM, 6 );		// 0x00000020

				pet[no].exp		= getIntegerToken( data, S_DELIM, 7 );		// 0x00000040
				pet[no].maxExp	= getIntegerToken( data, S_DELIM, 8 );		// 0x00000080
				pet[no].level	= getIntegerToken( data, S_DELIM, 9 );		// 0x00000100

				pet[no].atk		= getIntegerToken( data, S_DELIM, 10);		// 0x00000200
				pet[no].def		= getIntegerToken( data, S_DELIM, 11 );		// 0x00000400
				pet[no].quick	= getIntegerToken( data, S_DELIM, 12 );		// 0x00000800
				pet[no].ai		= getIntegerToken( data, S_DELIM, 13 );		// 0x00001000

				pet[no].earth	= getIntegerToken( data, S_DELIM, 14 );		// 0x00002000
				pet[no].water	= getIntegerToken( data, S_DELIM, 15 );		// 0x00004000
				pet[no].fire	= getIntegerToken( data, S_DELIM, 16 );		// 0x00008000
				pet[no].wind	= getIntegerToken( data, S_DELIM, 17 );		// 0x00010000

				pet[no].maxSkill= getIntegerToken( data, S_DELIM, 18 );		// 0x00020000

				pet[no].changeNameFlag = getIntegerToken( data, S_DELIM, 19 );// 0x00040000

				getStringToken( data, S_DELIM, 20, sizeof( name )-1, name );// 0x00080000
				makeRecvString( name );
				if( strlen( name ) <= PET_NAME_LEN )
					strcpy( pet[no].name, name );
				getStringToken( data, S_DELIM, 21, sizeof( freeName )-1, freeName );// 0x00100000
				makeRecvString( freeName );
				if( strlen( freeName ) <= PET_NAME_LEN )
					strcpy( pet[no].freeName, freeName );
//				getStringToken( data, S_DELIM, 22, sizeof( status )-1, status );
//				setCharStatus( &pet[no].status, status );	// ステ`タス
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							pet[no].graNo = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							pet[no].hp = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							pet[no].maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							pet[no].mp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							pet[no].maxMp = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							pet[no].exp = getIntegerToken( data, S_DELIM, i );// 0x00000040
							i++;
						}
						else
						if( mask == 0x00000080 )
						{
							pet[no].maxExp = getIntegerToken( data, S_DELIM, i );// 0x00000080
							i++;
						}
						else
						if( mask == 0x00000100 )
						{
							pet[no].level = getIntegerToken( data, S_DELIM, i );// 0x00000100
							i++;
						}
						else
						if( mask == 0x00000200 )
						{
							pet[no].atk = getIntegerToken( data, S_DELIM, i );// 0x00000200
							i++;
						}
						else
						if( mask == 0x00000400 )
						{
							pet[no].def = getIntegerToken( data, S_DELIM, i );// 0x00000400
							i++;
						}
						else
						if( mask == 0x00000800 )
						{
							pet[no].quick = getIntegerToken( data, S_DELIM, i );// 0x00000800
							i++;
						}
						else
						if( mask == 0x00001000 )
						{
							pet[no].ai = getIntegerToken( data, S_DELIM, i );// 0x00001000
							i++;
						}
						else
						if( mask == 0x00002000 )
						{
							pet[no].earth = getIntegerToken( data, S_DELIM, i );// 0x00002000
							i++;
						}
						else
						if( mask == 0x00004000 )
						{
							pet[no].water = getIntegerToken( data, S_DELIM, i );// 0x00004000
							i++;
						}
						else
						if( mask == 0x00008000 )
						{
							pet[no].fire = getIntegerToken( data, S_DELIM, i );// 0x00008000
							i++;
						}
						else
						if( mask == 0x00010000 )
						{
							pet[no].wind = getIntegerToken( data, S_DELIM, i );// 0x00010000
							i++;
						}
						else
						if( mask == 0x00020000 )
						{
							pet[no].maxSkill = getIntegerToken( data, S_DELIM, i );// 0x00020000
							i++;
						}
						else
						if( mask == 0x00040000 )
						{
							pet[no].changeNameFlag = getIntegerToken( data, S_DELIM, i );// 0x00040000
							i++;
						}
						else
						if( mask == 0x00080000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x00080000
							makeRecvString( name );
							if( strlen( name ) <= PET_NAME_LEN )
								strcpy( pet[no].name, name );
							i++;
						}
						else
						if( mask == 0x00100000 )
						{
							getStringToken( data, S_DELIM, i, sizeof( freeName )-1, freeName );// 0x00100000
							makeRecvString( freeName );
							if( strlen( freeName ) <= PET_NAME_LEN )
							strcpy( pet[no].freeName, freeName );
							i++;
						}
					}
				}
			}

			}
			break;

		// エンカウント率のO定
		case 'E':
			{
			// カテゴリ文字をwばす
			data++;

			minEncountPercentage = getIntegerToken( data, S_DELIM, 1 );
			maxEncountPercentage = getIntegerToken( data, S_DELIM, 2 );
			nowEncountPercentage = minEncountPercentage;
			}
			break;

		// 庑gのパラメ`タ
		case 'J':
			{
			char name[256];
			char memo[256];
			int no;
			no = data[1] - '0';

			// カテゴリ文字をwばす
			data += 3;

			// 庑gがないrはK了
			magic[no].useFlag = getIntegerToken( data, S_DELIM, 1 );
			if( magic[no].useFlag == 0 )
				break;

			magic[no].mp		= getIntegerToken( data, S_DELIM, 2 );
			magic[no].field		= getIntegerToken( data, S_DELIM, 3 );
			magic[no].target	= getIntegerToken( data, S_DELIM, 4 );
			if( magic[no].target >= 100 )
			{
				magic[no].target %= 100;
				magic[no].deadTargetFlag = 1;
			}
			else
			{
				magic[no].deadTargetFlag = 0;
			}

			getStringToken( data, S_DELIM, 5, sizeof( name )-1, name );
			makeRecvString( name );
			if( strlen( name ) <= sizeof( magic[no].name )-1 )
			{
				strcpy( magic[no].name, name );
			}
			getStringToken( data, S_DELIM, 6, sizeof( memo )-1, memo );
			makeRecvString( memo );
			if( strlen( memo ) <= sizeof( magic[no].memo )-1 )
			{
				strcpy( magic[no].memo, memo );
			}
			}
			break;

		// 仲gのパラメ`タ
		case 'N':
			{
			ACTION *ptAct;
			char name[256];
			int no;
			int kubun;
			int i;
			int checkPartyCount;


			no = data[1] - '0';

			// カテゴリ文字をwばす
			data += 3;

			// 仲g情螭胜rはK了
			kubun = getInteger62Token( data, S_DELIM, 1 );
			if( kubun == 0 )
			{
				// さっきまで仲gがいたら後I理してK了
				if( party[no].useFlag != 0 && party[no].id != pc.id )
				{
					ptAct = getCharObjAct( party[no].id );
					if( ptAct != NULL )
					{
						delCharParty( ptAct );
					}
				}
				party[no].useFlag	= 0;
				party[no].ptAct		= NULL;

				// lかがiけたとき残りが自分だけならパ`ティは解散
				checkPartyCount = 0;
				for( i = 0; i < MAX_PARTY; i++ )
				{
					if( party[i].useFlag != 0 )
						checkPartyCount++;
				}
				if( checkPartyCount <= 1 )
				{
					partyModeFlag = 0;
					clearPartyParam();
				}
				break;
			}

			// ここを通るということはPCはパ`ティに入ったことになる
			partyModeFlag = 1;
			prSendFlag = 0;		// たまにPRの返事がこないのでここでフラグを下ろす。

			party[no].useFlag	= 1;

			unsigned int mask;

			if( kubun == 1 )
			{
				party[no].id		= getIntegerToken( data, S_DELIM, 2 );	// 0x00000002
				party[no].level		= getIntegerToken( data, S_DELIM, 3 );	// 0x00000004
				party[no].maxHp		= getIntegerToken( data, S_DELIM, 4 );	// 0x00000008
				party[no].hp		= getIntegerToken( data, S_DELIM, 5 );	// 0x00000010
				party[no].mp		= getIntegerToken( data, S_DELIM, 6 );	// 0x00000020

				getStringToken( data, S_DELIM, 7, sizeof( name )-1, name );	// 0x00000040
				makeRecvString( name );
				if( strlen( name ) <= sizeof( party[no].name )-1 )
				{
					strcpy( party[no].name, name );
				}
				else
				{
					strcpy( party[no].name, "???" );
				}
			}
			else
			{
				mask = 2;
				i = 2;
				for( ; mask > 0; mask <<= 1 )
				{
					if( kubun & mask )
					{
						if( mask == 0x00000002 )
						{
							party[no].id = getIntegerToken( data, S_DELIM, i );// 0x00000002
							i++;
						}
						else
						if( mask == 0x00000004 )
						{
							party[no].level = getIntegerToken( data, S_DELIM, i );// 0x00000004
							i++;
						}
						else
						if( mask == 0x00000008 )
						{
							party[no].maxHp = getIntegerToken( data, S_DELIM, i );// 0x00000008
							i++;
						}
						else
						if( mask == 0x00000010 )
						{
							party[no].hp = getIntegerToken( data, S_DELIM, i );// 0x00000010
							i++;
						}
						else
						if( mask == 0x00000020 )
						{
							party[no].mp = getIntegerToken( data, S_DELIM, i );// 0x00000020
							i++;
						}
						else
						if( mask == 0x00000040 )
						{
							getStringToken( data, S_DELIM, i, sizeof( name )-1, name );// 0x00000040
							makeRecvString( name );
							if( strlen( name ) <= sizeof( party[no].name )-1 )
							{
								strcpy( party[no].name, name );
							}
							else
							{
								strcpy( party[no].name, "???" );
							}
							i++;
						}
					}
				}
			}

			if( party[no].id != pc.id )
			{
				ptAct = getCharObjAct( party[no].id );
				if( ptAct != NULL )
				{
					party[no].ptAct = ptAct;
					setCharParty( ptAct );
					// NPCがリ`ダ`のr
					if( no == 0 )
					{
						setCharLeader( ptAct );
					}
#if 0
					else
					{
						// リ`ダ`がＮＰＣのr
						if( party[0].id != pc.id )
						{
							// リ`ダ`のいる鏊艘
							if( party[0].ptAct != NULL )
							{
								stockCharMovePoint( ptAct, party[0].ptAct->gx, party[0].ptAct->gy );
							}
						}
						// リ`ダ`がＰＣのr
						else
						{
							// リ`ダ`のいる鏊艘
							stockCharMovePoint( ptAct, nowGx, nowGy );
						}
					}
#endif
				}
				else
				{
					party[no].ptAct = NULL;
				}
			}
			else
			{
				party[no].ptAct = pc.ptAct;
				setPcParty();

				// PCがリ`ダ`のr
				if( no == 0 )
				{
					setPcLeader();
				}
#if 0
				// PCがリ`ダ`以外のr
				else
				{
					// リ`ダ`のいる鏊艘
					if( party[0].ptAct != NULL )
					{
						stockCharMovePoint( pc.ptAct, party[0].ptAct->gx, party[0].ptAct->gy );
					}
				}
#endif
			}
			}
			break;

		// アイテムの情
		case 'I':
			{
			int i;
			int no;
			char name[256];
			char name2[256];
			char memo[256];

			// カテゴリ文字をwばす
			data++;

			for( i = 0; i < MAX_ITEM; i++ )
			{
				no = i * 9;
				getStringToken( data, '|', no+1, sizeof( name ) - 1 , name );
				makeRecvString( name );

				// 名前がoいrはアイテムがoいとする
				if( strlen( name ) == 0 )
				{
					pc.item[i].useFlag = 0;
					continue;
				}

				pc.item[i].useFlag = 1;
				if( strlen( name ) <= ITEM_NAME_LEN )
				{
					strcpy( pc.item[i].name, name );
				}
				getStringToken( data, '|', no+2, sizeof( name2 ) - 1, name2 );
				makeRecvString( name2 );
				if( strlen( name2 ) <= ITEM_NAME2_LEN )
				{
					strcpy( pc.item[i].name2, name2 );
				}
				pc.item[i].color = getIntegerToken( data, '|', no+3 );
				if( pc.item[i].color < 0 )
					pc.item[i].color = 0;
				getStringToken( data, '|', no+4, sizeof( memo ) - 1, memo );
				makeRecvString( memo );
				if( strlen( memo ) <= ITEM_MEMO_LEN )
				{
					strcpy( pc.item[i].memo, memo );
				}
				pc.item[i].graNo = getIntegerToken( data, '|', no+5 );
				pc.item[i].field = getIntegerToken( data, '|', no+6 );
				pc.item[i].target = getIntegerToken( data, '|', no+7 );
				if( pc.item[i].target >= 100 )
				{
					pc.item[i].target %= 100;
					pc.item[i].deadTargetFlag = 1;
				}
				else
				{
					pc.item[i].deadTargetFlag = 0;
				}
				pc.item[i].level = getIntegerToken( data, '|', no+8 );
				pc.item[i].sendFlag = getIntegerToken( data, '|', no+9 );
			}
			}
			break;

		// ペットの技
		case 'W':
			{
			int i;
			int no, no2;
			char name[256];
			char memo[256];

			no = data[1] - '0';

			// カテゴリ文字をwばす
			data += 3;

			for( i = 0; i < MAX_SKILL; i++ )
			{
				petSkill[no][i].useFlag = 0;
			}
			for( i = 0; i < MAX_SKILL; i++ )
			{
				no2 = i * 5;
				getStringToken( data, '|', no2+4, sizeof( name ) - 1, name );
				makeRecvString( name );
				// 名前がoいrはスキルがない
				if( strlen( name ) == 0 )
				{
					continue;
				}

				petSkill[no][i].useFlag = 1;
				if( strlen( name ) <= SKILL_NAME_LEN )
				{
					strcpy( petSkill[no][i].name, name );
				}
				else
				{
					strcpy( petSkill[no][i].name, "??? name ???" );
				}
				petSkill[no][i].skillId = getIntegerToken( data, '|', no2+1 );
				petSkill[no][i].field = getIntegerToken( data, '|', no2+2 );
				petSkill[no][i].target = getIntegerToken( data, '|', no2+3 );
				getStringToken( data, '|', no2+5, sizeof( memo ) - 1, memo );
				makeRecvString( memo );
				if( strlen( name ) <= SKILL_MEMO_LEN )
				{
					strcpy( petSkill[no][i].memo, memo );
				}
				else
				{
					strcpy( petSkill[no][i].memo, "??? memo ???" );
				}
			}
			}
			break;


#if 0
    case 'S':       // スキル
        {
            int c=0;
            for(int i=0;i<MAXSKILL;i++){
                pinfo.skill_kind_table[c] = getIntegerToken( data + 1, '|' , (i*2)+1 );
                pinfo.skill_level_table[c] = getIntegerToken( data + 1, '|' , (i*2)+1+1 );
                if( pinfo.skill_kind_table[c]!=-1){
                    c++;
                }else{
					break;
				}
            }
            pinfo.skill_kind_table[c] = SKILL_NUM;
            pinfo.skill_level_table[c] = 1;
			for( i = c+1; i < MAXSKILL+1; i++ )
			{
            	pinfo.skill_kind_table[i] = -1;
            	pinfo.skill_level_table[i] = 0;
			}
        }
        break;
    case 'T':       // 称号
        {
            for(int i=0;i<MAXTITLE;i++){
                char t[TITLESIZE];
                strcpy( t , "");
                getStringToken( data + 1, '|' , i+1 , sizeof(t)-1,t );
				makeRecvString(t);
                strcpy( pinfo.title[i] , t );
            }
        }
        break;
	case 'G':
		for (i=1;i<strlen(data);i++){
			if(i>MAXGEM)break;
			m[0] = data[i];
			m[1] = '\0';
			pinfo.knowngem[i-1] = atoi(m);
		}
		break;
#endif
    }
}


// マップI理 /////////////////////////////////////////////////////////////
// マップファイルの情螭钎隶Д氓单啶蛉・辍
// マップが浃铯盲郡oいrにマップを要求する。
void lssproto_MC_recv( int fd, int fl, int x1, int y1, int x2, int y2,
	int tileSum, int partsSum, int eventSum, char *data )
{
	char showString[512];
	char floorName[32];

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf( msg , "地形のチェックサムをうけとった。FL%d %d,%d-%d,%d (%ud/%ud)",
		fl,x1,y1,x2,y2, tileSum, partsSum );
    PUSH( msg );
#endif

	getStringToken( data, '|', 1, sizeof( showString )-1, showString );

	makeRecvString( showString );
	// F在いるフロア以外の情螭I理しない
	if( nowFloor == fl )
	{
		getStringToken( showString, '|', 1, sizeof( floorName )-1, floorName );
		if( strlen( floorName ) <= FLOOR_NAME_LEN )
		{
			strcpy( nowFloorName, floorName );
		}
		else
		{
			strcpy( nowFloorName, "???" );
		}

		// パレット涓I理
		char strPal[32];

		palNo = -2;

		getStringToken( showString, '|', 2 , sizeof( strPal )-1, strPal );
		if( strlen( strPal ) == 0 )
		{
			if( TimeZonePalChangeFlag == FALSE || loginFlag )
			{
				// パレットチェンジ
				palNo = -1;
			}
		}
		else
		{
			int pal;
			pal = atoi( strPal );
			if( pal >= 0 )
			{
				if( TimeZonePalChangeFlag == TRUE || loginFlag )
				{
					// 固定パレットO定
					palNo = pal;
				}
			}
			else
			{
				if( TimeZonePalChangeFlag == FALSE || loginFlag )
				{
					// パレットチェンジ
					palNo = -1;
				}
			}
		}
	}

	if( mapCheckSum( fl, x1, y1, x2, y2, tileSum, partsSum, eventSum ) )
	{
		if( nowFloor == fl )
		{
			// チェックサム同じrのI理
			floorChangeFlag = FALSE;
			if( warpEffectStart )
			{
				warpEffectOk = TRUE;
			}
		}
		// もうこのフラグはいらないのでFALSEにする
		loginFlag = FALSE;
	}
}


// マップデ`タを受け取りファイルに
void lssproto_M_recv( int fd, int fl, int x1, int y1, int x2, int y2, char* data )
{
	char showString[512];
	char floorName[32];
	char tilestring[18192];
	char partsstring[18192];
	char eventstring[18192];
	unsigned short tile[2048] , parts[2048], event[2048];
	int i;
	char tmp[100];
	int flag;

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

#ifdef DEBUGPUSH
    char msg[800];
    sprintf( msg , "サ`バ`から地形をうけとった。FL%d %d,%d-%d,%d" , fl,x1,y1,x2,y2 );
    PUSH( msg );
#endif

	getStringToken( data, '|', 1 , sizeof( showString )-1, showString );

	makeRecvString( showString );

	if( nowFloor == fl )
	{
		getStringToken( showString, '|', 1 , sizeof( floorName )-1, floorName );
		if( strlen( floorName ) <= FLOOR_NAME_LEN )
		{
			strcpy( nowFloorName, floorName );
		}
		else
		{
			strcpy( nowFloorName, "???" );
		}

		// パレット涓I理
		char strPal[32];

		palNo = -2;

		getStringToken( showString, '|', 2 , sizeof( strPal )-1, strPal );
		if( strlen( strPal ) == 0 )
		{
			if( TimeZonePalChangeFlag == FALSE || loginFlag )
			{
				// パレットチェンジ
				palNo = -1;
			}
		}
		else
		{
			int pal;
			pal = atoi( strPal );
			if( pal >= 0 )
			{
				if( TimeZonePalChangeFlag == TRUE || loginFlag )
				{
					// 固定パレットO定
					palNo = pal;
				}
			}
			else
			{
				if( TimeZonePalChangeFlag == FALSE || loginFlag )
				{
					// パレットチェンジ
					palNo = -1;
				}
			}
		}
	}

	getStringToken( data, '|', 2 , sizeof( tilestring )-1, tilestring );	
	getStringToken( data, '|', 3 , sizeof( partsstring )-1, partsstring );
	getStringToken( data, '|', 4 , sizeof( eventstring )-1, eventstring );

	for( i = 0; ; i++ )
	{
		flag = getStringToken( tilestring, ',', i+1, sizeof( tmp )-1, tmp );
		tile[i] = a62toi( tmp );

		getStringToken( partsstring, ',', i+1, sizeof( tmp )-1, tmp );
		parts[i] = a62toi( tmp );

		getStringToken( eventstring, ',', i+1, sizeof( tmp )-1, tmp );
		event[i] = a62toi( tmp );

		if( flag == 1 )
			break;
	}

	writeMap( fl, x1, y1, x2, y2, tile, parts, event );

	// マップが空のrデ`タがきたらマップをiみzむようにする
	if( mapEmptyFlag
	 || floorChangeFlag )
	{
		if( nowFloor == fl )
		{
			redrawMap();
			floorChangeFlag = FALSE;
			if( warpEffectStart )
			{
				warpEffectOk = TRUE;
			}
		}
	}

	// もうこのフラグはいらないのでFALSEにする
	loginFlag = FALSE;
}




// ??????????? /////////////////////////////////////////////////////
void lssproto_C_recv( int fd, char *data ) 
{
	int i, j;
	char bigtoken[2048];
	char smalltoken[2048];

	int id;
	int x;
	int y;
	int dir;
	int graNo;
	int level;
	int nameColor;
	char name[2048];
	char freeName[2048];
	int walkable;
	int height;
	int classNo;
	char info[1024];
	int money;
	int charType;
	ACTION *ptAct;

	// ?????R??????c? ???p}???
	if( logOutFlag )
		return;

	// 殛L中はoする ???????????c? ???v???
	if( encountNowFlag )
	{
		return;
	}

	for( i = 0; ; i++ )
	{
		// パラメ`タoくなったらKわる
		getStringToken( data, ',', i+1, sizeof( bigtoken )-1, bigtoken );
		if( strlen( bigtoken ) == 0 )
			break;

		// 11目のト`クンがあったらそれはキャラ。
		getStringToken( bigtoken , '|' , 11 , sizeof( smalltoken )-1, smalltoken );
		if( strlen( smalltoken ) > 0 )
		{
			// NPCの情
			charType = getIntegerToken( bigtoken, '|', 1 );
			getStringToken( bigtoken, '|', 2, sizeof( smalltoken )-1, smalltoken );
			id = a62toi( smalltoken );
			getStringToken( bigtoken, '|', 3, sizeof( smalltoken )-1, smalltoken );
			x = atoi( smalltoken );
			getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
			y = atoi( smalltoken );
			getStringToken( bigtoken, '|', 5, sizeof( smalltoken )-1, smalltoken );
			dir = (atoi( smalltoken )+3)%8;
			getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
			graNo = atoi( smalltoken );
			getStringToken( bigtoken, '|', 7, sizeof( smalltoken )-1,smalltoken );
			level = atoi( smalltoken );
			nameColor = getIntegerToken( bigtoken, '|', 8 );
			getStringToken( bigtoken , '|' , 9 , sizeof( name )-1, name );
			makeRecvString( name );
			getStringToken( bigtoken , '|' , 10 , sizeof( freeName )-1, freeName );
			makeRecvString( freeName );
			getStringToken( bigtoken, '|', 11, sizeof( smalltoken )-1, smalltoken );
			walkable = atoi( smalltoken );
			getStringToken( bigtoken, '|', 12, sizeof( smalltoken )-1, smalltoken );
			height = atoi( smalltoken );

			if( pc.id == id )
			{
				// PCキャラ
				if( pc.ptAct == NULL )
				{
					// アクションoかったら作る
					createPc( graNo, x, y, dir );
					updataPcAct();
				}
				else
				{
					// アクションあったらO定だけ
#if 0
					setPcGraNo( graNo, dir );
					setPcMovePoint( x, y );
#else
					// すでにアクションあるrは、
					// X,Y,DIRはoする
					setPcGraNo( graNo, pc.dir );
#endif
				}
				setPcParam( name, freeName, level, nameColor, walkable, height );
				if( (pc.status & CHR_STATUS_LEADER) != 0
				 && party[0].useFlag != 0 )
				{
					party[0].level = pc.level;
					strcpy( party[0].name, pc.name );
				}
				// 仲g情螭盲郡楗伐绁螗荪ぅ螗郡蚋
				for( j = 0; j < MAX_PARTY; j++ )
				{
					if( party[j].useFlag != 0 && party[j].id == id )
					{
						party[j].ptAct = pc.ptAct;
						setPcParty();
						// PCがリ`ダ`のr
						if( j == 0 )
						{
							setPcLeader();
						}
						break;
					}
				}
			}
			else
			{
				// NPC
#if 0
				// ユ`ザがO定した名前があるならそれを使う
				if( strlen( freeName ) > 0 )
				{
					strcpy( name, freeName );
				}
#endif
				setNpcCharObj( id, graNo, x, y, dir, name, freeName,
					level, nameColor, walkable, height, charType );
				ptAct = getCharObjAct( id );
				if( ptAct != NULL )
				{
					// 仲g情螭盲郡楗伐绁螗荪ぅ螗郡蚋
					for( j = 0; j < MAX_PARTY; j++ )
					{
						if( party[j].useFlag != 0 && party[j].id == id )
						{
							party[j].ptAct = ptAct;
							setCharParty( ptAct );
							// NPCがリ`ダ`のr
							if( j == 0 )
							{
								setCharLeader( ptAct );
							}
							break;
						}
					}
				}
			}

#ifdef DEBUGPUSH
//			PUSHVAR( "Charindex %d　img %dのキャラをだせとサ`バ`がいった。", id, graNo );
#endif
		}
		else
		{
			getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
			if( strlen( smalltoken ) > 0 )
			{
				// アイテム情
				getStringToken( bigtoken, '|', 1, sizeof(smalltoken)-1, smalltoken );
				id = a62toi( smalltoken );
				getStringToken( bigtoken, '|', 2, sizeof(smalltoken)-1, smalltoken );
				x = atoi( smalltoken );
				getStringToken( bigtoken, '|', 3, sizeof(smalltoken)-1, smalltoken );
				y = atoi( smalltoken );
				getStringToken( bigtoken, '|', 4, sizeof(smalltoken)-1, smalltoken );
				graNo = atoi( smalltoken );
				classNo = getIntegerToken( bigtoken, '|', 5 );
				getStringToken( bigtoken, '|', 6, sizeof( info )-1, info );
				makeRecvString( info );

				setItemCharObj( id, graNo, x, y, 0, classNo, info );
			}
			else
			{
				getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
				if( strlen( smalltoken ) > 0 )
				{
					// お金情
					getStringToken( bigtoken, '|', 1, sizeof(smalltoken)-1, smalltoken );
					id = a62toi( smalltoken );
					getStringToken( bigtoken, '|', 2, sizeof(smalltoken)-1, smalltoken );
					x = atoi( smalltoken );
					getStringToken( bigtoken, '|', 3, sizeof(smalltoken)-1, smalltoken );
					y = atoi( smalltoken );
					getStringToken( bigtoken, '|', 4, sizeof(smalltoken)-1, smalltoken );
					money = atoi( smalltoken );

					sprintf( info, "%d Stone", money );

					// お金の量でグラフィックを浃à毪
					if( money > 10000 )
					{
						setMoneyCharObj( id, 24050, x, y, 0, money, info );
					}
					else
					if( money > 1000 )
					{
						setMoneyCharObj( id, 24051, x, y, 0, money, info );
					}
					else
					{
						setMoneyCharObj( id, 24052, x, y, 0, money, info );
					}
				}
			}
		}
	}
}




// キャラのアクションと抹消 ///////////////////////////////////////////////
// キャラのアクション
void lssproto_CA_recv( int fd, char *data ) 
{
	char bigtoken[2048];
	char smalltoken[2048];
	int alreadytellC[1024];
	int tellCindex = 0;
	int tellflag;
	int i, j;
	int charindex;
	int x;
	int y;
	int act;
	int dir;
	int effectno;
	int effectparam1;
	int effectparam2;
	ACTION *ptAct;


	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	// 殛L中はoする
	if( encountNowFlag )
	{
		return;
	}

	for( i = 0; ; i++ )
	{
		getStringToken( data, ',', i+1, sizeof( bigtoken )-1, bigtoken );
		if( strlen( bigtoken ) == 0 )
			break;
		getStringToken( bigtoken, '|', 1, sizeof( smalltoken )-1, smalltoken );
		charindex = a62toi( smalltoken );
		getStringToken( bigtoken, '|', 2, sizeof( smalltoken )-1, smalltoken );
		x = atoi( smalltoken );
		getStringToken( bigtoken, '|', 3, sizeof( smalltoken )-1, smalltoken );
		y = atoi( smalltoken );
		getStringToken( bigtoken, '|', 4, sizeof( smalltoken )-1, smalltoken );
		act = atoi( smalltoken );
		getStringToken( bigtoken, '|', 5, sizeof( smalltoken )-1, smalltoken );
		dir = (atoi( smalltoken )+3)%8;
		getStringToken( bigtoken, '|', 6, sizeof( smalltoken )-1, smalltoken );
		effectno = atoi( smalltoken );
		effectparam1 = getIntegerToken( bigtoken, '|', 7 );
		effectparam2 = getIntegerToken( bigtoken, '|', 8 );


		if( pc.id == charindex )
		{
			// PCキャラの幼涓
			if( pc.ptAct == NULL
			 || (pc.ptAct != NULL && pc.ptAct->anim_chr_no == 0) )
			{
				// アクション出来てないrはCを要求しアクションを作る
				lssproto_C_send( sockfd, charindex );
			}
			else
			{
				changePcAct( x, y, dir, act, effectno, effectparam1, effectparam2 );
			}
			continue;
		}

		ptAct = getCharObjAct( charindex );
		if( ptAct == NULL )
		{
			// 同じIDが}数あってもサ`バにい合わせるのは１IDで一回にする
			tellflag = 0;
			for( j = 0; j < tellCindex; j++ )
			{
				if( alreadytellC[j] == charindex )
				{
					tellflag = 1;
					break;
				}
			}
			if( tellflag == 0 && tellCindex < sizeof(alreadytellC) )
			{
				alreadytellC[tellCindex] = charindex;
				tellCindex++;

				lssproto_C_send( sockfd, charindex );
			}
		}
		else
		{
			changeCharAct( ptAct, x, y, dir, act, effectno, effectparam1, effectparam2 );
		}
	}
}


// キャラの抹消
void lssproto_CD_recv( int fd, char *data )
{
	int i, j;
	int id;

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	for( i = 1; ; i++ )
	{
//		id = getInteger62Token( data, '|', i );
		id = getInteger62Token( data, ',', i );
		if( id == -1 )
			break;

		delCharObj( id );

		for( j = 0; j < MAX_PARTY; j++ )
		{
			if( party[j].useFlag != 0 && party[j].id == id )
			{
				party[j].ptAct = NULL;
				break;
			}
		}
	}
}




// 移 ///////////////////////////////////////////////////////////////////
// サ`バに移婴工毪长趣护à
void walkSendForServer( int x, int y, char *direction )
{
    lssproto_W_send( sockfd , x, y, direction );
}


// サ`バに移婴工毪长趣护à
//  こちらはチェックサムを送ってこないので１度でもいった所でないと使えない。
void noChecksumWalkSendForServer( int x, int y, char *direction )
{
    lssproto_w_send( sockfd , x, y, direction );
}


// 移鹰偿蕙螗嗓畏荡鹗苄
void lssproto_W_recv( int fd, int id, int x, int y )
{
	// 今回はあたり判定とかクライアントでして
	// サ`バには教えるだけなので何もしない。かも？
}




// チャット文字列送信 //聊天字符串发送///////////////////////////////////////////////////
void chatStrSendForServer( char *str, int color )
{


	char dest[1024], m[1024];
	int x, y;


	x = nowGx;
	y = nowGy;

	makeSendString( str, dest, sizeof(dest) );
	sprintf( m, "P|%s", dest );
	lssproto_TK_send( sockfd, x, y, m, color, NowMaxVoice );
}




// テキスト受信 ///////////////////////////////////////////////////////////
void lssproto_TK_recv( int fd, int index, char *message, int color )
{
	char id[2];
	char msg[2024];
	ACTION *ptAct;

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	getStringToken( message, '|', 1, sizeof( id  )-1, id );

	if( id[0] == 'P' )
	{
		getStringToken( message, '|', 2, sizeof( msg )-1, msg );
		makeRecvString( msg );
		StockChatBufferLine( msg, color );

		if( index >= 0 )
		{
			if( pc.ptAct != NULL && pc.id == index )
			{
				// 1000ミリ秒表示
				setPcFukidashi( 1000 );
			}
			else
			{
				ptAct = getCharObjAct( index );
				if( ptAct != NULL )
				{
					// 1000ミリ秒表示
					setCharFukidashi( ptAct, 1000 );
				}
			}
		}
	}



#if 0




	if(index == my_id_in_server){
		if(pinfo.ptaction != NULL){
			createPlayerCommonEffect( pinfo.ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}else{
		int goind = searchGameObjectIndex( index );
//		if( goind >= 0 ||gameobj[goind].ptaction!=NULL){
		if( goind >= 0 && gameobj[goind].ptaction!=NULL ){
			createPlayerCommonEffect( gameobj[goind].ptaction, SPR_fukidasi, 0, 1, -5 );
		}
	}

#ifdef DEBUGPUSH
	if(color<249)PUSHVAR("浃噬钎幞氓哗`ジ来た（%d）'%s'",color, message); 
#endif
	char id[2];
    char msg[2024];
	getStringToken( message , '|' , 1 , sizeof( id )-1, id);
	if(id[0] == 'P'){
		getStringToken( message , '|' , 2 , sizeof( msg )-1, msg);
		makeRecvString(msg);
		char cutmes[1024];
		int i, j;
		for(i=0, j=0;;i++,j++){
			cutmes[j] = msg[i];
			if(msg[i] == '\0'){
				chatinputPushString(cutmes, color);
				break;
			}
			if( IsDBCSTrailByte( msg, &msg[i+1] ) ){
				continue;
			}
			cutmes[j+1]='\0';
			if(GetStringWidth(cutmes)>600){
				j = -1;
				chatinputPushString(cutmes, color);
			}
		}
	}else if(id[0] == 'D'){
		int i;
		selectwindowid = getIntegerToken(message, '|', 2);
		void setDengonbanMessageNumber(int i);//netaction.cppに本体
		setDengonbanMessageNumber(getIntegerToken(message, '|', 3));
		for(i=0;i<5;i++){
			getStringToken( message , '|' , i+4 , sizeof( selectstringline[i] )-1, selectstringline[i]);
			makeRecvString(selectstringline[i]);
		}
		cantclosewindowflag = 1;
		OpenNPCWindow(NPC_BOARD_WINDOW);
	}else if(id[0] == 'W'){
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode )-1, mode);
		char caption[255];
		int flag;
		flag = getStringToken( message , '|' , 4 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		strcpy(selectstringline[0],caption);
		for(i=1;;i++){
			selectstringlineindex[i] = getIntegerToken(message, '|', 3+i*2);
			flag = getStringToken( message , '|' , 4+i*2 , sizeof( caption )-1, caption);
			makeRecvString(caption);
			strcpy(selectstringline[i],caption);
			if(flag==1)break;
		}
		selectstringlinenumber=i;
		cantclosewindowflag = 1;
		if(mode[0] == 'G')OpenNPCWindow(NPC_KATARIBE_WINDOW);
		else if(mode[0] == 'N')OpenNPCWindow(NPC_WINDOW);
		else PUSHVAR("サ`バがウィンドウ_けといったが理解できなかった", msg);
	}else if(id[0] == 'M'){
		int i = 0, j = 0, k = 0;
		if(strlen(message)>1024){
#ifdef DEBUGPUSH
			PUSH("サ`バから来たメッセ`ジがLすぎるなり");
#endif
			return;
		}
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char mode[10];
		getStringToken( message , '|' , 3 , sizeof( mode )-1, mode);
		char caption[1024], onelinestring[1024];
		getStringToken( message , '|' , 4 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		for(i=0;;i++){
			if(caption[i] == '\n'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				k = 0;
			}else if (caption[i] == '\0'){
				onelinestring[k] = '\0';
				memcpy(selectstringline[j], onelinestring, k+1);
				j++;
				break;
			}else{
				onelinestring[k] = caption[i];
				k++;
			}
		}
		selectstringlinenumber=j;
		if(mode[0] == 'G')OpenNPCWindow(NPC_G_MESSAGE_WINDOW);
		else if(mode[0] == 'N')OpenNPCWindow(NPC_MESSAGE_WINDOW);
		else PUSHVAR("サ`バがウィンドウ_けといったが理解できなかった", msg);
	}else if(id[0] == 'S'){
		int i;
#if 1
		// 店のデ`タが来たのでバッファを初期化
		for( i = 0; i < MAXSHOPITEM; i++ )
		{
			shopitem[i].plice = 0;
			shopitem[i].stock = 0;
			shopitem[i].itm.name[0] = '\0';
			shopitem[i].itm.name2[0] = '\0';
			shopitem[i].itm.memo[0] = '\0';
			shopitem[i].itm.stage = 0;
			shopitem[i].itm.imgno = 0;
		}
#endif
		selectwindowid = getIntegerToken( message , '|' , 2 );	
		for(i=0;i<7;i++){
			getStringToken( message , '|' , 3+i , sizeof( shopmessage[i] )-1, shopmessage[i] );
			makeRecvString(shopmessage[i]);
		}
		shopgold = getIntegerToken( message , '|' , 10 );
		shopstock = getIntegerToken(message, '|', 11 );
		for(i=0;i<shopstock;i++){
			getStringToken( message , '|' , 12+i*5, sizeof(shopitem[i].itm.name)-1, shopitem[i].itm.name);
			makeRecvString(shopitem[i].itm.name);
			if(strlen(shopitem[i].itm.name)==0)break;
			shopitem[i].plice = getIntegerToken( message , '|' , 13+i*5);
			shopitem[i].itm.stage = getIntegerToken( message , '|' , 14+i*5);
			if(shopitem[i].itm.stage<0)shopitem[i].itm.stage=0;
			shopitem[i].itm.imgno = getIntegerToken( message , '|' , 15+i*5);
			getStringToken( message , '|' , 16+i*5, sizeof(shopitem[i].itm.memo)-1, shopitem[i].itm.memo);
			makeRecvString(shopitem[i].itm.memo);
			shopitem[i].stock = 1;
		}
		for (i=0;i<40;i++){
			charitemplice[i] = getIntegerToken(message, '|', 12+shopstock*5+i);
		}
		void setShoppage(int i);//layout.cpp
		setShoppage (0);
		OpenNPCWindow(NPC_SHOP_START_WINDOW);
		cantclosewindowflag = 1;
		PUSHVAR( "サ`バ`から、 '%s'というメッセ`ジきた" , msg );
	}else if(id[0] == 'L'){//ろすと
		getStringToken( message , '|' , 2 , sizeof( selectstringline[0] )-1, selectstringline[0] );
			makeRecvString(selectstringline[0]);
		getStringToken( message , '|' , 3 , sizeof( selectstringline[1] )-1, selectstringline[1] );
			makeRecvString(selectstringline[1]);
		selectstringlineindex[1] = SYSTEM_CHARLOST;
		selectstringlinenumber=1;
		cantclosewindowflag = 1;
		OpenSystemWindow();
	}else if(id[0] == 'K'){
		void setSystemMessage(int i, char *mes, int index);//layout.cpp
		int i;
		char token[1024];
		getStringToken( message , '|' , 2 , sizeof( token )-1, token);
		selectwindowid = atoi(token);
		char caption[255];
		int flag;
		int index;
		flag = getStringToken( message , '|' , 3 , sizeof( caption )-1, caption);
		makeRecvString(caption);
		strcpy(selectstringline[0],caption);
		setSystemMessage( 0, caption, SYSTEM_SYSTEM);
		for(i=1;;i++){
			flag = getStringToken( message , '|' , i+3 , sizeof( caption )-1, caption);
			makeRecvString(caption);
			strcpy(selectstringline[i],caption);
			if(strcmp(selectstringline[i],"ログアウト")==0){
				index = SYSTEM_LOGOUT;
			}else{
				index = SYSTEM_SERVERPUSH_1+i-1;
			}
			selectstringlineindex[i]=index;
			setSystemMessage( i, caption, index);
			if(flag==1)break;
		}
		selectstringlinenumber=i;
		deadflag = 1;
		OpenSystemWindow();
//		OpenNPCWindow(NPC_WINDOW);
		PUSH("TKで死んだと言われた");
	}else if(id[0] == 'R'){
		deadflag = 0;
	}else{
		PUSH("TK来たけど理解できんかった");
	}

#endif

}




// 新キャラ作成 ///////////////////////////////////////////////////////////
// 作成したキャラデ`タをサ`バに送る
void createNewCharStart( void )
{
	newCharStatus = 0;

	// サ`バで保存位置が固定になったらこっちにする
	lssproto_CreateNewChar_send( sockfd, selectPcNo, newCharacterName,
		newCharacterGraNo, newCharacterFaceGraNo,
		newCharacterVit, newCharacterStr, newCharacterTgh, newCharacterDex,
		newCharacterEarth, newCharacterWater, newCharacterFire, newCharacterWind,
		newCharacterHomeTown );

	SETSENDING;
}


// 新キャラ登h完了待ち
//    0 ... 受信待ち / 1 ... 受信完了 / -1 ... タイムアウト / -2 ... エラ`がってきた
int createNewCharProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( newCharStatus == 1 )
		{
			return 1;
		}
		else
		{
			return -2;
		}
    }

	SETTIMEOUT2( NET_ERRMSG_CREATECHARTIMEOUT );

	return 0;
}


// 新キャラ登h完了受信
void lssproto_CreateNewChar_recv( int fd, char *result, char *data ) 
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;

		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			newCharStatus = 1;
		}
    }
}








// キャラ削除 /////////////////////////////////////////////////////////////
// 削除キャラ番号送信
void delCharStart( void )
{
	charDelStatus = 0;

	lssproto_CharDelete_send( sockfd, gamestate_deletechar_charname );

	SETSENDING;
}


// キャラ削除待ち
//    0 ... 削除待ち / 1 ... 削除完了 / -1 ... タイムアウト / -2 ... エラ`がってきた
int delCharProc( void )
{
	if( netproc_sending == NETPROC_RECEIVED )
	{
		netproc_sending = NETPROC_NOTSEND;
		if( charDelStatus )
		{
			return 1;
		}
		else
		{
			return -2;
		}
	}

	SETTIMEOUT2( NET_ERRMSG_DELETECHARTIMEOUT );

	return 0;
}


// キャラ削除完了受信
void lssproto_CharDelete_recv( int fd, char *result, char *data )
{
	if( netproc_sending == NETPROC_SENDING )
	{
		netproc_sending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			charDelStatus = 1;
    	}
    }
}



#if 0
// パスワ`ド涓 /////////////////////////////////////////////////////////

static short netprocChagePasswdSending = NETPROC_NOTSEND;
static BOOL netprocChagePasswdResult = FALSE;
static DWORD chagePasswdStartTime = 0;	// タイムアウトチェック

// 新しいパスワ`ド送信（新清h）
//   CDKEYが必要
void changePassword( char *_cdkey, char *_newPasswd )
{
	char cdkey[64];
	char newPasswd[64];
	char oldPasswd[64];

//	SETNEEDCHOOSE( GAMESTATE_CHPASS_ERROR_START );

	getPassword( oldPasswd );
	strcpy( cdkey, _cdkey );
	strcpy( newPasswd, _newPasswd );

    // ChangePasswdを送信すると同rにClientLoginも送信する( lssproto.html 参照)
	netprocChagePasswdSending = NETPROC_SENDING;
	netprocChagePasswdResult = FALSE;
    lssproto_ClientLogin_send( sockfd, cdkey, newPasswd );
    lssproto_ChangePasswd_send( sockfd, oldPasswd, newPasswd );

	SET_SENDTIME( chagePasswdStartTime );
}


// パスワ`ド涓Y果待ち
// 0 ... 完了待ち/ 1 ... 完了(成功) / 2 ... 完了(失) / -1 ... タイムアウト
int changePasswordDoing( void )
{
	if( netprocChagePasswdSending == NETPROC_RECEIVED )
	{
		netprocChagePasswdSending = NETPROC_NOTSEND;
		if( netprocChagePasswdResult )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	CHECK_TIMEOUT( chagePasswdStartTime, TIMEOUT );
//	SETTIMEOUT( GAMESTATE_ERRMSG_CHPASSTIMEOUT , GAMESTATE_CHPASS_ERROR_START );

	return 0;
}


// パスワ`ド涓Y果受信
void lssproto_ChangePasswd_recv( int fd, char *result, char *data )
{
	if( netprocChagePasswdSending == NETPROC_SENDING )
	{
		netprocChagePasswdSending = NETPROC_RECEIVED;
		if( strcmp( result, SUCCESSFULSTR ) == 0 )
		{
			netprocChagePasswdResult = TRUE;
		}
    }
}
#endif


// 仲g要求の受信 /////////////////////////////////////////////////////////
void lssproto_PR_recv( int fd, int request, int result )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	if( request == 1 && result == 1 )
	{
		// lかのパ`ティ`に入る要求が通った
		setPcParty();
	}
	else
	if( request == 0 && result == 1 )
	{
		// パ`ティ`から除した
		delPcParty();
		delPcLeader();
		partyModeFlag = 0;
		clearPartyParam();
	}
	prSendFlag = 0;
}



// EV（イベント）送信後の受信待ち /////////////////////////////////////////
void lssproto_EV_recv( int fd, int seqno, int result )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	if( eventWarpSendId == seqno )
	{
		eventWarpSendFlag = 0;
		if( result == 0 )
		{
			// ワ`プ失・筏郡樵位妞虮硎兢丹护
			redrawMap();
			floorChangeFlag = FALSE;
			// 制的にフェ`ドインさせる
			warpEffectStart = TRUE;
			warpEffectOk = TRUE;
		}
	}
	else
	if( eventEnemySendId == seqno )
	{
		if( result == 0 )
		{
			eventEnemySendFlag = 0;
		}
		//else
		//{
			// フラグオフはprocess.cppでやる
		//}
	}
}


// 仲gOK、殛L途中参加OK、ＤＵＥＬ ＯＫの状B受信 ////////////////////////
void lssproto_FS_recv( int fd, int flg )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	pc.etcFlag = (unsigned short)flg;
	pc.etcFlag &= ~PC_ETCFLAG_JOINT_BTL;	// このビットは通常ＯＦＦ
}



// アドレスブックの内容受信 ///////////////////////////////////////////////
void lssproto_AB_recv( int fd, char *data )
{
	int i;
	int no;
	int nameLen;
	char name[256];
	int flag;
	int useFlag;

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	for( i = 0; i < MAX_ADR_BOOK; i++ )
	{
		no = i * 6;

		useFlag = getIntegerToken( data, '|', no+1 );
		if( useFlag == -1 )
		{
			useFlag = 0;
		}
		if( useFlag <= 0 )
		{
#if 0
			if( addressBook[i].useFlag == 1 )
#else
			if( MailHistory[i].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
			{
				memset( &MailHistory[i], 0, sizeof( MailHistory[0] ) ) ;
				SaveMailHistory( i );
				// 未iチェック
				mailLamp = CheckMailNoReadFlag();
				// 手アニメ消去
				DeathLetterAction();
			}
			addressBook[i].useFlag = 0;
			addressBook[i].name[0] = '\0';
			continue;
		}

		addressBook[i].useFlag = 1;

		flag = getStringToken( data, '|', no+2, sizeof( name )-1 , name );

		if( flag == 1 )
			break;

		makeRecvString( name );
		nameLen = strlen( name );
		if( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
		{
			strcpy( addressBook[i].name, name );
		}
		addressBook[i].level = getIntegerToken( data, '|', no+3 );
		addressBook[i].dp = getIntegerToken( data, '|', no+4 );
		addressBook[i].onlineFlag = (short)getIntegerToken( data, '|', no+5 );
		addressBook[i].graNo = getIntegerToken( data, '|', no+6 );
	}
}


#if 1
// アドレスブックの内容受信（g体）////////////////////////////////////////
void lssproto_ABI_recv( int fd, int num, char* data )
{
	char name[256];
	int nameLen;
	int useFlag;

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	useFlag = getIntegerToken( data, '|', 1 );
	if( useFlag == -1 )
	{
		useFlag = 0;
	}
	if( useFlag == 0 )
	{
#if 0
		if( addressBook[num].useFlag == 1 )
#else
		if( MailHistory[num].dateStr[MAIL_MAX_HISTORY-1][0] != '\0' )
#endif
		{
			memset( &MailHistory[num], 0, sizeof( MailHistory[0] ) ) ;
			SaveMailHistory( num );
			// 未iチェック
			mailLamp = CheckMailNoReadFlag();
			// 手アニメ消去
			DeathLetterAction();
		}
		addressBook[num].useFlag = useFlag;
		addressBook[num].name[0] = '\0';
		return;
	}

	addressBook[num].useFlag = useFlag;

	getStringToken( data, '|', 2, sizeof( name )-1 , name );
	makeRecvString( name );
	nameLen = strlen( name );
	if( 0 < nameLen && nameLen <= CHAR_NAME_LEN )
	{
		strcpy( addressBook[num].name, name );
	}
	addressBook[num].level = getIntegerToken( data, '|', 3 );
	addressBook[num].dp = getIntegerToken( data, '|', 4 );
	addressBook[num].onlineFlag = (short)getIntegerToken( data, '|', 5 );
	addressBook[num].graNo = getIntegerToken( data, '|', 6 );
}
#endif


// 殛LY果メッセ`ジ受信 /////////////////////////////////////////////////
void lssproto_RS_recv( int fd, char *data )
{
	int i;
	char token[2048];
	char item[2048];

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	battleResultMsg.useFlag = 1;

	for( i = 0; i < RESULT_CHR_EXP; i++ )
	{
		getStringToken( data, ',', i+1, sizeof( token )-1, token );

		battleResultMsg.resChr[i].petNo = getIntegerToken( token, '|', 1 );
		battleResultMsg.resChr[i].levelUp = getIntegerToken( token, '|', 2 );
		battleResultMsg.resChr[i].exp = getInteger62Token( token, '|', 3 );
	}

	getStringToken( data, ',', i+1, sizeof( token )-1, token );
	getStringToken( token, '|', 1, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[0], item );
	}
	getStringToken( token, '|', 2, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[1], item );
	}
	getStringToken( token, '|', 3, sizeof( item )-1, item );
	makeRecvString( item );
	if( strlen( item ) <= RESULT_ITEM_NAME_LEN )
	{
		strcpy( battleResultMsg.item[2], item );
	}
}


// デュエルY果メッセ`ジ受信 /////////////////////////////////////////////////
void lssproto_RD_recv( int fd, char *data )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	battleResultMsg.useFlag = 2;

	battleResultMsg.resChr[0].exp = getInteger62Token( data, '|', 1 );
	battleResultMsg.resChr[1].exp = getInteger62Token( data, '|', 2 );
}


// アイテム位置涓苄 ///////////////////////////////////////////////////
void lssproto_SI_recv( int fd, int from, int to )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	swapItem( from, to );
}


// アイテム情笫苄 ///////////////////////////////////////////////////////
void lssproto_I_recv( int fd, char *data )
{
	int i, j;
	int no;
	char name[256];
	char name2[256];
	char memo[256];

	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	for( j = 0; ; j++ )
	{
		no = j * 10;

		i = getIntegerToken( data, '|', no+1 );

		if( getStringToken( data, '|', no+2, sizeof( name ) - 1 , name ) == 1 )
			break;
		makeRecvString( name );

		// 名前がoいrはアイテムがoいとする
		if( strlen( name ) == 0 )
		{
			pc.item[i].useFlag = 0;
			continue;
		}

		pc.item[i].useFlag = 1;
		if( strlen( name ) <= ITEM_NAME_LEN )
		{
			strcpy( pc.item[i].name, name );
		}
		getStringToken( data, '|', no+3, sizeof( name2 ) - 1, name2 );
		makeRecvString( name2 );
		if( strlen( name2 ) <= ITEM_NAME2_LEN )
		{
			strcpy( pc.item[i].name2, name2 );
		}
		pc.item[i].color = getIntegerToken( data, '|', no+4 );
		if( pc.item[i].color < 0 )
			pc.item[i].color = 0;
		getStringToken( data, '|', no+5, sizeof( memo ) - 1, memo );
		makeRecvString( memo );
		if( strlen( memo ) <= ITEM_MEMO_LEN )
		{
			strcpy( pc.item[i].memo, memo );
		}
		pc.item[i].graNo = getIntegerToken( data, '|', no+6 );
		pc.item[i].field = getIntegerToken( data, '|', no+7 );
		pc.item[i].target = getIntegerToken( data, '|', no+8 );
		if( pc.item[i].target >= 100 )
		{
			pc.item[i].target %= 100;
			pc.item[i].deadTargetFlag = 1;
		}
		else
		{
			pc.item[i].deadTargetFlag = 0;
		}
		pc.item[i].level = getIntegerToken( data, '|', no+9 );
		pc.item[i].sendFlag = getIntegerToken( data, '|', no+10 );
	}
}


// ウィンドウI理 /////////////////////////////////////////////////////////
void lssproto_WN_recv( int fd,int windowtype,int buttontype,int seqno,int objindex,char* data )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	openServerWindow( windowtype, buttontype, seqno, objindex, data );
}


// ペットメ`ル演出 ///////////////////////////////////////////////////////
void lssproto_PME_recv( int fd, int objindex,
	int graphicsno, int x, int y, int dir, int flg, int no, char *cdata )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	// 殛L中はoする
	if( encountNowFlag )
	{
		return;
	}

	// 送信演出
	if( flg == 0 )
	{
		switch( no )
		{
			case 0:
				createPetAction( graphicsno, x, y, dir, 0, dir, -1 );
				break;

			case 1:
				createPetAction( graphicsno, x, y, dir, 2, 0, -1 );
				break;
		}
	}
	else
	// 受信演出
	{
		char smalltoken[2048];
		int id;
		int x;
		int y;
		int dir;
		int graNo;
		int level;
		int nameColor;
		char name[2048];
		char freeName[2048];
		int walkable;
		int height;
		int charType;

		charType = getIntegerToken( cdata, '|', 1 );
		getStringToken( cdata, '|', 2, sizeof( smalltoken )-1, smalltoken );
		id = a62toi( smalltoken );
		getStringToken( cdata, '|', 3, sizeof( smalltoken )-1, smalltoken );
		x = atoi( smalltoken );
		getStringToken( cdata, '|', 4, sizeof( smalltoken )-1, smalltoken );
		y = atoi( smalltoken );
		getStringToken( cdata, '|', 5, sizeof( smalltoken )-1, smalltoken );
		dir = (atoi( smalltoken )+3)%8;
		getStringToken( cdata, '|', 6, sizeof( smalltoken )-1, smalltoken );
		graNo = atoi( smalltoken );
		getStringToken( cdata, '|', 7, sizeof( smalltoken )-1,smalltoken );
		level = atoi( smalltoken );
		nameColor = getIntegerToken( cdata, '|', 8 );
		getStringToken( cdata, '|' , 9 , sizeof( name )-1, name );
		makeRecvString( name );
		getStringToken( cdata, '|' , 10 , sizeof( freeName )-1, freeName );
		makeRecvString( freeName );
		getStringToken( cdata, '|', 11, sizeof( smalltoken )-1, smalltoken );
		walkable = atoi( smalltoken );
		getStringToken( cdata, '|', 12, sizeof( smalltoken )-1, smalltoken );
		height = atoi( smalltoken );

		// すでに情螭盲郡楹韦猡筏胜
		if( setReturnPetObj( id, graNo, x, y, dir, name, freeName,
			level, nameColor, walkable, height, charType ) )
		{
			switch( no )
			{
				case 0:
					createPetAction( graphicsno, x, y, dir, 1, 0, objindex );
					break;

				case 1:
					createPetAction( graphicsno, x, y, dir, 3, 0, objindex );
					break;
			}
		}
	}
}


// i？ ///////////////////////////////////////////////////////////////////
void lssproto_IS_recv( int fd, char* cdata )
{
}




// EN（エンカウント）送信後の受信待ち /////////////////////////////////////
// result の	１：通常エンカウント
//				２：デュエル
//				４：Q
//				５：ヘルプ不可
void lssproto_EN_recv( int fd,int result,int field )
{
	// ログアウトはじめたらサ`バからの情螭oする
	if( logOutFlag )
		return;

	// エンカウンフラグＯＮ
	if( result > 0 )
	{
		EncountFlag = TRUE;
		// エンカウントが固定长钎濂ē毪r
		if( result == 6
		 || result == 2 )
		{
			eventEnemyFlag = 1;
		}
		else
		{
			eventEnemyFlag = 0;
		}
		// エラ`チェック
		if( field < 0 || BATTLE_MAP_FILES <= field ){
			BattleMapNo = 0;	// デフォルト番号
		}else{
			BattleMapNo = field;	// 殛Lマップ番号
		}
		// デュエルのr
		if( result == 2 ) DuelFlag = TRUE;
		else DuelFlag = FALSE;
		// ヘルプoしのr
		if( result == 2 || result == 5 ) NoHelpFlag = TRUE;
		else NoHelpFlag = FALSE;
		
		// フラグオフはprocess.cppでやる
		// sendEnFlag = 0;
		// duelSendFlag = 0;

		// バトルコマンド初期化
		BattleStatusReadPointer = BattleStatusWritePointer =0;
		BattleCmdReadPointer = BattleCmdWritePointer =0;
	}
	else
	{
		// Y果もらってエンカウントできないならすぐフラグオフ
		sendEnFlag = 0;
		duelSendFlag = 0;
		jbSendFlag = 0;
	}
}


// 殛L中ヘルプの受信
void lssproto_HL_recv( int fd, int flg )
{
	helpFlag = flg;
}





// バトルコマンド受信 /////////////////////////////////////////////////////
void lssproto_B_recv( int fd,char* command )
{

	// バトル状B受信
	if( *( command + 1 ) == 'C'){
//		strcpy( BattleStatusBak, command );
		strcpy( BattleStatusBak[ BattleStatusWritePointer ], command );
		BattleStatusWritePointer = ( BattleStatusWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}
	// 本人受信
	else if( *( command + 1 ) == 'P')
		sscanf( command + 3, "%X|%X|%X", &BattleMyNo, &BattleBpFlag, &BattleMyMp );
	// コマンド入力gみフラグ受信
	else if( *( command + 1 ) == 'A'){
		sscanf( command + 3, "%X|%X", &BattleAnimFlag, &BattleSvTurnNo );
		// タ`ン受信フラグＯＮのr
		if( BattleTurnReceiveFlag == TRUE ){ 
			BattleCliTurnNo = BattleSvTurnNo;	// 最初のタ`ン番号をえる。
			BattleTurnReceiveFlag = FALSE;		// フラグＯＦＦ
		}
	}
	// 殛L制K了受信
	else if( *( command + 1 ) == 'U')
		BattleEscFlag = TRUE;
	// 殛LY果受信
	else {
//		strcpy( BattleCmdBak, command );
		strcpy( BattleCmdBak[ BattleCmdWritePointer ], command );
		BattleCmdWritePointer = ( BattleCmdWritePointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
	}
	
#ifdef _DEBUG_MSG
		StockChatBufferLine( command, FONT_PAL_GRAY );
#endif
	
}

// 殛Lに出すペットをxkした
void lssproto_KS_recv( int fd,int petarray,int result )
{
	int cnt = 0; // 待Cカウント
	int i;
	
	// 受信待ちフラグ初期化
	BattlePetReceiveFlag = FALSE;
	// バトル受信中のペット番号初期化
	BattlePetReceivePetNo = -1;
	// 成功したr
	if( result == TRUE ){ 
		// ペット番号バックアップ初期化
		battlePetNoBak = -2;
		// 以前のバトルペットを待Cでなくする
		//if( pc.battlePetNo != -1 ) pc.selectPetNo[ pc.battlePetNo ] = FALSE;
		
		// 今回のバトルペットを待Cにする
		if( petarray != -1 ){ 
			// 待Cにする
			pc.selectPetNo[ petarray ] = TRUE;
			// メ`ルペットだったら、ＯＦＦにする
			if( pc.mailPetNo == petarray ) pc.mailPetNo = -1;
			// 待Cペット数チェック
			for( i = 0 ; i < 5 ; i++ ){
				if( pc.selectPetNo[ i ] == TRUE && i != petarray ) cnt++;
				// 人数オバ`のr、ＯＦＦにする
				if( cnt >= 3 ){ 
					pc.selectPetNo[ i ] = FALSE;
					cnt--;
				}
			}
		}
		// 今回のバトルペット番号
		pc.battlePetNo = petarray;
	}
}

//	スキルアップポイント
void lssproto_SKUP_recv( int fd,int point )
{
	// ステ`タスアップポイント受信
	StatusUpPoint = point;
}

// アドレスブックメッセ`ジを受け取った
void lssproto_MSG_recv( int fd,int aindex,char* text ,int color)
{
	char moji[ 256 ];
	int noReadFlag;
	
	// メ`ルランプフラグＯＮ
	mailLamp = TRUE;
	
	// 最新履s番号の更新
	MailHistory[ aindex ].newHistoryNo--;
	// リミットチェック
	if( MailHistory[ aindex ].newHistoryNo <= -1 ) MailHistory[ aindex ].newHistoryNo = MAIL_MAX_HISTORY - 1;
	
	// 日rを取り出す
	getStringToken( text, '|', 1, sizeof( MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] )-1, 
					MailHistory[ aindex ].dateStr[ MailHistory[ aindex ].newHistoryNo ] );
	// 文章を取り出す
	getStringToken( text, '|', 2, sizeof( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] )-1, 
					MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	// エスケ`プはずす
	makeRecvString( MailHistory[ aindex ].str[ MailHistory[ aindex ].newHistoryNo ] );
	
	// ペットのグラフィック番号取り出す
	noReadFlag = getIntegerToken( text, '|', 3 );
	// ペットメ`ルのr
	if( noReadFlag != -1 ){
		// ペットのグラフィック番号
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = noReadFlag;
		// ペットのレベルを取り出す
		MailHistory[ aindex ].petLevel[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 4 );
		// ペットの名前を取り出す
		getStringToken( text, '|', 5, sizeof( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] ), 
						MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// ペットの名前のエスケ`プはずす
		makeRecvString( MailHistory[ aindex ].petName[ MailHistory[ aindex ].newHistoryNo ] );
		// アイテムのグラフィック番号を取り出す
		MailHistory[ aindex ].itemGraNo[ MailHistory[ aindex ].newHistoryNo ] = getIntegerToken( text, '|', 6 );
		// メ`ルが届いたメッセ`ジ作成
		sprintf( moji,"%s からペットメ`ルが届いた！", addressBook[ aindex ].name );
	}
	// 普通メ`ルのr
	else{	
		MailHistory[ aindex ].noReadFlag[ MailHistory[ aindex ].newHistoryNo ] = TRUE;
		// メ`ルが届いたメッセ`ジ作成
		sprintf( moji,"%s からメ`ルが届いた！", addressBook[ aindex ].name );
	}
	
	// チャット文字をバッファに溜める（一行）
	StockChatBufferLine( moji, FONT_PAL_WHITE );
	
	// てるペ`ジのr
	if( mailHistoryWndSelectNo == aindex ){
		// てるペ`ジを１ペ`ジずらす
		mailHistoryWndPageNo++;
		// リミットチェック
		if( mailHistoryWndPageNo >= MAIL_MAX_HISTORY ) mailHistoryWndPageNo = 0;
		// 手アニメ`ション抹
	//	DeathLetterAction();
	}
	// 受信音
	play_se( 101, 320, 240 );
	// メ`ルデ`タの保存
	SaveMailHistory( aindex );
}


// ペットの技を使いKわったを受信
void lssproto_PS_recv( int fd,int result,int havepetindex,int havepetskill,int toindex )
{
	char moji[ 256 ];
	
	// フラグをＯＦＦ
	ItemMixRecvFlag = FALSE;
	
	// 失・筏郡
	if( result == 0 ){
		//一行インフォに表示
		sprintf( moji,"失・筏蓼筏浚");
		// チャット文字をバッファに溜める（一行）
		StockChatBufferLine( moji, FONT_PAL_WHITE );
	}
	
}



// デバッグ用出力 /////////////////////////////////////////////////////////
void lssproto_Echo_recv( int fd, char *test )
{
#if 1
#ifdef _DEBUG_MSG

	// F在の日rを取得
	time( &serverAliveLongTime );
	serverAliveTime = localtime( &serverAliveLongTime );

#endif
#endif
}







// ダミ`受信I理
void lssproto_PlayerNumGet_recv( int fd, int logincount, int player )
{
}


void lssproto_ProcGet_recv( int fd, char* data )
{
}




/*
   レ`ダ`の情螭颏Δ堡趣搿

*/
void lssproto_R_recv( int fd, char* data ) 
{
}


void lssproto_D_recv( int fd,int category,int dx,int dy,char* data ) 
{
}

/*
	える欷违博`ムオブジェクトインデックスの_列
*/
void lssproto_ACI_recv( int fd, char* data )
{
}














































#if 0

// GAMESTATE_USERINFOのためにO定しなければならない情蟆
char gamestate_userinfo_cdkey[128];
char gamestate_userinfo_passwd[128];


// GAMESTATE_CHPASSのためにO定しなければならない情蟆
char gamestate_chpass_oldpass[128];
char gamestate_chpass_newpass[128];

// GAMESTATE_CREATECHARのためにO定しなければならない情蟆
// いまはHPしかO定できないがそのうち全部O定できるようにする
int gamestate_createchar_hp;
int gamestate_createchar_mp;
int gamestate_createchar_str;
int gamestate_createchar_tough;
int gamestate_createchar_skills[128];  // 仕摔毳攻毪违畅`ドをならべていれて、最後は-1
char gamestate_createchar_charname[128];
int gamestate_createchar_imgno;


#endif

