#include "windows.h"
#include "../systeminc/process.h"
#include "../systeminc/t_music.h"
#include "../systeminc/handletime.h"
#define ACTIVE	1
//#include "system.h"
//#include "../oft/vg410.h"

extern double NowTime;	// F在の?g

/*和音ディ?イ１????*/
#define DEBUG_ON	1
#define THREAD_ON	1
#define MODE98		0
#define CLASS_UP	1
#define VOICE_KILL_ON	0

typedef struct{
	int name;
	int tone_no;
	int volume;
	int note_no;
	int release_cnt;
	int release_spd;
	int lfo_h;
	int lfo_m;
	int lfo_s;
	int lfo_s_cnt;
	int lfo_s_ang;
	int pan;
	int loop_flg;
	int delay_cnt;
	long freq;
} VOICE_EQU;

#define	MAX_ENV		20
typedef struct{
	int voice_address;	//ボイスの?（－１はボイスなし）
	int tone;
	int count;
	int distance;
	int volume;
	int volume_old;
	int side;
	int panpot;
	int panpot_old;
} ENVIRONMENT_EQU;
ENVIRONMENT_EQU env_tbl[MAX_ENV];

#define DEBUG	0
#define MY_TIMER	1
#define VOICE_MAX	64
#define TRACK_MAX	32
//#define STRICT
#include <windows.h>
#include <dsound.h>
#include <stdio.h>
#include <winuser.h>
#include <winbase.h>
#include "math2.h"
#include <math.h>
#include <main.h>

#include <tchar.h>
#include <crtdbg.h>
#include <mmsystem.h>

#define RELEASE(x) 	if(x){x->Release();x=NULL;}
#define	LOG(a,b)	(log(b)/log(a))

static int mes_flg=0;

#if THREAD_ON
int thread_flg=0;
#endif
int dsound_flg=0;

/*-------------------------------------------
	外??
--------------------------------------------*/
LPDIRECTSOUND pDSound = NULL;
LPDIRECTSOUNDBUFFER pDSPrimary = NULL;
LONG primaryVolume;
LONG primaryPan;
LONG primaryFrequency;

LPDIRECTSOUNDBUFFER pDSData_tone[TONE_MAX + 1];
LPDIRECTSOUNDBUFFER pDSData_stream;
LPDIRECTSOUNDBUFFER pDSData[VOICE_MAX];
BOOL dataPlay[VOICE_MAX];
int dataVolume[TRACK_MAX];
int dataPan[TRACK_MAX];
LONG dataFrequency[TRACK_MAX];
VOICE_EQU voice[VOICE_MAX];
TONE_EQU tone_tbl[TONE_MAX];
int tone_max_buf;

#if VOICE_KILL_ON
int voice_kill_buf[VOICE_MAX];
int voice_kill_point;
#endif

int t_music_playing_flg = 0;
int t_music_fade_flg = 0;
int t_music_fade_vol;
int draw_map_bgm_flg = 0;
static int map_bgm_vct_no = 0;
int map_bgm_no = 0;

/*-------------------------------------------
	Wave?ソ`スのiみ?み
--------------------------------------------*/
BOOL LoadWave(
			  LPCTSTR lpName,	// Waveデ`タ名
			  WAVEFORMATEX* (&pWaveHeader),	// WAVEFORMATEX造体へのポイ?タ
			  BYTE* (&pbWaveData),	// サウ?ド?デ`タへのポイ?タ
			  DWORD &cbWaveSize)	// サウ?ド?デ`タのサイズ
{
	pWaveHeader = NULL;	// WAVEFORMATEX造体へのポイ?タ
	pbWaveData = NULL;	// サウ?ド?デ`タへのポイ?タ
	cbWaveSize = 0;	// サウ?ド?デ`タのサイズ
	const char c_szWAV[] = "WAVE";	// ?ソ`ス内でのWaveファイ?の形式は，"WAVE"とする

	// ?ソ`スからファイ??デ`タをiみ?む
	HRSRC hResInfo = FindResource(NULL, lpName, c_szWAV);
	if(hResInfo == NULL)
	    return FALSE;
	HGLOBAL hResData = LoadResource(NULL, hResInfo);
	if(hResData == NULL)
	    return FALSE;
	void *pvRes = LockResource(hResData);
	if(pvRes == NULL)
	    return FALSE;

	// iみ?んだ?ソ`ス?デ`タを解析し，必要なデ`タにアクセスできるポイ?タを取得する
	DWORD *pdw = (DWORD *)pvRes;
	DWORD *pdwEnd;
	DWORD dwRiff = *pdw++;
	DWORD dwLength = *pdw++;
	DWORD dwType = *pdw++;

	if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))
	    return FALSE;      // このファイ?はRIFFファイ?ではない．
	if (dwType != mmioFOURCC('W', 'A', 'V', 'E'))
	    return FALSE;      // このファイ?はWave形式ファイ?ではない
	pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

	while (pdw < pdwEnd)
	{
	    dwType = *pdw++;
	    dwLength = *pdw++;

	    switch (dwType)
	    {
	    case mmioFOURCC('f', 'm', 't', ' '):
	        if (pWaveHeader == NULL)
	        {
	            if (dwLength < sizeof(WAVEFORMAT))
	                return FALSE;      // このファイ?は正しいWave形式ファイ?ではない
	            pWaveHeader = (WAVEFORMATEX *)pdw;
	        }
	        break;
	    case mmioFOURCC('d', 'a', 't', 'a'):
	        if ((pbWaveData == NULL) || (!cbWaveSize))
	        {
	            pbWaveData = (LPBYTE)pdw;
	            cbWaveSize = dwLength;
	        }
	        break;
	    }
	    if(pWaveHeader && (pbWaveData != NULL) && cbWaveSize)
	        break;
	    pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
	}
	if(pdwEnd <= pdw)
	    return FALSE;	// このファイ?は正しいWave形式ファイ?ではない

	// Waveデ`タ取得成功

	return TRUE;
}

/*-------------------------------------------
	?ソ`スを元にセカ?ダ??バッファを作る
--------------------------------------------*/
BOOL CreateSoundData(LPCSTR pName, LPDIRECTSOUNDBUFFER &pDSData)
{
	// ?ソ`スからWaveデ`タをiみ?む
	WAVEFORMATEX *pWaveHeader = NULL;	// WAVEFORMATEX造体へのポイ?タ
	BYTE *pbWaveData = NULL;	// サウ?ド?デ`タへのポイ?タ
	DWORD cbWaveSize = 0;	// サウ?ド?デ`タのサイズ
	if(!LoadWave(pName, pWaveHeader, pbWaveData, cbWaveSize))
	{
#ifdef _DEBUG
	    MessageBox(hWnd, "Waveデ`タのiみ?みに失", "Error", MB_OK);
#endif
		return FALSE;
	}

	// セカ?ダ??バッファを作成する
	DSBUFFERDESC dsbdesc; 
	HRESULT hr; 
	// DSBUFFERDESC造体をO定
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
	// スタティック?バッファを作る
#if ACTIVE
	dsbdesc.dwFlags = DSBCAPS_STATIC | /*DSBCAPS_CTRLDEFAULT | */DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS; 
#else
	dsbdesc.dwFlags = DSBCAPS_STATIC | /*DSBCAPS_CTRLDEFAULT | */DSBCAPS_GETCURRENTPOSITION2; 
#endif
	dsbdesc.dwBufferBytes = cbWaveSize;
	dsbdesc.lpwfxFormat = pWaveHeader;
	// バッファを作る
	hr = pDSound->CreateSoundBuffer(&dsbdesc, &pDSData, NULL); 
	if(DS_OK != hr) { 
	    // バッファの作成に失
		return FALSE;
	} 

	// セカ?ダ??バッファにWaveデ`タを?き?む
	LPVOID lpvPtr1; 	// 最?のブ?ックのポイ?タ
	DWORD dwBytes1; 	// 最?のブ?ックのサイズ
	LPVOID lpvPtr2; 	// ２番目のブ?ックのポイ?タ
	DWORD dwBytes2; 	// ２番目のブ?ックのサイズ

	hr = pDSData->Lock(0, cbWaveSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0); 

	// DSERR_BUFFERLOSTが返された?，Restore?ソッドを使ってバッファを?元する
	if(DSERR_BUFFERLOST == hr) { 
	    pDSData->Restore(); 
	    hr = pDSData->Lock(0, cbWaveSize, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0); 
	} 
	if(DS_OK == hr) { 
	    // ?ック成功

	    // ここで，バッファに?き?む
		// バッファにデ`タをコピ`する
		CopyMemory(lpvPtr1, pbWaveData, dwBytes1);
		if ( 0 != dwBytes2 )
		    CopyMemory(lpvPtr2, pbWaveData + dwBytes1, dwBytes2);

	   // ?き?みがKわったらすぐにUnlockする．
	    hr = pDSData->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2); 
	}

	return TRUE;
}

// ＷＡＶＥヘッダ情笳iみ?み
//
// 引　?：	hmmio     ... ファイ?オ`プ?ハ?ド?
//			pWfmtx    ... ヘッダ情蟾窦{I域ポイ?タ
//			pDataSize ... デ`タサイズ
// 	TRUE  ... 正常K了
//			FALSE ... 失
static BOOL WaveFormatRead( HMMIO hmmio, WAVEFORMATEX *pWfmtx, DWORD *pDataSize )
{
	MMCKINFO parent, child;

	parent.ckid = (FOURCC)0;
	parent.cksize = 0;
	parent.fccType = (FOURCC)0;
	parent.dwDataOffset = 0;
	parent.dwFlags = 0;
	child = parent;

	//WAVEチ??クを探す
	parent.fccType = mmioFOURCC('W','A','V','E');
	if ( (int)mmioDescend(hmmio,&parent,NULL,MMIO_FINDRIFF) ){
//		ERRPRINT("WAVEチ??クを探す");
		return FALSE;
	}

	//fmtチ??クに入る
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if ( mmioDescend( hmmio , &child , &parent , MMIO_FINDCHUNK ) ){
//		ERRPRINT("fmtチ??クに入る");
		return FALSE;
	}

	//フォ`マットデ`タの取得
	if (mmioRead( hmmio , (char *)pWfmtx , sizeof(WAVEFORMATEX) ) != sizeof(WAVEFORMATEX) ){
//		ERRPRINT("フォ`マットデ`タの取得");
		return FALSE;
	}

	//ＰＣＭフォ`マットかどうかのチェック
	if (pWfmtx->wFormatTag != WAVE_FORMAT_PCM ){
//		ERRPRINT("ＰＣＭフォ`マットでは?りません");
		return FALSE;
	}
	//???チ??クからiける
	if (mmioAscend( hmmio , &child , 0 )){
//		ERRPRINT("???チ??クからiける");
		return FALSE;
	}
	
	//dataチ??クに入る
	child.ckid = mmioFOURCC( 'd' , 'a' , 't' , 'a' );
	if ( mmioDescend( hmmio , &child , &parent , MMIO_FINDCHUNK ) ){
//		ERRPRINT("dataチ??クに入る");
		return FALSE;
	}

	*pDataSize = child.cksize;

	return TRUE;
}

static BYTE	*pDSbuffData;

//
// ＷＡＶＥデ`タiみ?み
//
// 引　?：	hmmio     ... ファイ?オ`プ?ハ?ド?
//			pDataSize ... iみ?むデ`タサイズ
//			pData     ... DirectSoundBuffer
// 	TRUE  ... 正常K了
//			FALSE ... 失
static BOOL WaveDataRead( HMMIO hmmio , DWORD *pDataSize, LPDIRECTSOUNDBUFFER pData )
{
//	BYTE	*pDSbuffData;

	//サ`フェ`スを?ック
	if( pData->Lock(0 , *pDataSize , (LPVOID*)&pDSbuffData , pDataSize , NULL , 0, 0 ) != DS_OK )
		return FALSE;

	//そこにデ`タを流し?む
	*pDataSize = (DWORD)mmioRead(hmmio , (char*)pDSbuffData , *pDataSize);

	//?ック解?
    if( pData->Unlock( pDSbuffData , *pDataSize , NULL , 0 ) != DS_OK )
		return FALSE;

	return TRUE;
}


//
// セカ?ダ?バッファの取得
//
// 引　?：	pWfmtx ... バッファのフォ`マット
//			DataSize ... バッファのサイズ
//			ppData ... 取得したセカ?ダ?バッファのポイ?タ
// 	TRUE  ... 正常K了
//			FALSE ... 失
static BOOL dwSoundInit( WAVEFORMATEX *pWfmtx, DWORD DataSize, LPDIRECTSOUNDBUFFER *ppData )
{
	DSBUFFERDESC  dsbufferdesc;

	//サウ?ドサ`フェ`スを起こす
	memset( &dsbufferdesc , 0  , sizeof( DSBUFFERDESC ) );
	dsbufferdesc.dwSize = sizeof( DSBUFFERDESC );
//	dsbufferdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2; 
#if ACTIVE
	dsbufferdesc.dwFlags = DSBCAPS_STATIC | /*DSBCAPS_CTRLDEFAULT |*/ DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS; 
#else
	dsbufferdesc.dwFlags = DSBCAPS_STATIC | /*DSBCAPS_CTRLDEFAULT |*/ DSBCAPS_GETCURRENTPOSITION2; 
#endif
    dsbufferdesc.dwBufferBytes = DataSize;
    dsbufferdesc.lpwfxFormat   = pWfmtx;
    if ( pDSound->CreateSoundBuffer( &dsbufferdesc , ppData , NULL ) != DS_OK )
		return FALSE;

	return TRUE;
}

int f_get(FILE *fp);
int get_num(FILE *fp);
int num_check(FILE *fp);

// ファイ?の名前、??の文??
#define mxSTR	200
/*-------------------------------------------
	DirectSound ?期化
--------------------------------------------*/
int InitDSound(void)
{
	HRESULT ret;
	//int d0;
	int d7;

	pDSound = NULL;
	pDSPrimary = NULL;
	for(d7=0;d7<VOICE_MAX;d7++)
		pDSData[d7]=NULL;
	for(d7=0;d7<VOICE_MAX;d7++)
		dataPlay[d7]=FALSE;

	ret = DirectSoundCreate(NULL, &pDSound, NULL);
	if(ret != DS_OK)
	{
#ifdef _DEBUG
	    MessageBox(hWnd, "DirectSoundオブジェクトの作成に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	//ここでf{?ベ?をO定
//	ret = pDSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
	ret = pDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if(ret != DS_OK)
	{
	    // f{?ベ?のO定に失
		RELEASE(pDSound);
#ifdef _DEBUG
	    MessageBox(hWnd, "f{?ベ?のO定に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	// スピ`カのO定
	pDSound ->SetSpeakerConfig(DSSPEAKER_COMBINED(DSSPEAKER_STEREO, DSSPEAKER_GEOMETRY_WIDE));

	// プ?イマ??バッファの作成
	// DSBUFFERDESC造体をO定
	DSBUFFERDESC dsbdesc; 
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
	// プ?イマ??バッファを指定
//	dsbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_PRIMARYBUFFER; 
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER; 
	dsbdesc.dwBufferBytes = 0;
	dsbdesc.lpwfxFormat = NULL;

	// バッファを作る
	ret = pDSound->CreateSoundBuffer(&dsbdesc, &pDSPrimary, NULL); 
	if(ret != DS_OK) { 
	    // バッファの作成に失
#ifdef _DEBUG
	    MessageBox(hWnd, "プ?イマ??バッファの音效作成に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	// プ?イマ??バッファのWaveフォ`マットをO定
	// 　　　先f{?ベ?以上のf{?ベ?がO定されている必要が?ります．
	WAVEFORMATEX pcmwf; 
	ZeroMemory(&pcmwf, sizeof(WAVEFORMATEX)); 
	pcmwf.wFormatTag = WAVE_FORMAT_PCM; 
	pcmwf.nChannels = 2;		// ２チ??ネ?（ステ?オ）
//	pcmwf.nSamplesPerSec = 44100;	// サ?プ??グ??`ト　44.1kHz
	pcmwf.nSamplesPerSec = 22050;	// サ?プ??グ??`ト　22.05kHz
	pcmwf.nBlockAlign = 4;
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign; 
	pcmwf.wBitsPerSample = 16;		// 16ビット
	ret = pDSPrimary->SetFormat(&pcmwf);
	if(ret != DS_OK){
#ifdef _DEBUG
	    MessageBox(hWnd, "プ?イマ??バッファのフォ`マットO定に失", "Error", MB_OK);
#endif
		return FALSE;
	}

#if 0
	// ?ソ`スを元にセカ?ダ??バッファを作成する
	for(d7=0;d7<TONE_MAX;d7++){
		BOOL f;
		f = CreateSoundData(MAKEINTRESOURCE(101+d7), pDSData_tone[d7]);
		if(!f){
			for(d7=0;d7<TONE_MAX;d7++)
				RELEASE(pDSData_tone[d7]);
			RELEASE(pDSPrimary);
			RELEASE(pDSound);
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファ音效の作成に失", "Error", MB_OK);
#endif
			return FALSE;
		}
	}
#else
	HMMIO hmmio;			//ファイ?ハ?ド?
    WAVEFORMATEX	Wfmtx;			//WAVEファイ?のフォ`マットデ`タ
	DWORD			DataSize;		//バッファ`のサイズ
	DWORD			WaveSize;		//Waveファイ?のサイズ
	//FILE			*fp;
	int program_no = 0;
//	int program_set_flg = 0;
	char fname[mxSTR];

	hmmio = (HMMIO)-1;		//????ク?`ズ?
	tone_max_buf = 0;

#if 0		//テキストファイ?iみ?みバ`ジ??///////////////////////////////
	fp = fopen( "data\\sndcnf.h", "r" );	//ＰＣＭ定xファイ?オ`プ?
	if( fp == NULL ){
		RELEASE(pDSPrimary);
		RELEASE(pDSound);
#ifdef _DEBUG
		MessageBox(hWnd, "音色O定ファイ?のオ`プ?に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	for(d7=0;d7<TONE_MAX;d7++){		//音色テ`ブ??期化
		tone_tbl[d7].voice_place = -1;
		tone_tbl[ tone ].play_time = 0;
		tone_tbl[ tone ].play_loop = 0;
	}

	while(1){
		d0 = f_get( fp );
		if(d0 >= 0x80 && d0 <=0x9f){
			fseek(fp,1,SEEK_CUR);		//h?スキップ
			continue;
		}
		if(d0 >= 0xe0){
			fseek(fp,1,SEEK_CUR);		//h?スキップ
			continue;
		}
		if(d0 == '{'){		//先^までスキップ
			break;
		}
	}
	//??????????　音色定xファイ?解析　??????????
	while(1){
		d0 = f_get( fp );		//１バイト取り出し
		if(d0 == EOF || tone_max_buf == TONE_MAX){		//K了なら
			fclose( fp );
			goto InitDSound_ok;
		}
		if( d0 == '\n' ){		//改行なら
			continue;
		}

		if( d0 == '/' ){		//コ??トなら
			while(1){			//そうならスキップ
				d0 = f_get( fp );
				if(d0 == '\n' || d0 == EOF )
					break;
			}
			continue;			//そうならやり直し
		}

		fseek(fp,-1,SEEK_CUR);		//１バイト
		while(1){
			d0 = f_get( fp );
			if(d0 >= 0x80 && d0 <=0x9f){
				fseek(fp,1,SEEK_CUR);		//h?スキップ
				continue;
			}
			if(d0 >= 0xe0){
				fseek(fp,1,SEEK_CUR);		//h?スキップ
				continue;
			}
			if(d0 == '{'){		//先^までスキップ
				break;
			}
		}
//		if(d0 == '{'){		//音色番?なら
			if(f_get( fp ) == '-'){		//K了なら
				fclose( fp );
				goto InitDSound_ok;
			}
			fseek(fp,-1,SEEK_CUR);		//１バイト
			if(!num_check( fp )){		//??でなければ
				goto InitDSound_err2;
			}
			program_no = get_num( fp );		//?iみ?み
			if(program_no > TONE_MAX){		//マックス以上なら
				goto InitDSound_err2;	//エ?`
			}
//			program_set_flg = 1;
//		} else {
//			fseek(fp,-1,SEEK_CUR);		//１バイト
//		}

//		if(program_set_flg){		//セットするなら
			tone_tbl[ program_no ].voice_place = tone_max_buf;		//音色セット先保存
//			program_set_flg = 0;
//		}
//		d0 = f_get( fp );		//ファイ?名の先^へ
//		fseek(fp,-1,SEEK_CUR);		//１バイト
		while(f_get( fp ) != '"');		//ファイ?名の先^へ
		d7=0;
		while(1){		//ファイ?名コピ`
			d0 = fgetc( fp );		//１バイト取り出し
			if( d0 == EOF )			//ＥＯＦか
				break;
			if( d0 == '\t' )		//タブか
				break;
			if( d0 == 0x20 )		//スペ`スか
				break;
			if( d0 == '\n' )		//改行か
				break;
			if( d0 == ',' )		//セパ?`タか
				break;
			if( d0 == '"' )		//セパ?`タか
				break;
			fname[d7++] = d0;
		}
		fname[d7++] = 0;		//K了コ`ド

		while(f_get( fp ) != ',');		//ボ??`?の先^へ
		if(!num_check( fp )){		//??でなければ
			goto InitDSound_err2;
		}

		d0 = get_num( fp );		//ボ??`?iみ?み
		if(d0 > 10){		//１１以上なら
			goto InitDSound_err2;
		}
		tone_tbl[ program_no ].voice_volume = d0;		//音色ボ??`?保存

		//ＷＡＶファイ?のオ`プ?
		if( (hmmio = mmioOpen(fname, NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?のオ`プ?に失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		//アト?ビ?`トの?`ド
		if( WaveFormatRead( hmmio , &Wfmtx , &WaveSize ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "アト?ビ?`トの?`ドに失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		DataSize = WaveSize;
		//サウ?ドサ`フェ`スを起こす
		if( dwSoundInit( &Wfmtx, DataSize, &pDSData_tone[tone_max_buf] ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファ初始化の作成に失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		//デ`タの?`ド
		if( WaveDataRead( hmmio, &DataSize, pDSData_tone[tone_max_buf] ) == FALSE  ){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?の?`ドに失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		mmioClose( hmmio , 0 );
		hmmio = (HMMIO)-1;		//????ク?`ズ?
		tone_max_buf++;		//音色セット先更新
	}

InitDSound_err:		//セカ?ダ??バッファエ?`
	fclose( fp );
	mmioClose( hmmio , 0 );
	for(d7=0;d7<tone_max_buf;d7++)
		RELEASE(pDSData_tone[d7]);
	RELEASE(pDSPrimary);
	RELEASE(pDSound);
	return FALSE;

InitDSound_err2:	//定xファイ?エ?`
	fclose( fp );
	if(hmmio != (HMMIO)-1){		//ＨＭＭＩＯオ`プ??なら
		mmioClose( hmmio , 0 );
	}
#ifdef _DEBUG
	MessageBox(hWnd, "音色O定ファイ?エ?`", "Error", MB_OK);
#endif
	for(d7=0;d7<tone_max_buf;d7++)
		RELEASE(pDSData_tone[d7]);
	RELEASE(pDSPrimary);
	RELEASE(pDSound);
	return FALSE;

InitDSound_ok:		//正常K了
#else		//イ?ク?`ドバ`ジ??///////////////////////////////////////////
	typedef struct
	{
		short no;
		char name[mxSTR];
		char vol;
		char note;
		char loop_flg;
	} SNDCONFIG;
	#include "sndcnf.h"

	for(d7=0;d7<TONE_MAX;d7++){		//音色テ`ブ??期化
		tone_tbl[d7].voice_place = -1;
	}

	//??????????　音色定xヘッダ`解析　??????????
	int d1 = 0;		//SNDCONFIGカウ?タ`?期化
	while(1){
		if(sndconfig[ d1 ].no == -1){		//K了なら
			goto InitDSound_ok;
		}
		program_no = sndconfig[ d1 ].no;		//音色番?セット
		if(program_no > TONE_MAX){		//マックス以上なら
			goto InitDSound_err2;	//エ?`
		}
		tone_tbl[ program_no ].voice_place = tone_max_buf;		//音色セット先保存
		//名前の取得
		strcpy( fname, sndconfig[ d1 ].name );

		if(sndconfig[ d1 ].vol < 0){		//ボ??`?がマイナスなら
			goto InitDSound_err2;
		}
		tone_tbl[ program_no ].voice_volume = sndconfig[ d1 ].vol;		//音色ボ??`?保存

		if(sndconfig[ d1 ].note > 60){		//ノ`ト番?エ?`なら
			goto InitDSound_err2;
		}
		if(sndconfig[ d1 ].note){		//ノ`ト番?保存
			tone_tbl[ program_no ].voice_note = sndconfig[ d1 ].note;
		} else {
			tone_tbl[ program_no ].voice_note = 0;
		}
		//?`プフ?グセット
		tone_tbl[ program_no ].voice_loop = sndconfig[ d1 ].loop_flg;

		//サ?プ??グ?`ト取り出し
		int d2;
		FILE *fp;
		fp = fopen( fname, "rb" );
		if( fp == NULL ){
#ifdef _DEBUG
//			MessageBox(hWnd, "ＷＡＶファイ?のオ`プ?に失", "Error", MB_OK);
#endif
//			return FALSE;
			d1++;		//SNDCONFIGカウ?タ`更新
			continue;
		}
		d2 = 8;						//’ＲＩＦＦ’８バイトスキップ
		d2 += 4;					//’ＷＡＶＥ’４バイトスキップ
		d2 += 4;					//’???　’４バイトスキップ
		d2 += 8;					//８バイトスキップ
		fseek(fp,d2,SEEK_CUR);		//ファイ?ポイ?タ`移
		d2 = fgetc(fp);				//サ?プ??グ?`ト取り出し
		d2 |= fgetc(fp) << 8;		//サ?プ??グ?`ト取り出し
		d2 |= fgetc(fp) << 16;		//サ?プ??グ?`ト取り出し
		d2 |= fgetc(fp) << 24;		//サ?プ??グ?`ト取り出し
		switch(d2){
			case 11025:
				tone_tbl[ program_no ].voice_rate = 25;
				break;
			case 22050:
				tone_tbl[ program_no ].voice_rate = 37;
				break;
			case 44100:
				tone_tbl[ program_no ].voice_rate = 49;
				break;
		}
		fclose(fp);

		//ＷＡＶファイ?のオ`プ?
		if( (hmmio = mmioOpen(fname, NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?のオ`プ?に失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		//アト?ビ?`トの?`ド
		if( WaveFormatRead( hmmio , &Wfmtx , &WaveSize ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "アト?ビ?`トの?`ドに失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		DataSize = WaveSize;
		//サウ?ドサ`フェ`スを起こす
		if( dwSoundInit( &Wfmtx, DataSize, &pDSData_tone[tone_max_buf] ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファ初始化の作成に失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		//デ`タの?`ド
		if( WaveDataRead( hmmio, &DataSize, pDSData_tone[tone_max_buf] ) == FALSE  ){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?の?`ドに失", "Error", MB_OK);
#endif
			goto InitDSound_err;
		}
		mmioClose( hmmio , 0 );
		hmmio = (HMMIO)-1;		//????ク?`ズ?
		d1++;		//SNDCONFIGカウ?タ`更新
		tone_max_buf++;		//音色セット先更新
	}

InitDSound_err:		//セカ?ダ??バッファエ?`
	mmioClose( hmmio , 0 );
	for(d7=0;d7<tone_max_buf;d7++)
		RELEASE(pDSData_tone[d7]);
	RELEASE(pDSPrimary);
	RELEASE(pDSound);
	return FALSE;

InitDSound_err2:	//定xファイ?エ?`
	if(hmmio != (HMMIO)-1){		//ＨＭＭＩＯオ`プ??なら
		mmioClose( hmmio , 0 );
	}
#ifdef _DEBUG
	MessageBox(hWnd, "音色O定ファイ?エ?`", "Error", MB_OK);
#endif
	for(d7=0;d7<tone_max_buf;d7++)
		RELEASE(pDSData_tone[d7]);
	RELEASE(pDSPrimary);
	RELEASE(pDSound);
	return FALSE;

InitDSound_ok:		//正常K了
#endif
#endif



	// ボイス?分音色番?０を}uする
	for(d7=0;d7<VOICE_MAX;d7++){
		if( pDSound->DuplicateSoundBuffer(pDSData_tone[0],&pDSData[d7]) != DS_OK ){
			for(d7=0;d7<tone_max_buf;d7++)
				RELEASE(pDSData_tone[d7]);
			for(d7=0;d7<VOICE_MAX;d7++)
				RELEASE(pDSData[d7]);
			RELEASE(pDSPrimary);
			RELEASE(pDSound);
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファの}uに失", "Error", MB_OK);
#endif
		return FALSE;
		}
	}

	// ボ??`?などの蛉〉盲工
	pDSPrimary->GetVolume(&primaryVolume);
//	for(d7=0;d7<TONE_MAX;d7++)
//		pDSData[d7]->GetVolume(&dataVolume[d7]);

	// パ?ポット
	pDSPrimary->GetPan(&primaryPan);
//	for(d7=0;d7<TONE_MAX;d7++)
//		pDSData[d7]->GetPan(&dataPan[d7]);

	// 周波?
	pDSPrimary->GetFrequency((DWORD*)&primaryFrequency);
//	for(d7=0;d7<TONE_MAX;d7++)
//		pDSData[d7]->GetFrequency((DWORD*)&dataFrequency[d7]);

	return TRUE;
}

volatile static char exit_thread_flg = 0;
static int thread_loop_start;
//static int thread_loop_end;
static int thread_stop_flg = 0;
static LPDIRECTSOUNDNOTIFY pDSNotify = NULL;
static char stream_flg = 0;
/*-------------------------------------------
	ス?ッド停止??
--------------------------------------------*/
void stop_thread(void)
{
	//ス?ッド停止なら
	if(thread_stop_flg){
		//ＢＧＭ停止
		pDSData_tone[ TONE_MAX ]->Stop();
//		pDSData_stream->Stop();
		while(1){
			if(exit_thread_flg == 1)
				break;
		}
		thread_stop_flg = 0;
		exit_thread_flg = 0;
		RELEASE(pDSData_stream);
	}
	//セ?カダ?バッファ_放
	RELEASE(pDSData_tone[TONE_MAX]);
}

/*-------------------------------------------
	ＢＧＭ停止??
--------------------------------------------*/
void stop_bgm(void)
{
	//ス?ッド停止なら
	if(thread_stop_flg){
		//ス?ッド停止へ
		stop_thread();
	} else {
		//セ?カダ?バッファ_放
		RELEASE(pDSData_tone[TONE_MAX]);
	}
	//フェ`ドフ?グク?ア
	t_music_fade_flg = 0;
	//再生?フ?グク?ア
	t_music_playing_flg = 0;
	//ＢＧＭ涓隶Д氓?ク?ア
	map_bgm_vct_no = 0;
	 map_bgm_no = t_music_bgm_no;
}

/*-------------------------------------------
	ＢＧＭフェ`ド??
--------------------------------------------*/
void fade_out_bgm(void)
{
	//フェ`ドアウト_始
	t_music_fade_flg = 1;
	//F在の音量保存
	t_music_fade_vol = bgm_tbl[t_music_bgm_no].volume * t_music_bgm_volume / 15;
}

/*-------------------------------------------
	K了の??
--------------------------------------------*/
int EndApp(void)
{
	int d7;
	//ス?ッド停止へ
	stop_thread();
	RELEASE(pDSData_tone[TONE_MAX]);
	//_放するなら
	if(stream_flg == 1){
		RELEASE(pDSNotify);
	}
	for(d7=0;d7<VOICE_MAX;d7++){
		pDSData[d7]->Stop();
		RELEASE( pDSData[d7] );
	}
	for(d7=0;d7<tone_max_buf;d7++){
		pDSData_tone[d7]->Stop();
		RELEASE( pDSData_tone[d7] );
	}
	RELEASE(pDSPrimary);
	RELEASE(pDSound);

	return TRUE;
}

#define MUSIC_END	-1
#define MUSIC_R		-2
#define MUSIC_LOOP	-3
#define MUSIC_FREQ	-4
#define MUSIC_VOL	-5
#define MUSIC_PAN	-6
#define MUSIC_TEMPO	-7
#define MUSIC_PEDAL	-8
#define MUSIC_NEIRO_NO	-9
#define MUSIC_KURI_RET	-10
#define MUSIC_KURI_SKIP	-11
#define MUSIC_KURI_START	-12
#define MUSIC_WAVE	-13
#define MUSIC_WAON	-14
#define MUSIC_DETUNE	-15
#define MUSIC_LFO_H		-16
#define MUSIC_LFO_M		-17
#define MUSIC_LFO_S		-18
static int cnt=0,cnt2=0;
static LARGE_INTEGER d_cnt;
static int ch;
static int step_time[TRACK_MAX];
static int gate_time[TRACK_MAX];
static int d_pedal[TRACK_MAX];
static int wave[TRACK_MAX];
static int step_point[TRACK_MAX];
static int step_def[TRACK_MAX];
static int play_flg[TRACK_MAX];
static int play_track_flg[TRACK_MAX];
static int play_kuri_num[8][TRACK_MAX];
static int play_kuri_lv[TRACK_MAX];
static int play_use_task[TRACK_MAX][8];
static int dataToneNo[TRACK_MAX];
static int tie_flg[TRACK_MAX];
static int note_old[TRACK_MAX][8];
static int play_use_task_cnt[TRACK_MAX];
static int play_detune[TRACK_MAX];
static int play_lfo_h[TRACK_MAX];
static int play_lfo_m[TRACK_MAX];
static int play_lfo_s[TRACK_MAX];
static int pause_flg;
static LARGE_INTEGER tempo;
static int step[TRACK_MAX][16384];
typedef enum {
	M_OPEN_ERR = 0,
	M_STEP_ERR,
	M_TRACK_ERR,
	M_LCOMMAND_ERR,
	M_TCOMMAND_ERR,
	M_VCOMMAND_ERR,
	M_V2COMMAND_ERR,
	M_P2COMMAND_ERR,
	M_F2COMMAND_ERR,
	M_2COMMAND_ERR,
	M_KAKKO_ERR,
	M_MML_ERR,
	M_D2COMMAND_ERR,
	M_NOTE_ERR,
	M_PLAY_ERR,
	M_KURI_ERR,
	M_KURISU_ERR,
	M_MUTE_ERR,
	M_W2COMMAND_ERR,
	M_TOCOMMAND_ERR,
	M_QCOMMAND_ERR,
	M_K2COMMAND_ERR,
	M_WAON_ERR,
	M_OCOMMAND_ERR,
	M_H2COMMAND_ERR,
	M_M2COMMAND_ERR,
	M_S2COMMAND_ERR,
} err_number;

static char *err_msg[]={
	"ファイ?のオ`プ?に失",
	"ステップタイ?エ?`",
	"ト?ック指定エ?`",
	"Ｌコマ?ドエ?`",
	"Ｔコマ?ドエ?`",
	"Ｖコマ?ドエ?`",
	"?Ｖコマ?ドエ?`",
	"?Ｐコマ?ドエ?`",
	"?Ｆコマ?ドエ?`",
	"?コマ?ドエ?`",
	"カッココマ?ドエ?`",
	"ＭＭＬエ?`",
	"?Ｄコマ?ドエ?`",
	"周波?オ`バ`エ?`",
	"?奏ト?ック指定エ?`",
	"Rり返しコマ?ドエ?`",
	"Rり返しの?が?いません",
	"ミ?`トト?ック指定エ?`",
	"?Ｗコマ?ドエ?`",
	"テ?ポコマ?ドエ?`",
	"Ｑコマ?ドエ?`",
	"?Ｋコマ?ドエ?`",
	"和音エ?`",
	"Ｏコマ?ドエ?`",
	"?Ｈコマ?ドエ?`",
	"?Ｍコマ?ドエ?`",
	"?Ｓコマ?ドエ?`",
};
//c c+ d d+ e f f+ g g+ a a+ b
//0 1  2 3  4 5 6  7 8  9 10 11
static int note_change_tbl[]={9,11,0,2,4,5,7};
static int freq_tbl[]={
	(int)(44100.0 / 261.63 * 277.18 / 32.0),	//b (o-1) 0	（未使用）
	2756,										//c (o0) 1
	(int)(44100.0 / 261.63 * 277.18 / 16.0),	//c+
	(int)(44100.0 / 261.63 * 293.67 / 16.0),	//d	
	(int)(44100.0 / 261.63 * 311.13 / 16.0),	//d+
	(int)(44100.0 / 261.63 * 329.63 / 16.0),	//e
	(int)(44100.0 / 261.63 * 349.23 / 16.0),	//f
	(int)(44100.0 / 261.63 * 366.99 / 16.0),	//f+
	(int)(44100.0 / 261.63 * 392.00 / 16.0),	//g
	(int)(44100.0 / 261.63 * 415.31 / 16.0),	//g+
	(int)(44100.0 / 261.63 * 440.00 / 16.0),	//a
	(int)(44100.0 / 261.63 * 466.16 / 16.0),	//a+
	(int)(44100.0 / 261.63 * 493.88 / 16.0),	//b
	5512,									//c (o1) 13
	(int)(44100.0 / 261.63 * 277.18 / 8.0),	//c+
	(int)(44100.0 / 261.63 * 293.67 / 8.0),	//d
	(int)(44100.0 / 261.63 * 311.13 / 8.0),	//d+
	(int)(44100.0 / 261.63 * 329.63 / 8.0),	//e
	(int)(44100.0 / 261.63 * 349.23 / 8.0),	//f
	(int)(44100.0 / 261.63 * 366.99 / 8.0),	//f+
	(int)(44100.0 / 261.63 * 392.00 / 8.0),	//g
	(int)(44100.0 / 261.63 * 415.31 / 8.0),	//g+
	(int)(44100.0 / 261.63 * 440.00 / 8.0),	//a
	(int)(44100.0 / 261.63 * 466.16 / 8.0),	//a+
	(int)(44100.0 / 261.63 * 493.88 / 8.0),	//b
	11025,									//c (o2) 25
	(int)(44100.0 / 261.63 * 277.18 / 4.0),	//c+
	(int)(44100.0 / 261.63 * 293.67 / 4.0),	//d
	(int)(44100.0 / 261.63 * 311.13 / 4.0),	//d+
	(int)(44100.0 / 261.63 * 329.63 / 4.0),	//e
	(int)(44100.0 / 261.63 * 349.23 / 4.0),	//f
	(int)(44100.0 / 261.63 * 366.99 / 4.0),	//f+
	(int)(44100.0 / 261.63 * 392.00 / 4.0),	//g
	(int)(44100.0 / 261.63 * 415.31 / 4.0),	//g+
	(int)(44100.0 / 261.63 * 440.00 / 4.0),	//a
	(int)(44100.0 / 261.63 * 466.16 / 4.0),	//a+
	(int)(44100.0 / 261.63 * 493.88 / 4.0),	//b
	22050,									//c (o3) 37
	(int)(44100.0 / 261.63 * 277.18 / 2.0),	//c+
	(int)(44100.0 / 261.63 * 293.67 / 2.0),	//d
	(int)(44100.0 / 261.63 * 311.13 / 2.0),	//d+
	(int)(44100.0 / 261.63 * 329.63 / 2.0),	//e
	(int)(44100.0 / 261.63 * 349.23 / 2.0),	//f
	(int)(44100.0 / 261.63 * 366.99 / 2.0),	//f+
	(int)(44100.0 / 261.63 * 392.00 / 2.0),	//g
	(int)(44100.0 / 261.63 * 415.31 / 2.0),	//g+
	(int)(44100.0 / 261.63 * 440.00 / 2.0),	//a
	(int)(44100.0 / 261.63 * 466.16 / 2.0),	//a+
	(int)(44100.0 / 261.63 * 493.88 / 2.0),	//b
	44100,									//c (o4) 49
	(int)(44100.0 / 261.63 * 277.18),		//c+
	(int)(44100.0 / 261.63 * 293.67),		//d
	(int)(44100.0 / 261.63 * 311.13),		//d+
	(int)(44100.0 / 261.63 * 329.63),		//e
	(int)(44100.0 / 261.63 * 349.23),		//f
	(int)(44100.0 / 261.63 * 366.99),		//f+
	(int)(44100.0 / 261.63 * 392.00),		//g
	(int)(44100.0 / 261.63 * 415.31),		//g+
	(int)(44100.0 / 261.63 * 440.00),		//a
	(int)(44100.0 / 261.63 * 466.16),		//a+
	(int)(44100.0 / 261.63 * 493.88),		//b
	88200,									//b+ (o5) 61
	(int)(44100.0 / 261.63 * 277.18 * 2.0),	//c+
	(int)(44100.0 / 261.63 * 293.67 * 2.0),	//d		（以下未使用）63
	(int)(44100.0 / 261.63 * 311.13 * 2.0),	//d+
	(int)(44100.0 / 261.63 * 329.63 * 2.0),	//e
	(int)(44100.0 / 261.63 * 349.23 * 2.0),	//f
	(int)(44100.0 / 261.63 * 366.99 * 2.0),	//f+
	(int)(44100.0 / 261.63 * 392.00 * 2.0),	//g
	(int)(44100.0 / 261.63 * 415.31 * 2.0),	//g+
	(int)(44100.0 / 261.63 * 440.00 * 2.0),	//a
	(int)(44100.0 / 261.63 * 466.16 * 2.0),	//a+
	(int)(44100.0 / 261.63 * 493.88 * 2.0),	//b
};
static long volume_tbl[128];
static long panpot_tbl[128];
static int detune_tbl[12*5+5];

int get_num(FILE *fp);
/*-------------------------------------------
	１バイトiみ?み??
--------------------------------------------*/
int f_get(FILE *fp)
{
	int d0;
	while(1){
		d0 = fgetc( fp );		//ト?ック番?取り出し
		if( d0 == EOF )
			return d0;
		if( d0 == '\t' )		//タブか
			continue;
		if( d0 != 0x20 )		//スペ`スか
			break;
	}
	if(d0 >= 'A' && d0 <= 'Z')		//大文?→小文?Q
		d0+=0x20;					//
	return d0;
}

/*-------------------------------------------
	?iみ?み??
--------------------------------------------*/
int get_num(FILE *fp)
{
	int d0,d1;
	while(1){
		d0 = f_get( fp );		//ト?ック番?取り出し
		if( d0 == EOF )
			return d0;
		if( d0 >= '0' && d0<= '9' )		//??か
			break;						//そうなら?へ
	}
	d1=d0-'0';		//１バイト目

	while(1){
		d0 = fgetc( fp );		//ト?ック番?取り出し
		if( d0 == EOF )
			break;
		if( d0 < '0' || d0> '9' )		//??か
			break;						//そうでなければ?へ
		d1*=10;		//２バイト目以降
		d1+=d0-'0';	//
	}
	fseek(fp,-1,SEEK_CUR);		//１バイト
	return d1;
}
/*-------------------------------------------
	?iみ?み??２（マイナス含む）
--------------------------------------------*/
int get_num2(FILE *fp)
{
	int d0,d1,d2=1;
	d0 = f_get( fp );		//１バイト取り出し
	if(d0=='-')		//マイナスなら
		d2=-1;
	else
		fseek(fp,-1,SEEK_CUR);		//１バイト

	while(1){
		d0 = fgetc( fp );		//ト?ック番?取り出し
		if( d0 == EOF )
			return d0;
		if( d0 >= '0' && d0<= '9' )		//??か
			break;						//そうなら?へ
	}
	d1=d0-'0';		//１バイト目

	while(1){
		d0 = fgetc( fp );		//ト?ック番?取り出し
		if( d0 == EOF )
			break;
		if( d0 < '0' || d0> '9' )		//??か
			break;						//そうでなければ?へ
		d1*=10;		//２バイト目以降
		d1+=d0-'0';	//
	}
	fseek(fp,-1,SEEK_CUR);		//１バイト
	return d1*d2;
}
/*-------------------------------------------
	??チェック??２（マイナス含む）
--------------------------------------------*/
int num_check2(FILE *fp)
{
	int d0;
	d0 = f_get( fp );		//１バイト取り出し
	if(d0=='-'){			//マイナスなら
		d0 = fgetc( fp );		//さらに１バイト取り出し
		fseek(fp,-2,SEEK_CUR);		//２バイト
		if( d0 >= '0' && d0<= '9' )		//??か
			return 1;		//はい
		else
			return 0;		//いいえ
	} else {
		fseek(fp,-1,SEEK_CUR);		//１バイト
		if( d0 >= '0' && d0<= '9' )		//??か
			return 1;		//はい
		else
			return 0;		//いいえ
	}
}

/*-------------------------------------------
	??チェック??
--------------------------------------------*/
int num_check(FILE *fp)
{
	int d0;
	d0 = f_get( fp );		//１バイト取り出し
	fseek(fp,-1,SEEK_CUR);		//１バイト
	if( d0 >= '0' && d0<= '9' )		//??か
		return 1;		//はい
	else
		return 0;		//いいえ
}
/*-------------------------------------------
	ステップタイ?iみ?み??
--------------------------------------------*/
int get_step(FILE *fp,int ch)
{
	int d0,futen,total;
	total = 0;
	futen = 0;
	while(1){
		d0 = f_get( fp );		//ＭＭＬ取り出し
		if( d0 >= '0' && d0<= '9' ){		//??なら
			fseek(fp,-1,SEEK_CUR);		//１バイト
			d0 = get_num( fp );
			if(d0 > 192)
				return -1;
			futen = 192 / d0;
			total += futen;
		} else if(d0 == '.'){		//付点なら
			if(!total){		//ト`タ?O定まだなら
				total = step_def[ch];		//付点?期セット
				futen = step_def[ch];		//
			}
			futen /= 2;
			total += futen;
		} else if(d0 == '^'){		//９８式タイなら
			d0 = f_get( fp );		//ＭＭＬ取り出し
			fseek(fp,-1,SEEK_CUR);		//１バイト
			if(d0 == '*'){		//~音Lなら
				continue;
			}
			if(!num_check( fp )){		//??でなければ
				return -1;		//エ?`
			}
			d0 = get_num( fp );
			if(d0 > 192)
				return -1;
			if(!total){		//ト`タ?O定まだなら
				total = step_def[ch];		//付点?期セット
			}
			futen = 192 / d0;
			total += futen;
		} else if(d0 == '*'){		//~音Lなら
			if(!num_check( fp )){		//??でなければ
				return -1;		//エ?`
			}
			futen = get_num( fp );
			total += futen;
		} else {
			fseek(fp,-1,SEEK_CUR);		//１バイト
			if(total)		//ト`タ?O定gみなら
				break;
			futen = step_def[ch];
			total = futen;
		}
	}
	return total;
}
/*-------------------------------------------
	ＭＭＬiみ?み?解析??
--------------------------------------------*/
int music_read( void )
{
	int d0,d1,d7,ch,ch_p[TRACK_MAX],do_p[TRACK_MAX],ch_oct[TRACK_MAX],err_flg=-1,err_line=0;
	int ch_ret[8][TRACK_MAX],ch_skip[8][TRACK_MAX],ch_kuri_lv[TRACK_MAX],waon_delay[TRACK_MAX];
	int o_tempo,q_com[TRACK_MAX];
	long d6;
	char moji[256];
	FILE *fp;
	fp = fopen( "oft.mml", "r" );
	if( fp == NULL ){
		sprintf( moji ,"%s" ,err_msg[0] );
#ifdef _DEBUG
		MessageBox(hWnd, moji, "Error", MB_OK);
#endif
		return err_flg;
	}

	for(d7=0;d7<TRACK_MAX;d7++){
		ch_p[d7] = 0;
		do_p[d7] = 0;
		ch_oct[d7] = 4;
		step_def[d7] = 48;
		play_track_flg[ d7 ] = 0;
		ch_kuri_lv[d7] = -1;
		for(d6=0;d6<8;d6++)
			ch_skip[d6][d7] = -1;
		q_com[d7] = 8;
		waon_delay[d7] = 0;
	}
	o_tempo=0;

	//??????????　解析_始　??????????

	while(1){

		//??????????　ト?ック番?O定　??????????

		err_line++;

		if((d0 = f_get( fp )) == EOF)		//ト?ック番?取り出し
			break;		//ファイ?のK端だ
		if( d0 == '\n' )		//改行か
			continue;			//そうならやり直し
		if( d0 == '/' ){		//コ??トか
			while(1){			//そうならスキップ
				d0 = f_get( fp );
				if(d0 == '\n' || d0 == EOF )
					break;
			}
			continue;			//そうならやり直し
		}
		if( d0 != '(' ){		//ト?ック_始か
			err_flg=M_TRACK_ERR;
			goto music_read_999;
		}
		if((d0 = f_get( fp )) == EOF)		//ト?ック番?取り出し
			break;		//ファイ?のK端だ
		if( d0 == 'p' ){		//?奏ト?ック指定か
			if((d0 = f_get( fp )) == EOF){		//全ト?ック?奏か
				err_flg=M_PLAY_ERR;
				goto music_read_999;

			}
			if( d0 == ')'){		//はい
				err_line--;		//まだAきが?るかもしれない
				continue;
			}

			fseek(fp,-1,SEEK_CUR);		//１バイト
			for(d7=0;d7<TRACK_MAX;d7++)		//?奏ト?ック?期化
				play_track_flg[ d7 ] = 0;	//

			while(1){
				if((d0 = fgetc( fp )) == EOF){		//K了か
					err_flg=M_PLAY_ERR;
					goto music_read_999;
				}
				if( d0 == ')')			//はい
					break;

				if( d0 >= '0' && d0<= '9' ){		//??か
					fseek(fp,-1,SEEK_CUR);		//１バイト
					d0 = get_num( fp );
					if(d0 < 1 || d0 > TRACK_MAX){
						err_flg=M_PLAY_ERR;
						goto music_read_999;
					}
					play_track_flg[ d0-1 ] = 1;	//?奏ト?ックセット
				}
			}
			err_line--;		//まだAきが?るかもしれない
			continue;
		}

		if( d0 == 's' ){		//ミ?`トト?ック指定か
			while(1){
				if((d0 = fgetc( fp )) == EOF){		//K了か
					err_flg=M_MUTE_ERR;
					goto music_read_999;
				}
				if( d0 == ')')			//はい
					break;

				if( d0 >= '0' && d0<= '9' ){		//??か
					fseek(fp,-1,SEEK_CUR);		//１バイト
					d0 = get_num( fp );
					if(d0 < 1 || d0 > TRACK_MAX){
						err_flg=M_MUTE_ERR;
						goto music_read_999;
					}
					play_track_flg[ d0-1 ] = 0;	//ミ?`トト?ックセット
				}
			}
			err_line--;		//まだAきが?るかもしれない
			continue;
		}

		if( d0 == 'o' ){		//テ?ポ指定か
			if(!num_check( fp )){		//??でなければ
				err_flg=M_TOCOMMAND_ERR;
				goto music_read_999;
			}
			o_tempo = get_num( fp );		//テ?ポ取り出し
			if((d0 = fgetc( fp )) == EOF){		//K了か
				err_flg=M_TOCOMMAND_ERR;
				goto music_read_999;
			}
			if(d0 != ')'){
				err_flg=M_TOCOMMAND_ERR;
				goto music_read_999;
			}
			err_line--;		//まだAきが?るかもしれない
			continue;
		}

		if( d0 != 't' ){		//ト?ック_始
			err_flg=M_TRACK_ERR;
			goto music_read_999;
		}
		if((d0 = f_get( fp )) == EOF)		//ト?ック番?取り出し
			break;		//ファイ?のK端だ
		if( d0 < '0' || d0> '9' ){		//ト?ック_始
			err_flg=M_TRACK_ERR;
			goto music_read_999;
		}
		fseek(fp,-1,SEEK_CUR);		//１バイト
		ch = get_num( fp );
		if((d0 = f_get( fp )) == EOF)		//ト?ック番?取り出し
			break;		//ファイ?のK端だ
		if( d0 != ')' ){		//ト?ック_始
			err_flg=M_TRACK_ERR;
			goto music_read_999;
		}

		if(ch < 1 || ch > TRACK_MAX){
			err_flg=M_TRACK_ERR;
			goto music_read_999;
		}

		ch--;		//ト?ック?定
		play_track_flg[ ch ] = 1;	//

		if(o_tempo!=0 && ch_p[ch]==0){		//テ?ポ指定が?ったら
			step[ch][ch_p[ch]++] = MUSIC_TEMPO;		//全ト?ックにテ?ポセット
#if MODE98
			step[ch][ch_p[ch]++] = (int)((float)120 / (float)o_tempo * (float)12600)*2;
#else
			step[ch][ch_p[ch]++] = (int)((float)120 / (float)o_tempo * (float)12600);
#endif
		}

		//??????????　ト?ックデ`タO定　??????????

music_read_100:
		if((d0 = f_get( fp )) == EOF)		//コマ?ド取り出し
			break;		//ファイ?のK端だ
		switch(d0){
		case '\n':		//改行
			break;
		case '/':		//コ??トか
			while(1){			//そうならスキップ
				d0 = f_get( fp );
				if(d0 == '\n' || d0 == EOF )
					break;
			}
			break;
		case '\'':		//和音なら
			int waon_step,waon_note[8+1],waon_cnt,octave,waon_delay_point;
			waon_step = waon_cnt = 0;
			octave = ch_oct[ch];

			while(1){
				d0 = f_get( fp );		//ＭＭＬ取り出し
				if( d0 >= '0' && d0<= '9' ){		//いきなり??なら
					err_flg=M_WAON_ERR;				//エ?`
					goto music_read_999;
				}
				if(d0 =='<'){		//オクタ`ブ上なら
					octave++;
					continue;
				}
				if(d0 =='>'){		//オクタ`ブ下なら
					octave--;
					if(octave < 0){		//オクタ`ブ０なら
						err_flg=M_NOTE_ERR;		//エ?`
						goto music_read_999;	//
					}
					continue;
				}
				if(d0 =='\''){		//和音K了なら
					break;
				}
				switch(d0){
				case 'c':		//音A
				case 'd':		//音A
				case 'e':		//音A
				case 'f':		//音A
				case 'g':		//音A
				case 'a':		//音A
				case 'b':		//音A
					d0-='a';
					d1 = note_change_tbl[d0];
					d0 = f_get( fp );		//シ?`プか
					if(d0 == '+')
						d1++;
					else if(d0 == '-')
						d1--;
					else fseek(fp,-1,SEEK_CUR);		//１バイト

					d0 = octave * 12 + d1 +1;	//ノ`ト番?オ`バ`なら
					if(d0 < 1 || d0 > 62){		//
						err_flg=M_NOTE_ERR;		//エ?`
						goto music_read_999;	//
					}
					waon_note[ waon_cnt++ ] = d0;	//ノ`ト番?セット

					d0 = f_get( fp );		//ＭＭＬ取り出し
					fseek(fp,-1,SEEK_CUR);		//１バイト
					if( d0 < '0' || d0> '9' ){		//??でなければ
						if( d0 != '*' ){		//?でもなければ??なら
							break;
						}
					}
					waon_step = get_step( fp, ch );		//ステップタイ?取り出し
					if(waon_step == -1){		//エ?`なら
						err_flg=M_WAON_ERR;				//エ?`
						goto music_read_999;
					}
					break;
				default:
					err_flg=M_WAON_ERR;				//エ?`
					goto music_read_999;
				}
			}
			if(waon_cnt > 8 || waon_cnt == 0){		//和音オ`バ`なら
				err_flg=M_WAON_ERR;				//エ?`
				goto music_read_999;
			}

			if(num_check( fp )){		//??なら
				waon_step = get_num( fp );		//ステップタイ?セット
			}
			step[ch][ch_p[ch]++] = MUSIC_WAON;		//和音セット
			step[ch][ch_p[ch]++] = waon_cnt;		//和音?セット
			waon_delay_point = ch_p[ch];		//和音ディ?イポイ?ト保存
			ch_p[ch]++;		//後でO定する

			for(d7=0;d7<waon_cnt;d7++){
				step[ch][ch_p[ch]++] = waon_note[d7];		//ノ`ト番?セット
			}
			if(waon_step){		//ステップタイ?の指定が?ったら
				d0 = waon_step;		//ステップタイ?セット
			} else {
				d0 = step_def[ch];	//ステップタイ?セット
			}
			step[ch][ch_p[ch]++] = d0;		//ステップタイ?セット
			d1 = f_get( fp );		//タイなら
			if(d1 == '&'){
				step[ch][ch_p[ch]++] = -1;	//ゲ`トタイ?セット
			} else {
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step[ch][ch_p[ch]++] = d0 * q_com[ch] / 8;	//ゲ`トタイ?セット
			}

			d0 = f_get( fp );		//ディ?イ指定?るなら
			if(d0 == ','){
				if(num_check( fp )){		//??なら
					waon_delay[ch] = get_num( fp );		//ディ?イ・辘坤
				} else {		//??でなければ
					err_flg=M_WAON_ERR;				//エ?`
					goto music_read_999;
				}
			} else {		//ディ?イ指定ないなら
				fseek(fp,-1,SEEK_CUR);		//１バイト
			}
			step[ch][waon_delay_point] = waon_delay[ch];		//ディ?イセット
			goto music_read_100;
		case 'l':		//音Lか
			d0 = f_get( fp );		//?は?
			if( d0 >= '0' && d0<= '9' ){		//??か
				fseek(fp,-1,SEEK_CUR);		//１バイト
				d0 = 192 / get_num( fp );
				if(d0 > 192 || d0 == 0){
					err_flg=M_LCOMMAND_ERR;
					goto music_read_999;
				}
				step_def[ ch ] = d0;
				goto music_read_100;
			}
			if( d0 != '*' ){		//~音Lか
				err_flg=M_LCOMMAND_ERR;
				goto music_read_999;
			}
			d0 = f_get( fp );		//?は?
			if( d0 >= '0' && d0<= '9' ){		//??か
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step_def[ ch ] = get_num( fp );
				goto music_read_100;
			}
			err_flg=M_LCOMMAND_ERR;
			goto music_read_999;
		case 'r':		//休?
			step[ch][ch_p[ch]++]=MUSIC_R;
			d0 = get_step( fp, ch );		//ステップタイ?取り出し
			if(d0 == -1){		//エ?`か
				err_flg=M_STEP_ERR;		//そうなら?へ
				goto music_read_999;
			}
			step[ch][ch_p[ch]++] = d0;		//ステップタイ?セット
			d1 = f_get( fp );		//タイなら
			if(d1 == '&'){
				step[ch][ch_p[ch]++] = -1;	//ゲ`トタイ?セット
			} else {
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step[ch][ch_p[ch]++] = d0 * q_com[ch] / 8;	//ゲ`トタイ?セット
			}
			goto music_read_100;
		case 'o':		//オクタ`ブ指定か
			if(!num_check( fp )){		//??でなければ
				err_flg=M_OCOMMAND_ERR;		//エ?`
				goto music_read_999;
			}
			ch_oct[ch] = get_num( fp );
			if(ch_oct[ch]<0 || ch_oct[ch]>5){		//焱猡胜
				err_flg=M_OCOMMAND_ERR;		//エ?`
				goto music_read_999;
			}
			goto music_read_100;
		case '<':		//オクタ`ブ上か
			ch_oct[ch]++;
			goto music_read_100;
		case '>':		//オクタ`ブ下か
			ch_oct[ch]--;
			if(ch_oct[ch] < 0){		//オクタ`ブ０なら
				err_flg=M_NOTE_ERR;		//エ?`
				goto music_read_999;	//
			}
			goto music_read_100;
		case 'q':		//ゲ`トタイ?か
				if(!num_check( fp )){		//??でなければ
						err_flg=M_QCOMMAND_ERR;
						goto music_read_999;
				}
				q_com[ch] = get_num( fp );		//ゲ`トタイ?セット
				if(q_com[ch] > 8 || q_com[ch] == 0){		//焱猡胜
					err_flg=M_QCOMMAND_ERR;
					goto music_read_999;
				}
			goto music_read_100;
		case 'c':		//音A
		case 'd':		//音A
		case 'e':		//音A
		case 'f':		//音A
		case 'g':		//音A
		case 'a':		//音A
		case 'b':		//音A
//			if(ch >= 6){
				d0-='a';
				d1 = note_change_tbl[d0];
				d0 = f_get( fp );		//シ?`プか
				if(d0 == '+')
					d1++;
				else if(d0 == '-')
					d1--;
				else fseek(fp,-1,SEEK_CUR);		//１バイト

				if(ch_oct[ch] >= 5){			//ノ`ト番?オ`バ`か
					if(d1 >= 2){			//
						err_flg=M_NOTE_ERR;	//はい
						goto music_read_999;	//
					}
				}
#if 0
				step[ch][ch_p[ch]++] = MUSIC_FREQ;
				d1 = freq_tbl[d1+1];
				if(ch_oct[ch] > 0){
					d0 = ch_oct[ch];
					d0 = 1 << d0;
					d1 *= d0;
				} else if(ch_oct[ch] < 0){
					d0 = -ch_oct[ch];
					d0 = 1 << d0;
					d1 /= d0;
				}
#endif
				d0 = ch_oct[ch] * 12 + d1 + 1;	//ノ`ト番?オ`バ`なら
				if(d0 < 1 || d0 > 62){			//
					err_flg=M_NOTE_ERR;			//エ?`
					goto music_read_999;		//
				}
				step[ch][ch_p[ch]++] = d0;	//ノ`ト番?セット
//			}
			d0 = get_step( fp, ch );		//ステップタイ?取り出し
			if(d0 == -1){		//エ?`か
				err_flg=M_STEP_ERR;		//そうなら?へ
				goto music_read_999;
			}
			step[ch][ch_p[ch]++] = d0;		//ステップタイ?セット
			d1 = f_get( fp );		//タイなら
			if(d1 == '&'){
				step[ch][ch_p[ch]++] = -1;	//ゲ`トタイ?セット
			} else {
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step[ch][ch_p[ch]++] = d0 * q_com[ch] / 8;	//ゲ`トタイ?セット
			}
			goto music_read_100;
		case 't':		//テ?ポ
			d0 = f_get( fp );		//テ?ポ取り出し
			if( d0 < '0' || d0> '9' ){		//??か
				err_flg=M_TCOMMAND_ERR;
				goto music_read_999;
			}
			fseek(fp,-1,SEEK_CUR);		//１バイト
			d0 = get_num( fp );
			step[ch][ch_p[ch]++] = MUSIC_TEMPO;
#if MODE98
			step[ch][ch_p[ch]++] = (int)((float)120 / (float)d0 * (float)12600)*2;
#else
			step[ch][ch_p[ch]++] = (int)((float)120 / (float)d0 * (float)12600);
#endif
			goto music_read_100;
		case 'v':		//ボ??`?
			d0 = f_get( fp );		//音量取り出し
			if( d0 < '0' || d0> '9' ){		//??か
				err_flg=M_VCOMMAND_ERR;
				goto music_read_999;
			}
			fseek(fp,-1,SEEK_CUR);		//１バイト
			d0 = get_num( fp );
			if(d0>15){		//音量オ`バ`か
				err_flg=M_VCOMMAND_ERR;
				goto music_read_999;
			}
			step[ch][ch_p[ch]++]=MUSIC_VOL;
			if( d0 ){
					d0 = 15 - d0;
//					d0 *= d0;
//					d0 = (int)((float)d0 * 39.0625);
//					step[ch][ch_p[ch]++] = -d0;
					d6 = d0;
					d6 = d6*d6*d6;
					d6 = (long)(10000.0 / ( 16.0 * 16.0 * 16.0 ) * d6 );
					step[ch][ch_p[ch]++] = -d6;
			} else {
				step[ch][ch_p[ch]++] = -10000;
			}
			goto music_read_100;
		case '@':		//?コマ?ド
			d0 = f_get( fp );		//コマ?ド取り出し

			if( d0 >= '0' && d0<= '9' ){		//??か
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step[ch][ch_p[ch]++] = MUSIC_NEIRO_NO;		//音色番?セット
				step[ch][ch_p[ch]++] = get_num( fp ) - 1;
				goto music_read_100;
			}

			switch(d0){
			case 'v':		//ボ??`?
				d0 = f_get( fp );		//音量取り出し
				if( d0 < '0' || d0> '9' ){		//??か
					err_flg=M_V2COMMAND_ERR;
					goto music_read_999;
				}
				fseek(fp,-1,SEEK_CUR);		//１バイト
				d0 = get_num( fp );
				if(d0>127){		//音量オ`バ`か
					err_flg=M_V2COMMAND_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++]=MUSIC_VOL;
#if 0
				if( d0 ){
					d0 = 127 - d0;
//					d0 *= d0;
//					d0 = (int)((float)d0 * 0.61);
//					step[ch][ch_p[ch]++] = -d0;
					d6 = d0;
					d6 = d6*d6*d6;
					d6 = (long)(10000.0 / ( 128.0 * 128.0 * 128.0 ) * d6 );
					step[ch][ch_p[ch]++] = -d6;
				} else {
					step[ch][ch_p[ch]++] = -10000;
				}
#else
//				d6=(long)(LOG(100,127)*10000) - (long)(LOG(100,(double)(d0))*10000);
//				step[ch][ch_p[ch]++] = -d6;

				step[ch][ch_p[ch]++] = d0;

#endif
				goto music_read_100;
			case 'h':		//?Ｈ
				if(!num_check( fp )){		//??でなければ
					err_flg=M_H2COMMAND_ERR;
					goto music_read_999;
				}
				d0 = get_num( fp );
				step[ch][ch_p[ch]++] = MUSIC_LFO_H;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'm':		//?Ｍ
				if(!num_check( fp )){		//??でなければ
					err_flg=M_M2COMMAND_ERR;
					goto music_read_999;
				}
				d0 = get_num( fp );
				if(d0 > 127){		//焱猡胜
					err_flg=M_M2COMMAND_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++] = MUSIC_LFO_M;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 's':		//?Ｓ
				if(!num_check2( fp )){		//??でなければ
					err_flg=M_S2COMMAND_ERR;
					goto music_read_999;
				}
				d0 = get_num2( fp );
				step[ch][ch_p[ch]++] = MUSIC_LFO_S;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'p':		//パ?ポット
				d0 = f_get( fp );		//移恿咳・瓿訾
				if( d0 < '0' || d0 > '9' ){		//??か
					err_flg=M_P2COMMAND_ERR;
					goto music_read_999;
				}
				fseek(fp,-1,SEEK_CUR);		//１バイト
				d0 = get_num( fp );
				if( d0 > 127 ){		//欹`バ`か
					err_flg=M_P2COMMAND_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++]=MUSIC_PAN;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'f':		//周波?
				if(!num_check2( fp )){		//??でなければ
					err_flg=M_F2COMMAND_ERR;
					goto music_read_999;
				}
				d0 = get_num2( fp );
				step[ch][ch_p[ch]++] = MUSIC_FREQ;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'k':		//周波?ずらし
				if(!num_check2( fp )){		//??でなければ
					err_flg=M_K2COMMAND_ERR;
					goto music_read_999;
				}
				d0 = get_num2( fp );
				if(d0 > 64 || d0 < -64){		//焱猡胜
					err_flg=M_K2COMMAND_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++] = MUSIC_DETUNE;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'd':		//ダ?パ`ヘダ?
				d0 = f_get( fp );		//ダ?パ`ヘダ?
				if( d0 < '0' || d0> '9' ){		//??か
					err_flg=M_D2COMMAND_ERR;
					goto music_read_999;
				}
				fseek(fp,-1,SEEK_CUR);		//１バイト
				d0 = get_num( fp );
				step[ch][ch_p[ch]++] = MUSIC_PEDAL;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			case 'w':		//??`ス?`ト
				d0 = f_get( fp );		//??`ス?`ト
				if( d0 < '0' || d0> '9' ){		//??か
					err_flg=M_W2COMMAND_ERR;
					goto music_read_999;
				}
				fseek(fp,-1,SEEK_CUR);		//１バイト
				d0 = get_num( fp );
				step[ch][ch_p[ch]++] = MUSIC_WAVE;
				step[ch][ch_p[ch]++] = d0;
				goto music_read_100;
			default:
				err_flg=M_2COMMAND_ERR;
				goto music_read_999;
			}
		case '[':		//カッココマ?ド
			d0 = f_get( fp );		//コマ?ド取り出し
			switch(d0){
			case 'l':
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != 'o'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != 'o'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != 'p'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != ']'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++]=MUSIC_LOOP;
				step[ch][ch_p[ch]++]=do_p[ch];
				goto music_read_100;
			case 'd':		//?`プ位置
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != 'o'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				d0 = f_get( fp );		//コマ?ド取り出し
				if(d0 != ']'){
					err_flg=M_KAKKO_ERR;
					goto music_read_999;
				}
				do_p[ch]=ch_p[ch];		//晗去互氓
				goto music_read_100;
			default:
					err_flg=M_KAKKO_ERR;
				goto music_read_999;
			}
		case '|':		//Rり返しコマ?ド
			d0 = f_get( fp );		//コマ?ド取り出し
			if(d0 == ':'){		//Rり返し_始か
				ch_kuri_lv[ch]++;		//A?ベ?アップ
				if(ch_kuri_lv[ch] == 8){		//Rり返ししすぎか
					err_flg=M_KURI_ERR;
					goto music_read_999;
				}
				step[ch][ch_p[ch]++] = MUSIC_KURI_START;
				if(num_check( fp )){		//??か
					step[ch][ch_p[ch]++] = get_num( fp );
				} else {
					step[ch][ch_p[ch]++] = 2;
				}
				ch_ret[ch_kuri_lv[ch]][ ch ] = ch_p[ch];
			} else {
				fseek(fp,-1,SEEK_CUR);		//１バイト
				step[ch][ch_p[ch]++]=MUSIC_KURI_SKIP;
				ch_skip[ch_kuri_lv[ch]][ ch ] = ch_p[ch];
				ch_p[ch]++;
			}
			goto music_read_100;

		case ':':		//Rり返しK了コマ?ド
			d0 = f_get( fp );		//コマ?ド取り出し
			if(d0 != '|'){		//Rり返しK了か
				err_flg=M_KURI_ERR;
				goto music_read_999;
			}
			step[ch][ch_p[ch]++] = MUSIC_KURI_RET;
			step[ch][ch_p[ch]++] = ch_ret[ch_kuri_lv[ch]][ ch ];		//_始?
			if(ch_skip[ch_kuri_lv[ch]][ ch ] >= 0){
				step[ch][ch_skip[ch_kuri_lv[ch]][ ch ]] = ch_p[ch];	//脱出?
				ch_skip[ch_kuri_lv[ch]][ ch ] = -1;
			}
			ch_kuri_lv[ch]--;		//A?ベ?ダウ?
			goto music_read_100;

		default:		//ＭＭＬエ?`
			err_flg=M_MML_ERR;
			goto music_read_999;
		}
	}
	for(d7=0;d7<TRACK_MAX;d7++){
		step[d7][ch_p[d7]]=MUSIC_END;
	}
	err_flg=0;

music_read_999:
	fclose( fp );
	if(err_flg){
		sprintf( moji ,"%d 行目 %s" ,err_line ,err_msg[err_flg] );
#ifdef _DEBUG
		MessageBox(hWnd, moji, "Error", MB_OK);
#endif
	} else {
		for(d7=0;d7<TRACK_MAX;d7++){
			if(ch_kuri_lv[d7]!=-1){
				err_flg=M_KURISU_ERR;
				sprintf( moji ,"ト?ック %d %s" ,d7+1 ,err_msg[err_flg] );
#ifdef _DEBUG
				MessageBox(hWnd, moji, "Error", MB_OK);
#endif
				break;
			}
		}
	}
	return err_flg;
}
/*-------------------------------------------
	ド?イバ`?期化??
--------------------------------------------*/
void music_init( void )
{
	int d7,d5;
	for(d7=0;d7<VOICE_MAX;d7++){
		voice[ d7 ].name = 0;
		voice[ d7 ].tone_no = -1;		//タスク?のプ?セット音色番?
		voice[ d7 ].loop_flg = 0;
	}
	for(d7=0;d7<TRACK_MAX;d7++){
		step_time[ d7 ] = 0;
		gate_time[ d7 ] = 0;
		step_point[ d7 ] = 0;
		play_flg[ d7 ] = 1;
		play_kuri_lv[ d7 ] = -1;
		dataToneNo[ d7 ] = 0;		//ト?ック?のプ?セット音色番?
		dataVolume[ d7 ] = 0;
		dataPan[ d7 ] = 0;
		dataFrequency[ d7 ] = 0;
		tie_flg[ d7 ] = 0;
		for(d5=0;d5<8;d5++){
			play_use_task[ d7 ][ d5 ] = -1;		//使用タスク番??期化
		}
		play_use_task_cnt[ d7 ] = 0;
		play_detune[ d7 ] = 0;
		play_lfo_h[ d7 ] = 0;
		play_lfo_m[ d7 ] = 0;
		play_lfo_s[ d7 ] = 0;
	}
	//h境音?期化
	for(d7=0;d7<MAX_ENV;d7++){
		env_tbl[d7].voice_address = -1;
		env_tbl[d7].count = 0;
	}

#if 0	///////////////////////////////////////////////////
	QueryPerformanceCounter( &d_cnt );

#if VOICE_KILL_ON
	voice_kill_point=0;
#endif
#if MODE98
	tempo.LowPart = 12600;
#else
	tempo.LowPart = 12600 *2;
#endif
	pause_flg=0;
#if 0
	dataVolume[1]=0;
	dataVolume[2]=-400;
	pDSData[2]->SetVolume(dataVolume[2]);
	pDSData[3]->SetVolume(-400);
	pDSData[4]->SetVolume(-400);
	pDSData[5]->SetVolume(-900);
	dataFrequency[5]=22050;
	pDSData[2]->SetPan(-700);
	pDSData[3]->SetPan(1000);
	pDSData[0]->SetFrequency((DWORD)50000);
	pDSData[1]->SetFrequency((DWORD)46000);
#endif
//	double y;
//	for(d7=0;d7<128;d7++){
//		y=-(10000-sin((double)d7*(PAI/256))*10000);
//	}
//	double y;
//	for(d7=0;d7<128;d7++){
//		y=(sqrt(d7)*3000/sqrt(128))-3000+sqrt(128);
//	}
#endif	///////////////////////////////////////////////////

#define PAN_POINT	5000
	//パ?テ`ブ?作成
	int y;
	double d6;		//左
	for(d7=64,d6=127;d6>=0;d6--){
		if(!((int)d6&1))
			continue;
		y=(int)(LOG(100,127)*PAN_POINT) - (int)(LOG(100,d6)*PAN_POINT);
		if(y>PAN_POINT)
			y=10000;
		panpot_tbl[d7++]=y;
	}
	for(d7=64,d6=127;d6>=0;d6--){
		if(!((int)d6&1))
			continue;
		y=(int)(LOG(100,127)*PAN_POINT) - (int)(LOG(100,d6)*PAN_POINT);
		if(y>PAN_POINT)
			y=10000;
		panpot_tbl[d7--]=-y;
	}
	d6=panpot_tbl[0]=-10000;
	d6=panpot_tbl[127]=10000;

	//ボ??`?テ`ブ?作成
	for(d6=127;d6>=0;d6--){
		y=(int)(LOG(100,127)*10000) - (int)(LOG(100,d6)*10000);
		if(y>10000)
			y=10000;
		volume_tbl[(int)d6]=-y;
	}

	//デチ?`?テ`ブ?作成
	for(d7=0;d7<12*5+5;d7++){
		detune_tbl[d7]=freq_tbl[d7+1]-freq_tbl[d7];
	}
}

/*-------------------------------------------
	再生?断??
--------------------------------------------*/
void play_stop( void )
{
	int d7;
	for(d7=0;d7<VOICE_MAX;d7++){
		pDSData[d7]->Stop();
		pDSData[d7]->SetCurrentPosition(0);
//		play_flg[ d7 ] = 0;
	}
	pause_flg=1;
}
void stop_voice(int ch);
/*-------------------------------------------
	再生_始??
--------------------------------------------*/
void play_start( void )
{
	int d7;
	for(d7=0;d7<VOICE_MAX;d7++){
		pDSData[d7]->Stop();
		pDSData[d7]->SetCurrentPosition(0);
	}
	music_init();		//ド?イバ`?期化
}

//??????????　ＬＦＯ??　??????????
void voice_lfo( int a0 )
{
	long d0;
	if(!voice[ a0 ].lfo_m){		//ＬＦＯoしなら
		return;
	}
	if(!voice[ a0 ].lfo_s){		//ＬＦＯoしなら
		return;
	}
	if(voice[ a0 ].lfo_h){			//ＬＦＯ_始でなければ
		voice[ a0 ].lfo_h--;
		return;
	}

	voice[ a0 ].lfo_s_cnt += voice[ a0 ].lfo_s_ang;		//今回のＬＦＯ量

//	if(voice[ a0 ].lfo_s_ang >= 0){		//方向がプ?スなら
	if(voice[ a0 ].lfo_s_cnt >= 0){		//方向がプ?スなら
		d0 = detune_tbl[ voice[ a0 ].note_no ] * voice[ a0 ].lfo_m / 128 * voice[ a0 ].lfo_s_cnt / abs(voice[ a0 ].lfo_s);
	} else {
		d0 = detune_tbl[ voice[ a0 ].note_no-1 ] * voice[ a0 ].lfo_m / 128 * voice[ a0 ].lfo_s_cnt / abs(voice[ a0 ].lfo_s);
	}
	pDSData[ a0 ]->SetFrequency((DWORD)(voice[ a0 ].freq + d0));			// 周波?セット

	if(abs(voice[ a0 ].lfo_s_cnt) == abs(voice[ a0 ].lfo_s)){		//折り返しなら
		voice[ a0 ].lfo_s_ang *= -1;
	}
}

//??????????　ボイス?イ?　??????????
#define VOICE_KEY_ON		1
#define VOICE_NORMAL		2
#define VOICE_TIE_SET		10
#define VOICE_KEY_OFF		100
#define VOICE_KEY_OFF_FAST	110
#define VOICE_KEY_OFF_WAIT	200

void voice_action( int a0 )
{
	DWORD	Status = 0;

	switch(voice[ a0 ].name){
	case VOICE_KEY_ON:		//通常キ`オ?
		if(voice[ a0 ].delay_cnt){		//キ`オ?はまだなら
			voice[ a0 ].delay_cnt--;
			return;
		}
		pDSData[ a0 ]->SetVolume(volume_tbl[ voice[ a0 ].volume ]);		// ボ??`?セット
		pDSData[ a0 ]->SetFrequency((DWORD)voice[ a0 ].freq);			// 周波?セット
		pDSData[ a0 ]->SetPan(panpot_tbl[ voice[ a0 ].pan ]);			// パ?ポットセット
		pDSData[ a0 ]->Play(0, 0, voice[ a0 ].loop_flg);				//キ`オ?
		voice[ a0 ].name = VOICE_NORMAL;
		voice_lfo( a0 );		//ＬＦＯ??へ
		break;
	case VOICE_NORMAL:		//通常キ`オ??
		voice_lfo( a0 );		//ＬＦＯ??へ
		break;
	case VOICE_TIE_SET:		//タイ用再セット
		pDSData[ a0 ]->SetVolume(volume_tbl[ voice[ a0 ].volume ]);		// ボ??`?セット
		pDSData[ a0 ]->SetFrequency((DWORD)voice[ a0 ].freq);		// 周波?セット
		pDSData[ a0 ]->SetPan(panpot_tbl[ voice[ a0 ].pan ]);			// パ?ポットセット
		voice[ a0 ].name = VOICE_NORMAL;
		break;
	case VOICE_KEY_OFF:		//通常キ`オフ
//		if( pDSData[ a0 ]->GetStatus( &Status ) == DS_OK ){
///		pDSData[ a0 ]->GetStatus( &Status );		//ボイスステ`タス取り出し
///		if( Status & DSBSTATUS_PLAYING || Status & DSBSTATUS_LOOPING ){		//再生?なら
			if(voice[ a0 ].volume){		//ボ??`?０以外なら
				voice[ a0 ].release_cnt++;		//ボ??`?涓?gか
				if(voice[ a0 ].release_spd != voice[ a0 ].release_cnt){
					voice_lfo( a0 );		//ＬＦＯ??へ
					break;		//そうでなければiける
				}
				voice[ a0 ].release_cnt = 0;		//ボ??`?涓
				voice[ a0 ].volume--;		//ボ??`?下げる
//				if(voice[ a0 ].volume < 0){		//ボ??`?０越したら
//					voice[ a0 ].volume=0;		//ボ??`?０にする
//				}
				pDSData[ a0 ]->SetVolume(volume_tbl[ voice[ a0 ].volume ]);		// ボ??`?涓
				voice_lfo( a0 );		//ＬＦＯ??へ
				break;		//そうならiける
			}
///		}
		pDSData[ a0 ]->Stop();							//キ`オフ
		pDSData[ a0 ]->SetCurrentPosition(0);			//
		voice[ a0 ].name = VOICE_KEY_OFF_WAIT;			//消されるまで待つ
//		play_use_task[ voice[ a0 ].track_no	] = -1;		//
		break;
#if !VOICE_KILL_ON
	case VOICE_KEY_OFF_FAST:		//速攻キ`オフ
		pDSData[ a0 ]->Stop();					//キ`オフ
		pDSData[ a0 ]->SetCurrentPosition(0);			//
		voice[ a0 ].name = 0;					//
		break;
#endif
	}
}
//??????????　ボイスパ??`タセット　??????????
void set_voice_param( int task_no, long note, int delay, int lfo_flg)
{
	voice[ task_no ].volume = dataVolume[ch];			// ボ??`?セット
//	voice[ task_no ].freq = freq + dataFrequency[ch];	// 周波?セット
	// 周波?セット
	if(play_detune[ch]>0){		//デチ?`?がプ?スなら
		voice[ task_no ].freq = freq_tbl[ note ] + detune_tbl[ note ]*play_detune[ch]/64 + dataFrequency[ch];
	} else if(play_detune[ch]<0){		//デチ?`?がマイナスなら
		voice[ task_no ].freq = freq_tbl[ note ] + detune_tbl[ note-1 ]*play_detune[ch]/64 + dataFrequency[ch];
	} else {					//デチ?`?指定がないなら
		voice[ task_no ].freq = freq_tbl[ note ] + dataFrequency[ch];
	}
	voice[ task_no ].note_no = note;				// ノ`ト番?セット
	voice[ task_no ].pan = dataPan[ch];				// パ?ポットセット
	voice[ task_no ].release_spd = wave[ch];		// エ?ベ?`プセット
	voice[ task_no ].release_cnt = 0;				//
	if(voice[ task_no ].tone_no == 12-1){				//?`プ音色なら
		voice[ task_no ].loop_flg = DSBPLAY_LOOPING;	//
	} else {
		voice[ task_no ].loop_flg = 0;					//
	}
	if(delay >= 0){								//和音ディ?イセットするなら
		voice[ task_no ].delay_cnt = delay;		//
	}
	if(lfo_flg){		//ＬＦＯセットするなら
		voice[ task_no ].lfo_h = play_lfo_h[ ch ];					//ＬＦＯセット
		voice[ task_no ].lfo_m = play_lfo_m[ ch ];					//
		if((voice[ task_no ].lfo_s = play_lfo_s[ ch ]) >= 0){		//
			voice[ task_no ].lfo_s_ang = 1;							//
		} else {
			voice[ task_no ].lfo_s_ang = -1;						//
		}
		voice[ task_no ].lfo_s_cnt = 0;								//
	}
}

//??????????　ボイスキ`オ?　??????????
static voice_seek_point=0;
int	set_tone( int tone, long note, int use_task_cnt, int delay)
{
	int d7,d1=0;

	for(d7=0;d7<VOICE_MAX;d7++){		//空いてタスクを探す
//		if( voice[ d7 ].name == 0){		//これか
		if( voice[ voice_seek_point ].name == 0){		//これか
			d1=1;										//そうなら?へ
			break;
		}
		voice_seek_point++;
		voice_seek_point&=VOICE_MAX-1;
	}
	if(!d1){		//空いてなかったか
#ifdef _DEBUG
		MessageBox(hWnd, "パ`ト切れ", "Error", MB_OK);		//そうならiける
#endif
		play_use_task[ch][use_task_cnt]=-1;		//使用タスクなし
		return -1;
	}
	d7 = voice_seek_point;

	play_use_task[ch][use_task_cnt]=d7;		//使用タスク番?保存

	voice[ d7 ].name = VOICE_KEY_ON;			//通常再生??へ

	if(voice[ d7 ].tone_no == tone)		//同じ音色番?か
		goto set_tone_500;				//そうなら?へ

	voice[ d7 ].tone_no = tone;		//音色番?保存

	RELEASE(pDSData[d7]);

	if( pDSound->DuplicateSoundBuffer(pDSData_tone[tone],&pDSData[d7]) != DS_OK ){		//音色コピ`
#ifdef _DEBUG
		MessageBox(hWnd, "セカ?ダ??バッファの}uに失", "Error", MB_OK);
#endif
		play_use_task[ch][use_task_cnt]=-1;		//使用タスクなし
		return -1;
	}

set_tone_500:
	set_voice_param( d7, note, delay, 1 );		//ボイスパ??`タセット

	return 0;
}

//??????????　速攻キ`オフ　??????????
void stop_voice(void)
{
	for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
		if(play_use_task[ch][cnt]!=-1){					//タスク使用?か
#if VOICE_KILL_ON
			voice_kill_buf[voice_kill_point++]=play_use_task[ch][cnt];
#else
			voice[ play_use_task[ch][cnt] ].name = VOICE_KEY_OFF_FAST;		//そうなら速攻キ`オフ
#endif
			play_use_task[ch][cnt]=-1;					//タスク未使用
		}
	}
}

//??????????　ド?イバ`?イ?　??????????
void gate(void)
{
	int next_command;
	DWORD	Status = 0;
	if(!play_flg[ ch ])		//停止?か
		return;				//そうならiける

	gate_time[ch]--;		//ゲ`トタイ?更新
	if(!gate_time[ch]){		//キ`オフなら
		if( !d_pedal[ch] ){		//キ`オフするなら
			if( !wave[ch] ){	//??`ス０なら
				stop_voice();		//すぐにキ`オフ
			} else {
				for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
					if(play_use_task[ch][cnt]!=-1){					//タスク使用?か
						voice[ play_use_task[ch][cnt] ].name = VOICE_KEY_OFF;	//そうならキ`オフ
					}
				}
			}
		}
	}

	step_time[ch]--;			//キ`オ?か
	if(step_time[ch]>0){		//
		return;				//そうでなければiける
	} else {
		if(!tie_flg[ch]){		//タイでなければ
			if( !d_pedal[ch] ){		//キ`オフするなら
				if( !wave[ch] ){	//??`ス０なら
					stop_voice();		//すぐにキ`オフ
				} else {
					for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
						if(play_use_task[ch][cnt]!=-1){					//タスク使用?か
							voice[ play_use_task[ch][cnt] ].name = VOICE_KEY_OFF;	//そうならキ`オフ
						}
					}
				}
			}
		}

gate_100:
		next_command = step[ch][step_point[ch]++];		//コマ?ド取り出し

		if(next_command >= 0){		//ノ`ト番?なら
			if(play_use_task_cnt[ch]!=1 && tie_flg[ch]==1){		//和音の使用?が`うなら
				tie_flg[ch]=0;		//タイ?止
			}
			if(!tie_flg[ch]){		//タイでなければ
				stop_voice();		//使用?のボイス停止
				set_tone( dataToneNo[ch], next_command, 0, 0);	//g音キ`オ?
				note_old[ch][0] = next_command;		//今回のノ`ト番?保存
			} else {
				for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
					//タイ用再セット
					voice[ play_use_task[ch][cnt] ].name = VOICE_TIE_SET;
					//ボイスパ??`タセット
					set_voice_param( play_use_task[ch][cnt], next_command, -1, 0 );
					note_old[ch][cnt] = next_command;		//今回のノ`ト番?保存
				}
			}
			step_time[ch] = step[ch][step_point[ch]++];		//ステップタイ?
			gate_time[ch] = step[ch][step_point[ch]++];		//ゲ`トタイ?
			if(gate_time[ch]==-1){		//タイなら
				tie_flg[ch]=1;
			} else {
				tie_flg[ch]=0;
			}
			play_use_task_cnt[ch] = 1;		//k音?セット
			return;
		}
		if(next_command==MUSIC_WAON){		//和音なら
			int waon_cnt,waon_delay,waon_delay_total=0;
			waon_cnt = step[ch][step_point[ch]++];		//和音?取り出し
			waon_delay = step[ch][step_point[ch]++];		//和音ディ?イ取り出し
			if(waon_cnt != play_use_task_cnt[ch] && tie_flg[ch]==1){		//和音の使用?が`うなら
				tie_flg[ch]=0;		//タイ?止
			}
			if(!tie_flg[ch]){		//タイでなければ
				stop_voice();		//使用?のボイス停止
				for(int cnt=0;cnt<waon_cnt;cnt++){
					//和音キ`オ?
					set_tone( dataToneNo[ch], step[ch][step_point[ch]], cnt, waon_delay_total);
					note_old[ch][cnt] = step[ch][step_point[ch]++];		//今回のノ`ト番?保存
					waon_delay_total += waon_delay;
				}
			} else {
				for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
					//タイ用再セット
					voice[ play_use_task[ch][cnt] ].name = VOICE_TIE_SET;
					//ボイスパ??`タセット
					set_voice_param( play_use_task[ch][cnt], step[ch][step_point[ch]], -1, 0 );
					note_old[ch][cnt] = step[ch][step_point[ch]++];		//今回のノ`ト番?保存
				}
			}
			step_time[ch] = step[ch][step_point[ch]++];		//ステップタイ?
			gate_time[ch] = step[ch][step_point[ch]++];		//ゲ`トタイ?
			if(gate_time[ch]==-1){		//タイなら
				tie_flg[ch]=1;
			} else {
				tie_flg[ch]=0;
			}
			play_use_task_cnt[ch] = waon_cnt;		//k音?セット
			return;
		}
		if(next_command==MUSIC_TEMPO){		//テ?ポか
			tempo.LowPart = step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_NEIRO_NO){		//音色切り替えか
//			change_neiro( ch, step[ch][step_point[ch]++] );
			dataToneNo[ch] = step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_LOOP){		//?`プか
			step_point[ch] = step[ch][step_point[ch]];
			goto gate_100;
		}
		if(next_command==MUSIC_KURI_START){		//Rり返し_始か
			play_kuri_lv[ch]++;		//A?ベ?アップ
			play_kuri_num[play_kuri_lv[ch]][ch] = step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_KURI_SKIP){		//Rり返し脱出か
			if(play_kuri_num[play_kuri_lv[ch]][ch] == 1){
				step_point[ch] = step[ch][step_point[ch]];
				play_kuri_lv[ch]--;		//A?ベ?ダウ?
			} else {
				step_point[ch]++;
			}
			goto gate_100;
		}
		if(next_command==MUSIC_KURI_RET){		//Rり返しK了か
			play_kuri_num[play_kuri_lv[ch]][ch]--;
			if(play_kuri_num[play_kuri_lv[ch]][ch] > 0){
				step_point[ch] = step[ch][step_point[ch]];
			} else {
				step_point[ch]++;
				play_kuri_lv[ch]--;		//A?ベ?ダウ?
			}
			goto gate_100;
		}
		if(next_command==MUSIC_R){		//Ｒか
			if(tie_flg[ch]){		//タイなら
				for(int cnt=0;cnt<play_use_task_cnt[ch];cnt++){
					//タイ用再セット
					voice[ play_use_task[ch][cnt] ].name = VOICE_TIE_SET;
					//ボイスパ??`タセット
					set_voice_param( play_use_task[ch][cnt], note_old[ch][cnt], -1, 0 );
				}
			}
			step_time[ch] = step[ch][step_point[ch]++];		//ステップタイ?
			gate_time[ch] = step[ch][step_point[ch]++];		//ゲ`トタイ?
			if(gate_time[ch]==-1){		//タイなら
				tie_flg[ch]=1;
			} else {
				tie_flg[ch]=0;
			}
			return;
		}
		if(next_command==MUSIC_LFO_H){		//ＬＦＯ＿Ｈか
			play_lfo_h[ch]=step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_LFO_M){		//ＬＦＯ＿Ｍか
			play_lfo_m[ch]=step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_LFO_S){		//ＬＦＯ＿Ｓか
			play_lfo_s[ch]=step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_PEDAL){		//ダ?パ`ヘダ?か
			d_pedal[ch]=step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_WAVE){		//エ?ベ?`プか
			wave[ch]=step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_VOL){		//ＶＯＬか
			dataVolume[ch]=step[ch][step_point[ch]++];
//			pDSData[play_use_task[ch]]->SetVolume(dataVolume[ch]);
			goto gate_100;
		}
		if(next_command==MUSIC_DETUNE){		//デチ?`?か
			play_detune[ch] = step[ch][step_point[ch]++];
			goto gate_100;
		}
		if(next_command==MUSIC_FREQ){		//周波?か
			dataFrequency[ch]=step[ch][step_point[ch]++];
//			pDSData[play_use_task[ch]]->SetFrequency((DWORD)dataFrequency[ch]);
			goto gate_100;
		}
		if(next_command==MUSIC_PAN){		//パ?ポットか
			dataPan[ch]=step[ch][step_point[ch]++];
//			pDSData[play_use_task[ch]]->SetPan(dataPan[ch]);
			goto gate_100;
		}
		if(next_command==MUSIC_END){		//K了か
			play_flg[ ch ] = 0;
			return;
		}
	}
}

//void CALLBACK music( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime )
void music( void )
{
	int d7;
#if 1
#if THREAD_ON
	while(!thread_flg){
#if CLASS_UP
		Sleep(1);		//再生?
#endif
#endif
		LARGE_INTEGER d0;
		QueryPerformanceCounter( &d0 );
		if((d_cnt.LowPart + tempo.LowPart) > d0.LowPart)
#if THREAD_ON
		{
			continue;
		}
#else
			return;
#endif
#if !DEBUG_ON
		if(abs(d0.LowPart - d_cnt.LowPart) > 3333333)
#if THREAD_ON
			continue;
#else
			return;
#endif
#endif
		d_cnt.LowPart += tempo.LowPart;
#endif
		if(!pause_flg){
#if VOICE_KILL_ON
			for( d7 = 0 ; d7 < voice_kill_point ; d7++ ){
				voice[ voice_kill_buf[ d7 ] ].name = 0;			//まずタスク?
				pDSData[ voice_kill_buf[ d7 ] ]->Stop();		//キ`オフ
				pDSData[ voice_kill_buf[ d7 ] ]->SetCurrentPosition(0);	//
			}
			voice_kill_point=0;
#endif
			for( d7 = 0 ; d7 < VOICE_MAX ; d7++ ){
				if(voice[ d7 ].name){
					voice_action( d7 );
				}
			}
			for( ch = 0 ; ch < TRACK_MAX ; ch++ ){
				if(play_track_flg[ ch ])
					gate();
			}
		} else {

#if CLASS_UP
			Sleep(8);		//ポ`ズ?
#endif
			QueryPerformanceCounter( &d_cnt );
		}

#if 0		//--------------

#if CLASS_UP
#if MODE98
	if( tempo.LowPart > 10080*2 )		//テ?ポ１５０未?なら
#else
	if( tempo.LowPart > 10080 )		//テ?ポ１５０未?なら
#endif
		Sleep(2);		//再生?
	else
		Sleep(1);		//再生?
#endif

#endif		//--------------

#if THREAD_ON
	}
	thread_flg=2;
	ExitThread( TRUE );
#endif
	d7=0;
	return;
}

/*--------------------------------------------
	Ｔ＿ＭＵＳＩＣ?期化
---------------------------------------------*/
int t_music_init()
{
	if( GetAsyncKeyState( 0x10 ) & 0x8000 ){		//ＳＨＩＦＴを?しながらなら
		dsound_flg = -1;
		return FALSE;
	}


	if(!InitDSound()){
		dsound_flg = -1;
		return FALSE;
	}

	music_init();		//ド?イバ`?期化

#if 0	////////////////////////////////////////////////////

	if(!music_read()){
		play_start();
	} else {
		play_stop();
		dsound_flg = -1;
		return FALSE;
	}
	QueryPerformanceCounter(&d_cnt);

#if THREAD_ON
	static DWORD dwChildThreadId = 0;
	HANDLE hChildThred = CreateThread( NULL,
										16384,
										(LPTHREAD_START_ROUTINE)music,
//										(LPVOID)hWnd,
										NULL,
										NULL,
										&dwChildThreadId );
#if CLASS_UP
	SetThreadPriority(hChildThred,THREAD_PRIORITY_HIGHEST);
//	SetThreadPriority(hChildThred,THREAD_PRIORITY_ABOVE_NORMAL);
#endif

#endif

#endif	/////////////////////////////////////////////////////

//	play_se(240, 320, 240);
	//タイト?ＢＧＭ再生_始
//	play_bgm(1);

	return TRUE;
}

/*--------------------------------------------
	Ｔ＿ＭＵＳＩＣK了
---------------------------------------------*/
void t_music_end()
{
	//ＤＳＯＵＮＤ常なら
	if(dsound_flg == -1)
		return;
#if THREAD_ON
//	thread_flg=1;
//	while(thread_flg!=2);
#endif
	EndApp();
}


#if 0
//??????????　抗粼偕??????????
//static voice_seek_point=0;
int play_se(int tone, int x, int y)
{
	int distance;
	//ＤＳＯＵＮＤ常なら
	if(dsound_flg == -1){
		return -1;
	}

	if(tone > TONE_MAX){		//マックス以上なら
		return -1;
	}

	if(tone_tbl[ tone ].voice_place == -1){		//未定x音色なら
		return -1;
	}

	if(tone_tbl[ tone ].play_time){		//既に再生?なら
		return 0;
	}

	if(voice[ voice_seek_point ].tone_no != tone){		//同じ音色番?でなければ
		voice[ voice_seek_point ].tone_no = tone;		//音色番?保存
		RELEASE(pDSData[voice_seek_point]);		//_放
		//音色コピ`
		if( pDSound->DuplicateSoundBuffer(pDSData_tone[tone_tbl[ tone ].voice_place],&pDSData[voice_seek_point]) != DS_OK ){
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファの}uに失", "Error", MB_OK);
#endif
			return -1;
		}
	}

	if(stereo_flg == T_MUSIC_MONO){		//?ノ??なら
		distance = 127 * tone_tbl[ tone ].voice_volume / 10;
		x = 64;
	} else {		//ステ?オなら
		distance = abs(y - 240);		//?心からのＹ座摔?x
		y = abs(x - 320);		//?心からのＸ座摔?x
		if(distance < y)		//?い方裼
			distance = y;		//

		distance = distance << 4;		//ボ??`?Q
		distance /= 0x5a;				//

		if(distance >= 127)		//焱猡胜
			return 0;			//Qらさない

		x /= 5;			//パ?ポットQ
		if(x < 0)		//
			x = 0;		//
		if(x > 127)		//
			x = 127;	//

		distance = (127 - distance) * tone_tbl[ tone ].voice_volume / 10;		//音色ボ??`?加算
	}

	pDSData[ voice_seek_point ]->SetVolume(volume_tbl[ distance ]);		// ボ??`?セット
	pDSData[ voice_seek_point ]->SetPan(panpot_tbl[ x ]);			// パ?ポットセット
	pDSData[ voice_seek_point ]->Play(0, 0, 0);						//キ`オ?

	voice_seek_point = voice_seek_point++;		//ポイ?タ`更新
	voice_seek_point&=VOICE_MAX-1;				//
	tone_tbl[ tone ].play_time = 8;		//再生??gセット

	return 0;
}

#else

//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓　失∽鳌　
HANDLE hEvent[3];	// イベ?ト?ハ?ド?
HANDLE  hThreadHandle;	// 作られたス?ッドのハ?ド?
DWORD dwThreadID;	// 作られたス?ッドのＩＤ
/*-------------------------------------------
	スト?`ミ?グ?バッファ再生用ス?ッドv?
--------------------------------------------*/
// ス?ッドがg行するv?に渡す造体の形式
struct SoundData {
    LPDIRECTSOUNDBUFFER pDSBuffer;	// スト?`?形式のサウ?ド?バッファ
    HANDLE *phEvent;	// 再生カ`ソ?が通知位置に?たときのイベ?ト?ハ?ド?の配列へのポイ?タ
    DWORD dwEventNum;	// イベ?トの?
    DWORD dwStopEventNum;	// 再生停止?のイベ?トのハ?ド?のイ?デックス
    LPBYTE lpWaveData;	// 再生するサウ?ド?デ`タ
    DWORD dwWaveSize;	// 再生するサウ?ド?デ`タのサイズ
    DWORD dwBlock_size;	// サウ?ド?デ`タの１ブ?ックの大きさ
} sd;

// デ`タのコピ`v?
void Block_Copy(LPDIRECTSOUNDBUFFER lpBuffer,
    DWORD blockadd, DWORD blocksize,
    LPBYTE lpWave, LONG &waveAdd, LONG waveSize)
{
    LPBYTE lpBlockAdd1, lpBlockAdd2;
    DWORD blockSize1, blockSize2;
	LONG ws = waveSize - waveAdd;

    // バッファを?ック
    HRESULT hr = lpBuffer->Lock(blockadd, blocksize, (LPVOID*)&lpBlockAdd1, &blockSize1, (LPVOID*)&lpBlockAdd2, &blockSize2, 0);
    if(hr == DS_OK)
    {
        if(ws < (long)blockSize1)
        {
            CopyMemory(lpBlockAdd1, lpWave + waveAdd, ws);
			waveAdd = blockSize1 - ws;
            CopyMemory(lpBlockAdd1 + ws, lpWave + thread_loop_start, waveAdd);
            waveAdd += thread_loop_start;
        }
        else
        {
            CopyMemory(lpBlockAdd1, lpWave + waveAdd, blockSize1);
            waveAdd += blockSize1;
        }
        if(lpBlockAdd2)
        {
			ws = waveSize - waveAdd;
            if(ws < (long)blockSize2)
            {
                CopyMemory(lpBlockAdd1, lpWave + waveAdd, ws);
				waveAdd = blockSize2 - ws;
                CopyMemory(lpBlockAdd1 + ws, lpWave + thread_loop_start, waveAdd);
	            waveAdd += thread_loop_start;
            }
            else
            {
                CopyMemory(lpBlockAdd2, lpWave + waveAdd, blockSize2);
				waveAdd += blockSize2;
            }
        }
        lpBuffer->Unlock(lpBlockAdd1, blockSize1, lpBlockAdd2, blockSize2);
    }
}

static LONG wave_count = 0;
static LONG wave_address = 0;
static LONG buffer_address = 0;
volatile static int thread_start_flg = 0;

// ス?ッドがg行するv?
DWORD WINAPI MyThreadFunc(LPVOID param)
{
	LONG waveAdd = 0;
    // バッファの最?のブ?ックにデ`タを?き?む．
    Block_Copy(sd.pDSBuffer, 0, sd.dwBlock_size, sd.lpWaveData, waveAdd, sd.dwWaveSize);
	thread_start_flg = 1;
    // デ`タを?き?むo限?`プに入る．
    while(TRUE)
    {
        // DirectSoundBufferからのイベ?トを待つ
        DWORD i = WaitForMultipleObjects(sd.dwEventNum, sd.phEvent, FALSE, INFINITE);
        // イベ?トがきたら，これから再生されるブ?ックの?のブ?ックに?き?む
		wave_count++;
        switch(i)
        {
            case WAIT_OBJECT_0 + 1:
                // １番目のブ?ックに?き?む
				buffer_address = 0; wave_address = waveAdd;
                Block_Copy(sd.pDSBuffer, 0, sd.dwBlock_size, sd.lpWaveData, waveAdd, sd.dwWaveSize);
                break;
            case WAIT_OBJECT_0:
                // ２番目のブ?ックに?き?む
				buffer_address = sd.dwBlock_size; wave_address = waveAdd;
                Block_Copy(sd.pDSBuffer, sd.dwBlock_size, sd.dwBlock_size, sd.lpWaveData, waveAdd, sd.dwWaveSize);
                break;
            case WAIT_OBJECT_0 + 2:
                 // 再生K了?に，ス?ッドをK了する
				exit_thread_flg = 1;
				ExitThread(TRUE);
            default:
                 // 再生K了?に，ス?ッドをK了する
				exit_thread_flg = 1;
				ExitThread(TRUE);
        }
//		InvalidateRect(hwndApp, NULL, TRUE);
    }
	return 0L;
}

//
// セカ?ダ?バッファの取得
//
// 引　?：	pWfmtx ... バッファのフォ`マット
//			DataSize ... バッファのサイズ
//			ppData ... 取得したセカ?ダ?バッファのポイ?タ
// 	TRUE  ... 正常K了
//			FALSE ... 失
static BOOL dwSoundInit2( WAVEFORMATEX *pWfmtx, DWORD DataSize, LPDIRECTSOUNDBUFFER *ppData, HMMIO hmmio )
{
	// スト?`ミ?グ?セカ?ダ??バッファを作る
	WAVEFORMATEX pcmwf = *pWfmtx;
	DSBUFFERDESC  dsbufferdesc;
	HRESULT ret;

	//_放するなら
	if(stream_flg == 1){
		RELEASE(pDSNotify);
		stream_flg = 0;
	}
	//サウ?ドサ`フェ`スを起こす
	memset( &dsbufferdesc , 0  , sizeof( DSBUFFERDESC ) );
	dsbufferdesc.dwSize = sizeof( DSBUFFERDESC );
	dsbufferdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY /*| DSBCAPS_CTRLDEFAULT */| DSBCAPS_GETCURRENTPOSITION2; 
//	dsbufferdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS; 
    dsbufferdesc.dwBufferBytes = DataSize;
    dsbufferdesc.lpwfxFormat   = pWfmtx;
    if ( pDSound->CreateSoundBuffer( &dsbufferdesc , ppData , NULL ) != DS_OK ){
#ifdef _DEBUG
	    MessageBox(hWnd, "セカ?ド?バッファの作成に失", "Error", MB_OK);
#endif
		return FALSE;
	}


	//ＷＡＶＥデ`タ?`ド
	if( WaveDataRead( hmmio, &DataSize, pDSData_stream ) == FALSE  ){
#ifdef _DEBUG
		MessageBox(hWnd, "ＷＡＶファイ?の?`ドに失", "Error", MB_OK);
#endif
		return FALSE;
	}

	// スト?`ミ?グ?セカ?ダ??バッファを作る
//	WAVEFORMATEX pcmwf = *pWaveHeader;
	DSBUFFERDESC dsbdesc; 
	// DSBUFFERDESC造体をO定
	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
#if ACTIVE
	dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY /*| DSBCAPS_CTRLDEFAULT */| DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS; 
#else
	dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY /*| DSBCAPS_CTRLDEFAULT */| DSBCAPS_GETCURRENTPOSITION2; 
#endif
	// 上でO定したWaveフォ`マットを?つ２秒のバッファを作る
	dsbdesc.dwBufferBytes = 2 * pcmwf.nAvgBytesPerSec; 
	dsbdesc.lpwfxFormat = &pcmwf; 
	ret = pDSound->CreateSoundBuffer(&dsbdesc, &pDSData_tone[ TONE_MAX ], NULL); 
	if(ret != DS_OK) { 
	    // バッファの作成に失
//		RELEASE(pDSound);
		RELEASE(pDSData_stream);
#ifdef _DEBUG
	    MessageBox(hWnd, "セカ?ド?バッファpDSData_streamの作成に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	// DirectSoundNotifyオブジェクトを作成する
	ret = pDSData_tone[ TONE_MAX ]->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify);
	if(ret != DS_OK)
	{
//		RELEASE(pDSound);
		RELEASE(pDSData_stream);
		RELEASE(pDSData_tone[ TONE_MAX ]);
#ifdef _DEBUG
	    MessageBox(hWnd, "DirectSoundNotifyの作成に失", "Error", MB_OK);
#endif
		return FALSE;
		//　作成に失
	}

	// イベ?ト?オブジェクトの作成
	hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[2] = CreateEvent(NULL, FALSE, FALSE, NULL);

	// DirectSoundNotifyオブジェクトをO定する
	// 造体を?期化
	DSBPOSITIONNOTIFY  pn[3];
	pn[0].dwOffset = 0;	// ０秒目
	pn[0].hEventNotify = hEvent[0];
	pn[1].dwOffset = 1 * pcmwf.nAvgBytesPerSec;	// １秒目
	pn[1].hEventNotify = hEvent[1];
	pn[2].dwOffset = DSBPN_OFFSETSTOP;	// 再生停止
	pn[2].hEventNotify = hEvent[2];	// ２つ目のイベ?ト?ハ?ド?を停止?のハ?ド?として使う

	// 停止位置O定
	HRESULT hr;
	hr = pDSNotify->SetNotificationPositions(3, pn);
	if(hr != DS_OK)
	{
	    // O定失
		RELEASE(pDSNotify);
//		RELEASE(pDSound);
		RELEASE(pDSData_stream);
		RELEASE(pDSData_tone[ TONE_MAX ]);
#ifdef _DEBUG
	    MessageBox(hWnd, "DirectSoundNotifyのO定に失", "Error", MB_OK);
#endif
		return FALSE;
	}

	stream_flg = 1;		//作成フ?グセット

	// ス?ッドスタ`トフ?グク?ア
	thread_start_flg = 0;

	// 再生用ス?ッドを作る
	sd.pDSBuffer = pDSData_tone[ TONE_MAX ];	// スト?`ミ?グ再生するDirectSoundBufferオブジェクト
	sd.phEvent = hEvent;	// 4.4.で作ったイベ?ト?ハ?ド?の配列
	sd.dwEventNum = 3;	// イベ?ト?ハ?ド?は３つ
	sd.dwStopEventNum = 2;	// ３つ目のイベ?ト?ハ?ド?が停止用イベ?ト
	sd.lpWaveData = pDSbuffData;	// 再生するWaveデ`タのポイ?タをO定；
	sd.dwWaveSize = DataSize;	// 再生するWaveデ`タの大きさをO定；
	sd.dwBlock_size = pcmwf.nAvgBytesPerSec;	// 44.1kHz，１６ビット?ステ?オ

	// ス?ッドを作るコ`ド
	hThreadHandle = CreateThread(NULL, 0, MyThreadFunc, &sd, 0, &dwThreadID);
	if(hThreadHandle == NULL)
	{
#ifdef _DEBUG
	    // ス?ッドの生成に失
#ifdef _DEBUG
	    MessageBox(hWnd, "ス?ッドの生成に失・筏蓼筏俊?急オフトまでB?下さい。", "Error", MB_OK);
#endif
#endif
		return FALSE;
	}

	// ス?ッドスタ`トまで待つ
	while(1){
		if(thread_start_flg)
			break;
	}

//	SetThreadPriority(hThreadHandle,THREAD_PRIORITY_HIGHEST);
//	SetThreadPriority(hThreadHandle,THREAD_PRIORITY_ABOVE_NORMAL);
//	pDSData_tone[ TONE_MAX ]->Play(0, 0, DSBPLAY_LOOPING);
	return TRUE;
}
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑　失∽鳌　

int t_music_bgm_no = -1;
char t_music_bgm_pitch[16]={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

//??????????　ＢＧＭ再生　??????????
#define	BGM_AUTO_LOOP	1
T_MUSIC_BGM bgm_tbl[] = {
#if 0		//ＳＣ版
	{"data\\bgm\\sabgm_s0.wav",		110,0,0},			//00	サクセス
	{"data\\scheme\\title.wav",		120,0,0},			//01	タイト?
	{"data\\scheme\\select.wav",	120,2,0},			//02	キ??xk（先^）
	{"data\\scheme\\tomorrow.wav",	120,2,0},			//03	町１（途?）
	{"data\\scheme\\tomorrow.wav",	120,2,0},			//04	ノノ
	{"data\\scheme\\battle1.wav",	120,2,0},			//05	殛L１（途?）
//	{"data\\scheme\\battle2.wav",	120,2,0},			//06	殛L２（先^）
	{"data\\scheme\\mukasi.wav",	120,2,0},			//06	殛L２（途?）
	{"data\\scheme\\perpetual.wav",	120,2,0},			//07	洞窟１（途?）
	{"data\\scheme\\haha.wav",		120,2,0},			//08	洞窟２（先^）
	{"data\\scheme\\perpetual.wav",	120,2,0},			//09	洞窟１（途?）
	{"data\\scheme\\just.wav",		120,2,0},			//10	フィ`?ド（途?）
	{"data\\scheme\\just.wav",		120,2,0},			//11	ノノ
#else
#if 0		//β版
	{"data\\bgm\\sabgm_s0.wav",	110,0,0},			//00	サクセス
	{"data\\bgm\\sabgm_s1.wav",	120,0,0},			//01	タイト?
//	{"data\\bgm\\sabgm_s2.wav",	124,2,0},			//02	キ??xk（途?）
	{"data\\bgm\\sabgm_s3.wav",	115,1,0},			//02	キ??xk２（先^）
	{"data\\bgm\\sabgm_t1.wav",	127,2,0},			//03	シ?ウ町２（途?）
	{"data\\bgm\\sabgm_t0.wav",	95,2,0},			//04	町１（途?）
	{"data\\bgm\\sabgm_b0.wav",	122,1,0},			//05	シ?ウ殛L１（先^）
	{"data\\bgm\\sabgm_b1.wav",	122,1,0},			//06	シ?ウ殛L１（先^）
	{"data\\bgm\\sabgm_d0.wav",	120,1,0},			//07	?ミサ洞窟（先^）
	{"data\\bgm\\sabgm_d1.wav",	120,1,0},			//08	?ミサ洞窟（先^）
	{"data\\bgm\\sabgm_d2.wav",	120,1,0},			//09	?ミサ洞窟（先^）
	{"data\\bgm\\sabgm_f0.wav",	127,2,0},			//10	シ?ウフィ`?ド１（途?）
	{"data\\bgm\\sabgm_f1.wav",	127,2,0},			//11	シ?ウフィ`?ド１（途?）
#else		//マスタ`版
	{"data\\bgm\\sabgm_s0.wav",	110,0,0},			//00	サクセス
	{"data\\bgm\\sabgm_s1.wav",	120,0,0},			//01	タイト?
	{"data\\bgm\\sabgm_s1.wav",	105,2,0},			//02	キ??xk（途?）
	{"data\\bgm\\sabgm_t1.wav",	127,2,0},			//03	シ?ウ町２（途?）
	{"data\\bgm\\sabgm_t0.wav",	95,2,0},			//04	町１（途?）

	{"data\\bgm\\sabgm_b0.wav",	122,1,0},			//05	シ?ウ殛L１（先^）
	{"data\\bgm\\sabgm_b1.wav",	122,2,0},			//06	シ?ウ殛L１（途?）

	{"data\\bgm\\sabgm_d0.wav",	120,1,0},			//07	?ミサ洞窟（先^）
	{"data\\bgm\\sabgm_d1.wav",	105,1,0},			//08	枚纯撸保ㄏ阮^）
	{"data\\bgm\\sabgm_d2.wav",	100,2,0},			//09	枚纯撸玻ㄍ?）

	{"data\\bgm\\sabgm_f0.wav",	127,2,0},			//10	サイナス（途?）
	{"data\\bgm\\sabgm_f1.wav",	100,2,0},			//11	ジ??ガ（途?）
#endif
#endif
};

int play_bgm(int bgm_no)
{
//	_asm{int 3}
	HMMIO hmmio;			//ファイ?ハ?ド?
    WAVEFORMATEX	Wfmtx;			//WAVEファイ?のフォ`マットデ`タ
	DWORD			DataSize;		//バッファ`のサイズ
	DWORD			WaveSize;		//Waveファイ?のサイズ

//	bgm_no = 0;

	//フェ`ドフ?グク?ア
	t_music_fade_flg = 0;

	if(dsound_flg == -1)
		return -1;

	if(bgm_no > 12)
		return -1;

	//最?の一回目でなければ
	if(t_music_bgm_no != -1){
		//ス?ッド停止へ
		stop_thread();
	}

	//ＷＡＶファイ?のオ`プ?
//	if( (hmmio = mmioOpen(fname[bgm_no], NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL){
	if( (hmmio = mmioOpen(bgm_tbl[bgm_no].fname, NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL){
#ifdef _DEBUG
//		MessageBox(hWnd, "ＷＡＶファイ?のオ`プ?に失", "Error", MB_OK);
#endif
		return FALSE;
	}
	//アト?ビ?`トの?`ド
	if( WaveFormatRead( hmmio , &Wfmtx , &WaveSize ) == FALSE ){
#ifdef _DEBUG
		MessageBox(hWnd, "アト?ビ?`トの?`ドに失", "Error", MB_OK);
#endif
		return FALSE;
	}
	DataSize = WaveSize;
	//サウ?ドサ`フェ`スを起こす
	if(bgm_tbl[bgm_no].loop_flg == 2){		//途??`プなら
		thread_stop_flg = 1;		//フ?グオ?
		//ＢＧＭ情螗互氓
#if BGM_AUTO_LOOP
		FILE *fp;
		int d0,d1,d2;
		fp = fopen( bgm_tbl[bgm_no].fname, "rb" );
		if( fp == NULL ){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?のオ`プ?に失", "Error", MB_OK);
#endif
			return FALSE;
		}
		d2 = 8;							//’ＲＩＦＦ’８バイトスキップ
		d2 += 4;						//’ＷＡＶＥ’４バイトスキップ
		d2 += 4;						//’???　’４バイトスキップ
		d2 += 16;						//１６バイトスキップ
		fseek(fp,d2,SEEK_CUR);			//ファイ?ポイ?タ`移
		d0 = fgetc(fp);					//波形デ`タの最小ブ?ック取り出し
		d0 |= fgetc(fp) << 8;			//
		d2 = 2;							//２バイトスキップ
		d2 += 12+DataSize;				//’ＤＡＴＡ’チ??ク　＋　デ`タサイズスキップ
		if(DataSize&1){					//サイズが奇?なら
			d2++;						//１バイト加算
		}
		d2 += 12*4;						//残り４８バイトスキップ
		fseek(fp,d2,SEEK_CUR);			//ファイ?ポイ?タ`移

		//?`プ_始位置
		d1 = fgetc(fp);					//?`プ_始ポイ?ト取り出し
		d1 |= fgetc(fp) << 8;			//
		d1 |= fgetc(fp) << 16;			//
		d1 |= fgetc(fp) << 24;			//
		thread_loop_start = d1 * d0;	//?`プ位置セット
		//?`プK了位置
		d1 = fgetc(fp);
		d1 |= fgetc(fp) << 8;
		d1 |= fgetc(fp) << 16;
		d1 |= fgetc(fp) << 24;
//		thread_loop_end = d1 * d0;

		fclose(fp);
#else
		thread_loop_start = bgm_tbl[bgm_no].loop_point << 1;
#endif
//		thread_loop_start = bgm_tbl[bgm_no].loop_start;
//		thread_loop_end = bgm_tbl[bgm_no].loop_end;
//		if( dwSoundInit2( &Wfmtx, DataSize, &pDSData_tone[TONE_MAX], hmmio ) == FALSE ){
		if( dwSoundInit2( &Wfmtx, DataSize, &pDSData_stream, hmmio ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "dwSoundInit2セカ?ダ??バッファの作成に失", "Error", MB_OK);
#endif
			return FALSE;
		}
#if BGM_AUTO_LOOP
		sd.dwWaveSize = d1 * d0;		//?`プK了位置セット（もしかしたらいらんかも）
#endif
	} else {
		thread_stop_flg = 0;		//フ?グオフ
		if( dwSoundInit( &Wfmtx, DataSize, &pDSData_tone[TONE_MAX] ) == FALSE ){
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファの作成に失", "Error", MB_OK);
#endif
			return FALSE;
		}
		//デ`タの?`ド
		if( WaveDataRead( hmmio, &DataSize, pDSData_tone[TONE_MAX] ) == FALSE  ){
#ifdef _DEBUG
			MessageBox(hWnd, "ＷＡＶファイ?の?`ドに失", "Error", MB_OK);
#endif
			return FALSE;
		}
	}
	mmioClose( hmmio , 0 );

	//ピッチセット
	pDSData_tone[ TONE_MAX ]->SetFrequency((DWORD)freq_tbl[36+1+t_music_bgm_pitch[bgm_no]]);
	//音量セット
	pDSData_tone[ TONE_MAX ]->SetVolume(volume_tbl[ bgm_tbl[bgm_no].volume * t_music_bgm_volume / 15 ]);		// ボ??`?セット

	//ＢＧＭ再生_始
	if(bgm_tbl[bgm_no].loop_flg){		//?`プなら
		pDSData_tone[ TONE_MAX ]->Play(0, 0, DSBPLAY_LOOPING);
	} else {
		pDSData_tone[ TONE_MAX ]->Play(0, 0, 0);
	}

	//再生?ＢＧＭ番?保存
	t_music_bgm_no = bgm_no;

	// F在の?gを
	NowTime = GetTickCount();

	//再生?フ?グセット
	t_music_playing_flg = 1;

	//再生?ＢＧＭ番?保存
//	map_bgm_no = t_music_bgm_no;

	return TRUE;
}

//??????????　ＢＧＭ音量セット　??????????
void bgm_volume_change(void)
{
	if(dsound_flg == -1)
		return;

	if(t_music_bgm_volume > 15){		//焱猡胜
		t_music_bgm_volume = 15;
	}
	if(t_music_bgm_volume < 0){		//焱猡胜
		t_music_bgm_volume = 0;
	}
	//音量セット
	pDSData_tone[ TONE_MAX ]->SetVolume(volume_tbl[ bgm_tbl[t_music_bgm_no].volume * t_music_bgm_volume / 15 ]);		// ボ??`?セット
}

//??????????　ＢＧＭピッチセット　??????????
void set_gbm_pitch(void)
{
	if(dsound_flg == -1)
		return;

	//ピッチセット
	pDSData_tone[ TONE_MAX ]->SetFrequency((DWORD)freq_tbl[36+1+t_music_bgm_pitch[t_music_bgm_no]]);
}

int t_music_se_no = -1;
//??????????　抗粼偕??????????
//static voice_seek_point=0;
int play_se(int tone, int x, int y)
{
	int total_level = 127;
	t_music_se_no = tone;

	if(dsound_flg == -1)
		return -1;

	if(tone > TONE_MAX){		//マックス以上なら
		return -1;
	}

	if(tone_tbl[ tone ].voice_place == -1){		//未定x音色なら
		return -1;
	}

	if(tone_tbl[ tone ].play_time){		//既に再生?なら
		return 0;
	}

	if(voice[ voice_seek_point ].tone_no != tone){		//同じ音色番?でなければ
		voice[ voice_seek_point ].tone_no = tone;		//音色番?保存
		RELEASE(pDSData[voice_seek_point]);		//_放
		//音色コピ`
		if( pDSound->DuplicateSoundBuffer(pDSData_tone[tone_tbl[ tone ].voice_place],&pDSData[voice_seek_point]) != DS_OK ){
#ifdef _DEBUG
			MessageBox(hWnd, "セカ?ダ??バッファの}uに失", "Error", MB_OK);
#endif
			return -1;
		}
	} else {
		pDSData[voice_seek_point]->SetCurrentPosition(0);	//再生カ`ソ??セット
	}
#if 0
	total_level = abs(y - 240);		//?心からのＹ座摔?x
	y = abs(x - 320) >> 1;		//?心からのＸ座摔?x
	if(total_level < y)		//?い方裼
		total_level = y;		//

	total_level = total_level << 4;		//ボ??`?Q
	total_level /= 0x5a;				//

	if(total_level >= 127)		//焱猡胜
		return 0;			//Qらさない

	total_level = 127 - total_level;				//

#endif

	if(stereo_flg == T_MUSIC_MONO){		//?ノ??なら
		x = 64;
	} else {
		x /= 5;			//パ?ポットQ
		if(x < 0)		//
			x = 0;		//
		if(x > 127)		//
			x = 127;	//
	}

	//音色ボ??`?
	total_level = 127 * tone_tbl[ tone ].voice_volume / 127;
//	total_level = total_level * tone_tbl[ tone ].voice_volume / 127;

	//マスタ`ボ??`?
	total_level = total_level * t_music_se_volume / 15;

	pDSData[ voice_seek_point ]->SetVolume(volume_tbl[ total_level ]);		// ボ??`?セット
	pDSData[ voice_seek_point ]->SetPan(panpot_tbl[ x ]);			// パ?ポットセット
	if(tone_tbl[ tone ].voice_note){		//ノ`ト番?指定なら
		pDSData[ voice_seek_point ]->SetFrequency((DWORD)freq_tbl[tone_tbl[ tone ].voice_note + tone_tbl[ tone ].voice_rate]);
	}

#if 0
	if(tone_tbl[ tone ].voice_loop == 0 ){		//?`プoしなら
		pDSData[ voice_seek_point ]->Play(0, 0, 0);			//キ`オ?
	} else {
		voice[ voice_seek_point ].loop_flg = 1;		//?`プ再生
		pDSData[ voice_seek_point ]->Play(0, 0, DSBPLAY_LOOPING);		//キ`オ?
	}
#else
	pDSData[ voice_seek_point ]->Play(0, 0, 0);			//キ`オ?
#endif
	while(1){
		voice_seek_point++;		//ポイ?タ`更新
		voice_seek_point&=VOICE_MAX-1;				//
		if(voice[ voice_seek_point ].loop_flg == 0){		//?`プ再生?でなければ
			break;
		}
	}
	tone_tbl[ tone ].play_time = 5;		//再生??gセット

	return 0;
}
#endif

//??????????　h境音再生　??????????
//int play_environment_tone(int tone, int distance, int voice_address, int panpot, int new_flg)
int play_environment_tone(int tbl_no, int new_flg)
{
//	int total_level;

//	t_music_se_no = tone;

	if(dsound_flg == -1)
		return -1;

	if(env_tbl[tbl_no].tone > TONE_MAX){		//マックス以上なら
		return -1;
	}

	if(tone_tbl[ env_tbl[tbl_no].tone ].voice_place == -1){		//未定x音色なら
		return -1;
	}

//	if(tone_tbl[ env_tbl[tbl_no].tone ].play_time){		//既に再生?なら
//		return 0;
//	}

	env_tbl[tbl_no].distance = env_tbl[tbl_no].distance << 4;		//ボ??`?Q
	env_tbl[tbl_no].distance /= 0x6a;				//

//	if(env_tbl[tbl_no].distance >= 127)				//焱猡胜
//		return 0;					//Qらさない

	if(env_tbl[tbl_no].distance >= 127)				//焱猡胜
		env_tbl[tbl_no].distance = 127;					//Qらさない

	if(new_flg){		//新胜
		if(voice[ env_tbl[tbl_no].voice_address ].tone_no != env_tbl[tbl_no].tone){		//同じ音色番?でなければ
			voice[ env_tbl[tbl_no].voice_address ].tone_no = env_tbl[tbl_no].tone;		//音色番?保存
			RELEASE(pDSData[env_tbl[tbl_no].voice_address]);		//_放
			//音色コピ`
			if( pDSound->DuplicateSoundBuffer(pDSData_tone[tone_tbl[ env_tbl[tbl_no].tone ].voice_place],&pDSData[env_tbl[tbl_no].voice_address]) != DS_OK ){
#ifdef _DEBUG
				MessageBox(hWnd, "セカ?ダ??バッファの}uに失", "Error", MB_OK);
#endif
				return -1;
			}
		}
	}

	//音色ボ??`?
//	total_level = (127 - env_tbl[tbl_no].volume) * tone_tbl[ env_tbl[tbl_no].tone ].voice_volume / 127;

	//マスタ`ボ??`?
//	total_level = total_level * t_music_se_volume / 15;

	//ボ??`?が浃铯盲郡胜
	if(env_tbl[tbl_no].volume != env_tbl[tbl_no].volume_old){
		// ボ??`?セット
		pDSData[ env_tbl[tbl_no].voice_address ]->SetVolume(volume_tbl[ env_tbl[tbl_no].volume ]);
		env_tbl[tbl_no].volume_old = env_tbl[tbl_no].volume;
	}

	//パ?ポットが浃铯盲郡胜
	if(env_tbl[tbl_no].panpot != env_tbl[tbl_no].panpot_old){
		// パ?ポットセット
		pDSData[ env_tbl[tbl_no].voice_address ]->SetPan(panpot_tbl[ env_tbl[tbl_no].panpot ]);
		env_tbl[tbl_no].panpot_old = env_tbl[tbl_no].panpot;
	}

	//ノ`ト番?指定なら
	if(tone_tbl[ env_tbl[tbl_no].tone ].voice_note){
		pDSData[ env_tbl[tbl_no].voice_address ]->SetFrequency((DWORD)freq_tbl[tone_tbl[ env_tbl[tbl_no].tone ].voice_note + tone_tbl[ env_tbl[tbl_no].tone ].voice_rate]);
	}

	if(new_flg){		//新胜
		pDSData[ env_tbl[tbl_no].voice_address ]->SetCurrentPosition(0);	//
		if(tone_tbl[ env_tbl[tbl_no].tone ].voice_loop == 0 ){		//?`プoしなら
			pDSData[ env_tbl[tbl_no].voice_address ]->Play(0, 0, 0);			//キ`オ?
		} else {
			voice[ env_tbl[tbl_no].voice_address ].loop_flg = 1;		//?`プ再生
			pDSData[ env_tbl[tbl_no].voice_address ]->Play(0, 0, DSBPLAY_LOOPING);		//キ`オ?
		}

		while(1){
			voice_seek_point++;		//ポイ?タ`更新
			voice_seek_point&=VOICE_MAX-1;				//
			if(voice[ voice_seek_point ].loop_flg == 0){		//?`プ再生?でなければ
				break;
			}
		}
	}
//	tone_tbl[ env_tbl[tbl_no].tone ].play_time = 8;		//再生??gセット

	return 0;
}

//??????????　マップＢＧＭチェック　??????????
void play_map_bgm_check(void)
{
	switch(map_bgm_vct_no){
	case 0:		//ＢＧＭ涓隶Д氓
		if(draw_map_bgm_flg){
			if(t_music_bgm_no != map_bgm_no){		//ＢＧＭ涓胜
				map_bgm_vct_no++;		//フェ`ドアウト_始へ
			}
		}
		draw_map_bgm_flg = 0;
		break;

	case 1:		//ＢＧＭフェ`ドアウト_始
		fade_out_bgm();
		map_bgm_vct_no++;		//フェ`ドアウトK了待ちへ
		break;

	case 2:		//ＢＧＭフェ`ドアウトK了待ち
		if(t_music_fade_flg == 0){		//フェ`ドアウトK了なら
			map_bgm_vct_no++;		//ＢＧＭ再生へ
		}
		break;

	case 3:		//ＢＧＭ再生
		play_bgm(map_bgm_no);		//ＢＧＭ再生
		map_bgm_vct_no = 0;		//ＢＧＭ涓隶Д氓
		break;
	}
}
//??????????　マップＢＧＭ再生　??????????
int play_map_bgm(int tone)
{
	switch(tone){
	//フィ`?ド
	case 40:	//町（大）
		map_bgm_no = 4;
		break;
	case 41:	//町（小）
		map_bgm_no = 3;
		break;
	case 42:	//洞窟１
		map_bgm_no = 7;
		break;
	case 43:	//洞窟２
		map_bgm_no = 8;
		break;
	case 44:	//洞窟３
		map_bgm_no = 9;
		break;
	case 45:	//フィ`?ド
		map_bgm_no = 10;
		break;
	case 46:	//フィ`?ド（ジ??グ?）
		map_bgm_no = 11;
		break;
	}
	return 0;
}

//??????????　h境音再生　??????????
int play_environment(int tone, int x, int y)
{
	int d0,d1,distance,volume;
	d0 = d1 = 0;
	float dx, dy;

//	tone = 157;

	if(tone < 20 || tone > 37){		//浃?胜
		return 0;					//Qらさない
	}

	switch(tone){
	case 28:		//すずめ
		if(SaTimeZoneNo != LS_MORNING){		//朝でなければ
			return 0;						//Qらさない
		}
		break;
	case 29:		//?
		if(SaTimeZoneNo != LS_NIGHT){		//夜でなければ
			return 0;					//Qらさない
		}
		break;
	case 37:		//森のB
		if(SaTimeZoneNo == LS_MORNING){		//朝なら
			return 0;						//Qらさない
		}
		if(SaTimeZoneNo == LS_NIGHT){		//夜なら
			tone = 29;					//?に涓
			break;
		}
		break;
	}






	dx = (float)(x - 320);
	dy = (float)(y - 240);

	distance = (int)sqrt( (double)(dx*dx+dy*dy) );

	volume = distance << 4;		//ボ??`?Q
	volume /= 0x6a;				//

	if(volume >= 127)				//焱猡胜
		return 0;					//Qらさない

	d0 = tone - 20;
	tone = 151 + d0;
	if(env_tbl[d0].count == 0){		//新胜
		//?x代入
		env_tbl[d0].distance = distance;
		volume = (127 - volume) * tone_tbl[ tone ].voice_volume / 127;
		env_tbl[d0].volume = volume * t_music_se_volume / 15;
		d1 = 1;		//裼氓?グセット
		if(env_tbl[d0].voice_address == -1){		//新胜
			env_tbl[d0].tone = tone;
		}
	} else {
		//近い方の?x代入
		if(env_tbl[d0].distance > distance){
			env_tbl[d0].distance = distance;
			volume = (127 - volume) * tone_tbl[ tone ].voice_volume / 127;
			env_tbl[d0].volume = volume * t_music_se_volume / 15;
			d1 = 1;		//裼氓?グセット
		}
	}
	//裼盲胜
	if(d1){
		env_tbl[d0].side = x;
	}
	env_tbl[d0].count++;
	return 0;
}

//??????????　h境音チェック　??????????
void play_environment_check(void)
{
	int d0,d1,d3,d7;
	for(d7=0; d7<MAX_ENV; d7++){
		if(env_tbl[d7].count == 0){		//音oしなら
			if(env_tbl[d7].voice_address != -1){		//消すなら
				pDSData[ env_tbl[d7].voice_address ]->Stop();		//キ`オフ
				voice[ env_tbl[d7].voice_address ].loop_flg = 0;		//?`プ再生K了
				env_tbl[d7].voice_address = -1;
			}
			continue;
		}
		if(env_tbl[d7].voice_address == -1){		//新胜
			env_tbl[d7].voice_address = voice_seek_point;
			env_tbl[d7].volume_old = -1;
			env_tbl[d7].panpot_old = -1;
			d0 = 1;
		} else {
			d0 = 0;
		}

		if(stereo_flg == T_MUSIC_MONO){		//?ノ??なら
			env_tbl[d7].panpot = d1 = 64;
		} else {
			d1 = env_tbl[d7].side / 5;			//パ?ポットQ
			if(d1 < 0)			//
				d1 = 0;			//
			if(d1 > 127)		//
				d1 = 127;		//
		}

		if(d0){		//新胜
			env_tbl[d7].panpot = d1;
		} else {
			d3 = d1 - env_tbl[d7].panpot;
			if(d3 > 2)
				d3 = 2;
			if(d3 < -2)
				d3 = -2;
			//パ?ポットO定
			env_tbl[d7].panpot += d3;
		}
//		play_environment_tone(env_tbl[d7].tone, env_tbl[d7].distance, env_tbl[d7].voice_address, env_tbl[d7].panpot, d0);
		play_environment_tone(d7, d0);

		env_tbl[d7].count = 0;		//カウ?タ`ク?ア
	}
}

//??????????　ＢＧＭフェ`ドチェック　??????????
void bgm_fade_check(void)
{
	//再生?でなければ
	if(t_music_playing_flg == 0){
		t_music_fade_flg = 0;
		return;
	}
	//フェ`ドアウトでなければ
	if(!t_music_fade_flg){
		return;
	}
	//フェ`ドアウトK了なら
	if(!t_music_fade_vol){
		stop_bgm();
		t_music_bgm_no = -1;
	} else {
		// ボ??`?下げる
		pDSData_tone[ TONE_MAX ]->SetVolume(volume_tbl[ --t_music_fade_vol ]);
	}
}

//static int test_h = 320;
//static int test_v = 140;

//??????????　抗籁隶Д氓??????????
void check_se_loop(void){
	int d7;

	if(dsound_flg == -1)
		return;
#if 0
	if(joy_con[0]&JOY_UP)
		test_v-=4;
	if(joy_con[0]&JOY_DOWN)
		test_v+=4;
	if(joy_con[0]&JOY_RIGHT)
		test_h+=4;
	if(joy_con[0]&JOY_LEFT)
		test_h-=4;
	play_environment(1, test_h, test_v);
#endif

	for(d7=0; d7<TONE_MAX ;d7++){
		if(tone_tbl[ d7 ].play_time){		//再生?g更新
			tone_tbl[ d7 ].play_time--;
		}
	}

	//h境音チェック
	play_environment_check();

	//マップＢＧＭチェック
	play_map_bgm_check();

	//ＢＧＭフェ`ドアウトチェックへ
	bgm_fade_check();
}

















int cdda_no = -1;
int stereo_flg = T_MUSIC_STEREO;
int t_music_se_volume = 15;
int t_music_bgm_volume = 15;
static MCI_OPEN_PARMS open  = {0};
static MCIERROR       dwRes = {0};
static int cdda_flg;
static int cdda_check_cnt = -1;
/////////////////////////////////////////////////////////////////////
//  CD-DAト?ック nを?奏する
//     成功すれば trueを返す。
//     失・工欷 ?ッセ`ジボックスを出して falseを返す。
bool cdda_open(int n)
{
	cdda_flg = 0;
// ト?ック?は 2以上で?るはずだ
    _ASSERT( n > 1);

//    MCI_OPEN_PARMS open  = {0};
//  MCIERROR       dwRes = {0};

// デバイスを_く
    open.lpstrDeviceType  = "cdaudio";
    dwRes = mciSendCommand( 0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)&open);
    if ( dwRes)
    {
#ifdef _DEBUG
//		MessageBox(hWnd, "ＣＤ－ＤＡのオ`プ?に失", "_J", MB_OK);
#endif
		cdda_flg = 1;
        return FALSE;
    }

// ?gフォ`マットをト?ック指定にする
    MCI_SET_PARMS set;
    set.dwTimeFormat = MCI_FORMAT_TMSF;
    dwRes = mciSendCommand( open.wDeviceID, MCI_SET, 
                            MCI_SET_TIME_FORMAT, (DWORD)&set); 
    if ( dwRes)
    {
#ifdef _DEBUG
//		MessageBox(hWnd, "ＣＤ－ＤＡのオ`プ?に失", "_J", MB_OK);
#endif
		cdda_flg = 1;
        return FALSE;
    }
	return TRUE;
}
/*--------------------------------------------
			ＣＤ＿ＤＡスタ`ト
---------------------------------------------*/
bool cdda_start(int n)
{
// 再生する
    // ト?ック nを?奏
    MCI_PLAY_PARMS play;
    play.dwFrom = MCI_MAKE_TMSF( n,0,0,0);
    play.dwTo   = MCI_MAKE_TMSF( n+1,0,0,0);
    dwRes = mciSendCommand( open.wDeviceID, 
                            MCI_PLAY, MCI_FROM | MCI_TO,
                            (DWORD)&play);
    if ( dwRes)
    {
#ifdef _DEBUG
//		MessageBox(hWnd, "ゲ`?ＣＤをセットして下さい", "_J", MB_OK);
#endif
		cdda_flg = 2;		//ＣＤＤＡスタ`ト失
        return FALSE;
    }
	return TRUE;
}
/*--------------------------------------------
			ＣＤ＿ＤＡ停止
---------------------------------------------*/
bool cdda_stop(void)
{
	if(cdda_flg == 1)		//ＣＤ－ＲＯＭないなら
		return TRUE;

	mciSendCommand(open.wDeviceID, MCI_STOP, 0, NULL);

// デバイスを]じる
    dwRes = mciSendCommand( open.wDeviceID, MCI_CLOSE, 0, (DWORD)NULL);
    if ( dwRes)
    {
#ifdef _DEBUG
//	    MessageBox(hWnd, "ゲ`?ＣＤをセットして下さい", "_J", MB_OK);
#endif
        return FALSE;
    }
	return TRUE;
}
/*--------------------------------------------
			ＣＤ＿ＤＡ再生
---------------------------------------------*/
extern	double NowTime;

bool cdda_play(int n)
{
	check_se_loop();		//抗籁隶Д氓

		cdda_flg = 1;		//ＣＤ－ＲＯＭないなら
		return TRUE;



	if(cdda_flg == 1)		//ＣＤ－ＲＯＭないなら
		return TRUE;

	if(cdda_no != n ){		//曲が浃铯盲郡
		cdda_flg = 0;		//フ?グク?ア
		cdda_check_cnt = -1;
	}

	if(cdda_flg)		//エ?`?なら
		return TRUE;

	cdda_check_cnt++;
	cdda_check_cnt&=63;
	if(cdda_check_cnt)
		return TRUE;

	if(cdda_no == n ){		//再生?なら
		MCI_STATUS_PARMS mcisp;	mcisp.dwItem = MCI_STATUS_MODE;
		if(mciSendCommand(open.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcisp)){		//常なら
#ifdef _DEBUG
//		    MessageBox(hWnd, "ゲ`?ＣＤをセットして下さい", "_J", MB_OK);
#endif
			cdda_flg = 3;		//ＣＤＤＡステ`タス常
			return FALSE;
		}
		if(mcisp.dwReturn == MCI_MODE_PLAY){		//再生?なら
			return TRUE;
		} else {		//止まったなら
//			cdda_start( n );
			cdda_no = Rnd( 11, 34 );
			cdda_start( cdda_no );
			// ?gのWれ忘れさせる
			NowTime = GetTickCount();
			return TRUE;
		}
	} else {
		cdda_no = n;		//再生する
		cdda_stop();
		cdda_open( n );
		cdda_start( n );
		// ?gのWれ忘れさせる
		NowTime = GetTickCount();
	}
	return TRUE;
}

