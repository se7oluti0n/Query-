/******************************************************
DecoderTables Ver.1.1.0 JPEG�����N���X�p�e�[�u��
	Copyright 2004 AsWe.Co.Ltd. All rights reserved.
The comments are written in Japanese (Shift-JIS).
******************************************************/
#pragma once

enum
eAnalyzedResult {		// �w�b�_��͌���
	emYet			= 0,			// �����
	emIsHTable		= 0x01,			// �n�t�}���e�[�u���L��
	emIsQTable		= 0x02,			// �ʎq���e�[�u���L��
	emIsJFIF		= 0x04,			// JFIF�t�H�[�}�b�g�w��L��
	emIsComment		= 0x08,			// �R�����g�L��
	emIsStartData	= 0x10			// �X�L�����f�[�^�L��
};

// �t���[�������w��p�����[�^
struct
SFrameComponent {
	u_char		C;					// �������ʎq
	char		H;					// �����T���v�����O�t�@�N�^
	char		V;					// �����T���v�����O�t�@�N�^
	char		Tq;					// �ʎq���e�[�u���Z���N�^
};

// �X�L���������w��p�����[�^
struct
SScanComponent {
	u_char		Cs;					// �X�L���������Z���N�^
	char		Td;					// DC�G���g���s�������e�[�u���̃Z���N�^
	char		Ta;					// AC�G���g���s�������e�[�u���̃Z���N�^
};

// �X�L�����f�[�^�̃w�b�_
struct
SScanHeader {
	u_char		numOfScanComp;		// �X�L����������
	u_char		SpectralStart;		// �X�y�N�g�����J�n
	u_char		SpectralEnd;		// �X�y�N�g�����I��
	char		Ah;					// �T�N�Z�b�V�u�E�A�v���L�V���[�V�����̏�ʃr�b�g
	char		Al;					// �T�N�Z�b�V�u�E�A�v���L�V���[�V�����̉��ʃr�b�g
};

// �n�t�}���e�[�u��
struct
SHuffmanDecodeTable {
	int			numOfElement;		// �e�[�u���v�f��
	int*		SizeTP;				// �n�t�}���T�C�Y�e�[�u��
	int*		CodeTP;				// �n�t�}��������e�[�u��
	int*		ValueTP;			// �n�t�}��������ɑΉ������l
};

