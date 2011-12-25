// QueryByMouse.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CQueryByMouseApp:
// このクラスの実装については、QueryByMouse.cpp を参照してください。
//

class CQueryByMouseApp : public CWinApp
{
public:
	CQueryByMouseApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CQueryByMouseApp theApp;