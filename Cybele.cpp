// Cybele.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "Cybele.h"
#include "CybeleTrayWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCybeleApp

BEGIN_MESSAGE_MAP(CCybeleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCybeleApp �R���X�g���N�V����

CCybeleApp::CCybeleApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CCybeleApp �I�u�W�F�N�g�ł��B

CCybeleApp theApp;


// CCybeleApp ������

BOOL CCybeleApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("Cybele Application Server. Birdland.Co.Ltd All rights reserved."));

	CCybeleTrayWnd* pWnd = new CCybeleTrayWnd();
	pWnd->Create();
	m_pMainWnd = pWnd;

	return TRUE;

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
//	return FALSE;
}
