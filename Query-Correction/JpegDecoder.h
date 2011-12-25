/******************************************************
CJpegDecoder Ver.1.1.0 JPEG�f�R�[�_�E�N���X
	Copyright 2004 AsWe Co.,Ltd. All rights reserved.
The comments are written in Japanese (Shift-JIS).

JPEG�f�R�[�_�E�N���X
	JPEG�f�[�^��RGB�摜�f�[�^�֕ϊ�����N���X

�g�p���@
	1.�C���X�^���X�𐶐�����
	2.SetJpegData()�ŉ摜�f�[�^�ւ̃|�C���^�ƃT�C�Y��n��
	3.SetJpegData()�̕Ԓl�ŃG���[���m�F����
	4.DoDisconvert()�ŋt�ϊ������s����
	5.DoDisconvert()�̕Ԓl�ŃG���[���m�F����
	6.GetRGBData()��RGB�摜�f�[�^�����o��

���ӎ���
	1.���̃t�@�C�������"JPEG.h"���C���N���[�h���邱��
	2.GetRGBData()�� redH, greenH, blueH �̗̈��CJpegDecoder��new�Ŋm�ۂ���B
	�@�Ăяo������delete���邱�ƁB
******************************************************/
#include "DecoderTables.h"
#include "JPEG.h"

class CInBitStream;

class CJpegDecoder
{
public:
	// �R���X�g���N�^
				CJpegDecoder( void );
	// �f�X�g���N�^
				~CJpegDecoder( void );

	// JPEG�f�[�^��ݒ肷��
	int		SetJpegData(							// �Ԓl�F��������
					char*		aJpegDataP,			// JPEG�f�[�^
					int			size );				// JPEG�f�[�^�̃T�C�Y

	// ������̉摜�f�[�^�ւ̃|�C���^��Ԃ��B
	//		redH, greenH, blueH �̗̈��CJpegDecoder��new�Ŋm�ۂ���B
	//		�Ăяo������delete���邱�ƁB
	void		GetRGBData(
					u_char**	redH,				// �����摜�ւ̃n���h��
					u_char**	greenH,				// �����摜�ւ̃n���h��
					u_char**	blueH,				// �����摜�ւ̃n���h��
					int&		HSize,				// ����f��
					int&		VSize );			// �c��f��

	// �������s�C���^�t�F�[�X(�G���[�������܂�)
	int		Decode( void );							// �Ԓl�F��������

	// �摜�����̎擾
	SJpegProperty	GetProperty( void )				// �Ԓl�FJPEG�摜�̓���
					{	return mProperty; };

protected:
	// ���o��,���ԃf�[�^
	u_char*				mRgbPA[3];					// �����摜�ւ̃|�C���^
	int*				mCompPA[3];					// �摜�̊e�F�����ւ̃|�C���g(���j�b�g�T�C�Y)
	int					mPreDCA[3];					// �O�u���b�N��DC����(3����)
	int					mDctDataA[64];				// 1�u���b�N����DCT�f�[�^
	int					mBlockA[64];				// 1�u���b�N���̉�f�f�[�^

	// �摜���
	SJpegProperty		mProperty;					// �v���p�e�B
	SScanHeader			mSHeader;					// �X�L�����w�b�_
	SFrameComponent		mFCompA[3];					// �t���[������
	SScanComponent		mSCompA[3];					// �X�L��������

	// �e�[�u��
	SHuffmanDecodeTable	mHTA[2][4];					// �����n�t�}���e�[�u�� AC,DC*4���
	int					mQTA[4][64];				// �ʎq���e�[�u���v�f *4����
	double				mCosT[8][8];				// cos�e�[�u��

	// ��������ԕϐ�
	int					mEnable;					// 1:SOI����EOI�̊Ԃ�������, 0:����ȊO
	CInBitStream		*mIBSP;						// JPEG�f�[�^�ǂݏo���p�r�b�g�X�g���[��
	char				mHMax;						// �����ő�T���v�����O�t�@�N�^
	char				mVMax;						// �����ő�T���v�����O�t�@�N�^
	int					mRestartInterval;			// ���X�^�[�g�C���^�o���i�C���^�o������MCU���j

	// �w�b�_�̉��
	int			AnalyzeHeader( void );					// �Ԓl�F��������
	// �}�[�J����
	eMarker		GetMarker( void );					// �Ԓl�F�}�[�J�R�[�h
	// �}�[�J����
	int			AnalyzeMarker( void );				// �Ԓl�F��͌���

	// JPEG�������s(�{����)
	void		DoDecode( void );
	// MCU����
	void		DecodeMCU();
	// RGB�ϊ�
	void		makeRGB(
					int			ux,					// �����j�b�g�ʒu
					int			uy );				// �c���j�b�g�ʒu
	// �G���g���s�[��������
	void		DecodeHuffmanBlock(
					int			cs );				// �����ԍ�
	// �n�t�}������1��̕���
	int			Decode1HuffmanCode(
					int			tc,					// �e�[�u���N���X 0:DC����, 1:AC����
					int			sc );				// �����ԍ�
	// �t�ʎq��
	void		InverseQuantization( 
					int		sc );					// �����ԍ�
	// 1�u���b�N�tDCT�ϊ�
	void		InverseDCT( void );

	// �n�t�}���e�[�u�����
	void		AnalyzeDHT(
					int			s  );				// �T�C�Y
	// �ʎq���e�[�u�����
	void		AnalyzeDQT(
					int			s  );				// �T�C�Y
	// �t���[�����
	void		AnalyzeFrame( void );
	// �X�L�����w�b�_���
	void		AnalyzeScan( void );
	// JFIF�t�H�[�}�b�g���
	void		AnalyzeJFIF( void );
	// JFXX�t�H�[�}�b�g���
	void		AnalyzeJFXX( void );

	// RGB�Z�o�p�l�␳
	u_char		ReviseValue(						// �Ԓl�F�␳�l
					double		v );				// �l
	// �G���[����RGB�f�[�^�p��
	void		DeleteRGB( void );
	// int�T�C�Y�̗̈�m��
	void		NewIntMemory(
					int**		h,					// �n���h��
					int			s,					// �T�C�Y
					int			v,					// �����l
					char		*mes );				// �G���[���b�Z�[�W
};

// �u���b�N���擾
#define GetBlocks(n)	(n>>3)+((n&0x07)>0)
