/******************************************************
CJpegDecoder JPEG�f�R�[�_�E�N���X Ver.1.1.0
	Copyright 2004 AsWe Co.,Ltd.
	All rights reserved.
The comments are written in Japanese (Shift-JIS).
******************************************************/
#include "stdafx.h"

#include <math.h>		// VC++�ł��K�v
#include <string.h>	    // �W��C++�̏ꍇ
#include "JPEG.h"
#include "InBitStream.h"
#include "JpegDecoder.h"

// �R���X�g���N�^
CJpegDecoder::CJpegDecoder( void )
{
	// �n�t�}���e�[�u��������
	for( int i=0; i<2; i++ )		// 0:DC����, 1:AC����
		for( int j=0; j<4; j++ ){	// 4����
			mHTA[i][j].SizeTP = mHTA[i][j].CodeTP = mHTA[i][j].ValueTP = NULL;
			mHTA[i][j].numOfElement = 0;
		}

	// �摜����������
	mProperty.CanDecode = emYet;	// �����
	mProperty.Format = 0;			// �t�H�[�}�b�g�w�薳��
	mProperty.MajorRevisions = 0;	// �o�[�W�����w�薳��
	mProperty.MinorRevisions = 0;	// �o�[�W�����w�薳��
	mProperty.Units = 0;			// �P�ʖ���
	mProperty.HDensity = 1;			// �����x1
	mProperty.VDensity = 1;			// �c���x1
	mProperty.HThumbnail = 0;		// �T���l�C������f��0
	mProperty.VThumbnail = 0;		// �T���l�C���c��f��0
	mProperty.ExtensionCode = 0;	// �g���R�[�h����
	mProperty.SamplePrecision = 0;	// �T���v�����x8�r�b�g
	mProperty.CommentP= NULL;		// �R�����g����
	mRestartInterval = 0;			// ���X�^�[�g�C���^�o���������F����
	mIBSP = NULL;					// �r�b�g�X�g���[������

	// cos�e�[�u���쐬
	for( int n=0; n<8; n++ )
		for( int m=0; m<8; m++ )
			mCosT[n][m] = cos(( 2 * m + 1) * n * kPaiDiv16);
}

// �f�X�g���N�^
CJpegDecoder::~CJpegDecoder( void )
{
	// �e�[�u���p��
	for( int i=0; i<2; i++ )		// 0:DC����, 1:AC����
		for( int j=0; j<4; j++ ){	// 4����
			if( mHTA[i][j].CodeTP != NULL )
				delete mHTA[i][j].CodeTP;
			if( mHTA[i][j].SizeTP != NULL )
				delete mHTA[i][j].SizeTP;
			if( mHTA[i][j].ValueTP != NULL )
				delete mHTA[i][j].ValueTP;
		}

	// �R�����g�̈�p��
	if( mProperty.CommentP != NULL )
		delete mProperty.CommentP;

	// �r�b�g�X�g���[���̔p��
	if( mIBSP != NULL )
		delete mIBSP;
}

// JPEG�f�[�^��ݒ肷��
int
CJpegDecoder::SetJpegData(
	char*		aJpegDataP,			// JPEG�f�[�^
	int			size )				// JPEG�f�[�^�̃T�C�Y
{
	// �r�b�g�X�g���[���̐���
	if( mIBSP != NULL )
		delete mIBSP;
	mIBSP = new CInBitStream( aJpegDataP, size );
	// �w�b�_�̉��
	return	AnalyzeHeader();			// �w�b�_��͌��ʂ�Ԃ�
}

// �ϊ���̉摜�f�[�^�ւ̃|�C���^��Ԃ��B
void
CJpegDecoder::GetRGBData(
	u_char**	redH,				// �����摜�ւ̃|�C���^
	u_char**	greenH,				// �����摜�ւ̃|�C���^
	u_char**	blueH,				// �����摜�ւ̃|�C���^
	int&		HSize,				// ����f��
	int&		VSize )				// �c��f��
{
	HSize = mProperty.HSize;
	VSize = mProperty.VSize;

	*redH = mRgbPA[0];
	*greenH = mRgbPA[1];
	*blueH = mRgbPA[2];
}

#ifdef _MSC_VER
#pragma warning(disable : 4101)
#endif

// �t�ϊ����s
int
CJpegDecoder::Decode( void )
{
	try{
		// �n�t�}���e�[�u��,�ʎq���e�[�u��,�X�L�����f�[�^�̑��݂̊m�F
		if( !(mProperty.CanDecode & (emIsHTable|emIsQTable|emIsStartData)) )
			return	emDataError;

		// �����\�Ȃ��
		DoDecode();
		return	emNoError;
	}

	catch( EMemoryError &me ){							// �������G���[����
		DeleteRGB();									// RGB�̈�̔p��
		return	emRuntimeError;							// ���s���G���[
	}

	catch( EIndexError &ie ){							// �e�[�u���A�N�Z�X�ᔽ
		DeleteRGB();									// RGB�̈�̔p��
		return	emRuntimeError;							// ���s���G���[
	}

	catch( EBufferError &eob ){							// �o�b�t�@�A�N�Z�X�ᔽ
		DeleteRGB();									// RGB�̈�̔p��
		return	emRuntimeError;							// ���s���G���[
	}

	catch( EDataFormatError &dfe ){						// �n�t�}�������������s
		DeleteRGB();									// RGB�̈�̔p��
		return	emDataError;							// �f�[�^�G���[
	}

	catch( ENotSupported &ns ){							// ��Ή��t�H�[�}�b�g
		DeleteRGB();									// RGB�̈�̔p��
		return	emDataError;							// �f�[�^�G���[
	}
}

/******************************* protected ***********************************/
// �w�b�_�̉��

int
CJpegDecoder::AnalyzeHeader( void )
{
	mEnable = 0;
	eMarker		mark;

	try{
		// �摜�J�n�}�[�J��T��
		while( mEnable == 0 ){							// SOI�}�[�J��T��
			mark = GetMarker();
			if( mark == emSOI )							// �摜�J�n
				mEnable = 1;
		}

		// ��͏���
		while( mEnable == 1 ){							// EOI�}�[�J�܂�
			mProperty.CanDecode |= AnalyzeMarker();		// ��͎��s�����ʂ̍X�V
			if( mProperty.CanDecode & emIsStartData )	// �X�L�����f�[�^�̎n�܂�
				return	(int)emNoError;
		}

		return (int)emNoError;							// �G���[�����i�X�L�����f�[�^�����j
	}	// end try

	// �G���[����
	catch( EMemoryError &me ){							// �������G���[����
		return (int)emRuntimeError;						// ���s���G���[
	}

	catch( EIndexError &ie ){							// �e�[�u���A�N�Z�X�ᔽ
		return (int)emDataError;						// �f�[�^�s���ɂ��A�N�Z�X�ᔽ
	}

	catch( EBufferError &eob ){							// �o�b�t�@�A�N�Z�X�ᔽ
		return (int)emDataError;						// EOI�}�[�J���܂܂�Ă��Ȃ�
	}

	catch( EDataFormatError &dfe ){						// �n�t�}�������̕������s
		return (int)emDataError;						// �f�[�^�G���[
	}

	catch( ENotSupported &ns ){							// ��Ή��t�H�[�}�b�g
		return (int)emDataError;						// �f�[�^�s���ɂ��A�N�Z�X�ᔽ
	}
}

#ifdef _MSC_VER
#pragma warning(default : 4101)
#endif

// �}�[�J����
int
CJpegDecoder::AnalyzeMarker( void )					// �Ԓl�F��͌���
{
	int		length;
	eMarker	mark = GetMarker();

	switch( mark ){
		case emSOF0:	// �t���[������(�񍷕��n�t�}�������F��{DCT����)
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			AnalyzeFrame();
			break;

		case emDHT:		// �n�t�}���e�[�u��
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			AnalyzeDHT( length );
			return emIsHTable;							// �n�t�}���e�[�u���L��

		case emDNL:		// ���C������`
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			mProperty.VSize = mIBSP->GetWord();			// ���C����
			break;

		case emDQT:		// �ʎq���e�[�u����`
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			AnalyzeDQT( length );
			return emIsQTable;							// �ʎq���e�[�u���L��

		case emEOI:		// �摜�I��
			mEnable = 0;
			break;

		case emSOS:		// �X�L�����J�n
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			AnalyzeScan();
			return emIsStartData;						// �X�L�����f�[�^�L��

		case emDRI:		// ���X�^�[�g�C���^�o����`
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			mRestartInterval = mIBSP->GetWord();
			break;

		case emCOM:		// �R�����g
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			if( mProperty.CommentP != NULL )				// �Ō�̃R�����g�̂ݗL���Ƃ���
				delete mProperty.CommentP;
			mProperty.CommentP = new( char[length + 1] );	// �O�̂���1�o�C�g�]�v�ɂƂ���'\0'������
			mIBSP->CopyByte( mProperty.CommentP, length );
			*(mProperty.CommentP+length) = '\0';			// �O�̂���
			return emIsComment;

		// ��Ή��t���[������
		case emSOF1:	// �񍷕��n�t�}�������F�g���V�[�P���V����DCT����
		case emSOF2:	// �񍷕��n�t�}�������F�v���O���b�V�uDCT����
		case emSOF3:	// �񍷕��n�t�}�������F��ԉt����
		case emSOF5:	// �����n�t�}�������F�����V�[�P���V����DCT����
		case emSOF6:	// �����n�t�}�������F�����v���O���b�V�uDCT����
		case emSOF7:	// �����n�t�}�������F������ԕ���
		case emSOF9:	// �񍷕��Z�p�����F�g���V�[�P���V����DCT����
		case emSOF10:	// �񍷕��Z�p�����F�v���O���b�V�uDCT����
		case emSOF11:	// �񍷕��Z�p�����F��ԁi�V�[�P���V�����j�t����
		case emSOF13:	// �����Z�p�����F�����V�[�P���V����DCT����
		case emSOF14:	// �����Z�p�����F�����v���O���b�V�uDCT����
		case emSOF15:	// �����Z�p�����F������ԕ���
		case emEXP:		// �Q�Ɛ����g��
		case emDAC:		// �Z�p�����������t����`
		case emDHP:		// �n�C�A���[�L�J���E�v���O���b�V������`
			throw( ENotSupported( mark, mIBSP->GetNextAddress() ) );
			break;

		// �A�v���P�[�V�����f�[�^�Z�O�����g
		case emAPP0:	// JPEG FIF
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			if( length >= 5 ){
				char	id[5];
				mIBSP->CopyByte( id, 5 );
				id[4] = '\0';							// �O�̂���
				if( strcmp( kJFIF, id ) == 0 ){			// JFIF�t�H�[�}�b�g�Ȃ��
					AnalyzeJFIF();
					mIBSP->SkipByte( length - 14 );		// �T���l�C���̓T�|�[�g���Ȃ�
				}
				else if( strcmp( kJFXX, id ) == 0 ){	// JFXX�t�H�[�}�b�g�Ȃ��
					AnalyzeJFXX();
					mIBSP->SkipByte( length - 1 );		// �T���l�C���̓T�|�[�g���Ȃ�
				}
				else									// JFIF�ł�JFXX�ł��Ȃ��Ȃ��
					mIBSP->SkipByte( length - 5 );		// ���̃o�[�W�����ł̓T�|�[�g���Ȃ��̂œǂݔ�΂�
			}
			else
				mIBSP->SkipByte( length );
			break;

		// ��Ή��}�[�J
		case emAPP1:	case emAPP2:	case emAPP3:	case emAPP4:
		case emAPP5:	case emAPP6:	case emAPP7:	case emAPP8:
		case emAPP9:	case emAPP10:	case emAPP11:	case emAPP12:
		case emAPP13:	case emAPP14:	case emAPP15:
			length = mIBSP->GetWord() - 2;				// �p�����[�^���w�蕔��������
			mIBSP->SkipByte( length );					// ���̃o�[�W�����ł̓T�|�[�g���Ȃ��̂œǂݔ�΂�
			break;

		// ���U�[�u��
		case emJPG:		case emJPG0:	case emJPG1:	case emJPG2:
		case emJPG3:	case emJPG4:	case emJPG5:	case emJPG6:
		case emJPG7:	case emJPG8:	case emJPG9:	case emJPG10:
		case emJPG11:	case emJPG12:	case emJPG13:	case emTEM:
		// �G���[
		case emError:
		default:
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "Marker error" ));
			break;
	}
	
	return	emYet;										// ��͌p��
}

// �}�[�J����
// �ǂݏo���ʒu���}�[�J�̎��ɂ���
eMarker
CJpegDecoder::GetMarker( void )
{
	while( 1 ){	// �o�b�t�@�̏I���܂ŌJ��Ԃ� �o�b�t�@���I����CInBitStream��throw����
		u_char	c = mIBSP->GetByte();
		if( c == (u_char)emMarker ){					// 0xff�̂Ƃ�
			c = mIBSP->GetByte();						// ���̃o�C�g�����o��
			if( c != (u_char)0 )						// 0�łȂ����
				if( c > (u_char)emRESst && c < (u_char)emSOF0 )	// ��`���ꂽ�}�[�J�łȂ��Ȃ�
					return	emError;					// �G���[
				else									// ��`���ꂽ�}�[�J�Ȃ�
					return (eMarker)c;					// �}�[�J��Ԃ�
		}
	}
}

// �n�t�}���e�[�u�����
void
CJpegDecoder::AnalyzeDHT(
	int			s )			// �T�C�Y
{
	// �I���A�h���X
	char*	theEndAddressP = mIBSP->GetNextAddress();
	if( theEndAddressP == NULL )
		throw( EDataFormatError( theEndAddressP, "AnalyzeDHT:DHT Length" ) );
	theEndAddressP += s;

	// ��͊J�n
	do{
		u_char	uc = mIBSP->GetByte();
		int		tc, th;
		if( (tc = uc>>4) > 1 )							// 0:DC����, 1:AC����
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "AnalyzeDHT:Tc" ) );
		if( (th = uc & 0x0f) > 3 )						// 0�`3
			throw( EDataFormatError( mIBSP->GetNextAddress()-1, "AnalyzeDHT:Th" ) );
		SHuffmanDecodeTable 	&aHT = mHTA[tc][th];		// �i�[�̈���w��

		int		i;										// �J�E���^
		int		num = 0;								// ������̐�
		u_char	cc[16];
		for( i=0; i<16; i++ ){
			cc[i] = mIBSP->GetByte();
			num += cc[i];
		}
		aHT.numOfElement = num;							// �v�f��

		// �e�[�u���̈�m��
		NewIntMemory( &aHT.SizeTP, num, 0, "AnalyzeDHT:Huffman Size-T" );
		NewIntMemory( &aHT.CodeTP, num, 0, "AnalyzeDHT:Huffman Code-T" );
		NewIntMemory( &aHT.ValueTP, num, 0, "AnalyzeDHT:Huffman Value-T" );

		// �T�C�Y�e�[�u������
		int		k = 0;									// �C���f�b�N�X
		for( i=1; i<=16; i++ ){							// �����꒷i�r�b�g�ɂ���
			int	j=1;									// SizeTP[]�ɋL�^��������
			while( j <= cc[i-1] ){						// i�r�b�g�̕����ꐔ�܂�
				if( k >= num )							// �����ꑍ�����I�[�o�[������
					throw( EIndexError( num, k, "AnalyzeDHT:Size Table Index over" ) );	// �A�N�Z�X�ᔽ�Ď�
				aHT.SizeTP[k++] = i;					// �����꒷�i�r�b�g���j���L�^
				j++;
			}
		}

		// ������e�[�u������
		k = 0;											// �C���f�b�N�X
		int	code = 0;									// ������i�R�[�h�j
		int	si = aHT.SizeTP[0];							// �T�C�Y�i�r�b�g���j
		while( 1 ){
			while( aHT.SizeTP[k] == si )				// �T�C�Y�̊m�F
				aHT.CodeTP[k++] = code++;				// ������̌���
			if( k >= num )								// �I������
				break;									// �I��

			do {										// ���̃T�C�Y�̏���
				code <<= 1;								// �����꒷��1�r�b�g���₷
				si++;									// �T�C�Y��1�r�b�g���₷
			} while( aHT.SizeTP[k] != si );				// si�r�b�g�̕����ꂪ�Ȃ��Ƃ��J��Ԃ�
		}	// while(1)

		// �����e�[�u������
		for( k=0; k<num; k++ )
			aHT.ValueTP[k] = mIBSP->GetByte();			// �Ή�����l�̎��o��

	}while( mIBSP->GetNextAddress() < theEndAddressP );
}

// �ʎq���e�[�u�����
void
CJpegDecoder::AnalyzeDQT(
	int			s )		// �T�C�Y
{
	char* theEndAddressP = mIBSP->GetNextAddress() + s;	// �I���A�h���X

	do {
		// �e�[�u���̎w��
		u_char	c = mIBSP->GetByte();
		int		*aQT = mQTA[ c&0x3 ];					// �e�[�u���͍ő�4��

		int		Pq = c >> 4;							// �G�������g���x 0:�v�fQk��8�r�b�g, 1:16�r�b�g
		if( Pq == 0 )									// 8�r�b�g�v�f
			for( int i=0; i<64; i++ )
				aQT[kZigzagT[i]] = int( u_char( mIBSP->GetByte() ) );
		else											// 16�r�b�g�v�f
			for( int i=0; i<64; i++ )
				aQT[kZigzagT[i]] = mIBSP->GetWord();

	}while( mIBSP->GetNextAddress() < theEndAddressP );
}

// �t���[���w�b�_���
void
CJpegDecoder::AnalyzeFrame( void )
{
	mProperty.SamplePrecision = mIBSP->GetByte();		// �T���v�����x
	mProperty.VSize = mIBSP->GetWord();					// �摜���T�C�Y
	mProperty.HSize = mIBSP->GetWord();					// �摜�c�T�C�Y
	mProperty.Dimension = mIBSP->GetByte();				// ������
	if( mProperty.Dimension != 3 && mProperty.Dimension != 1 )	// ��������3�܂���1�łȂ��ꍇ
		throw( ENotSupported( emSOF0, mIBSP->GetNextAddress() ) );	// �T�|�[�g���Ȃ�

	int i;
	mHMax = mVMax = 0;
	for( i=0; i<mProperty.Dimension; i++ ){
		mFCompA[i].C = mIBSP->GetByte();					// �������ʎq
		u_char	c = mIBSP->GetByte();

		mFCompA[i].H = c >> 4;							// �����T���v�����O�t�@�N�^
		if( mFCompA[i].H > mHMax )
			mHMax = mFCompA[i].H;

		mFCompA[i].V = c & 0xf;							// �����T���v�����O�t�@�N�^
		if( mFCompA[i].V > mVMax )
			mVMax = mFCompA[i].V;

		mFCompA[i].Tq = mIBSP->GetByte();				// �ʎq���e�[�u���Z���N�^
	}
}

// �X�L�����w�b�_���
void
CJpegDecoder::AnalyzeScan( void )
{
	u_char	c;
	mSHeader.numOfScanComp = mIBSP->GetByte();			// �X�L����������

	for( int i=0; i<mSHeader.numOfScanComp; i++ ){
		mSCompA[i].Cs = mIBSP->GetByte();				// �X�L���������Z���N�^
		c = mIBSP->GetByte();
		mSCompA[i].Td = c >> 4;							// DC�����p�n�t�}���e�[�u���̃Z���N�^
		if( mSCompA[i].Td > 2 )
			throw( EIndexError( 2, mSCompA[i].Td, "AnalyzeScan:Td Over" ) );
		mSCompA[i].Ta = c & 0xf;							// AC�����p�n�t�}���e�[�u���̃Z���N�^
		if( mSCompA[i].Ta > 2 )
			throw( EIndexError( 2, mSCompA[i].Ta, "AnalyzeScan:Ta Over" ) );
	}

	mSHeader.SpectralStart = mIBSP->GetByte();			// ���g�p
	mSHeader.SpectralEnd = mIBSP->GetByte();			// ���g�p
	c = mIBSP->GetByte();								// ���g�p
	mSHeader.Ah = c >> 4;								// ���g�p
	mSHeader.Al = c & 0xf;								// ���g�p
}

// JFIF�t�H�[�}�b�g���
void
CJpegDecoder::AnalyzeJFIF( void )
{
	mProperty.Format = 1;								// JFIF
	mProperty.MajorRevisions = mIBSP->GetByte();		// �o�[�W�������
	mProperty.MinorRevisions = mIBSP->GetByte();		// �o�[�W��������
	mProperty.Units = mIBSP->GetByte();					// �P�� 0:�����A1:dots/inch�A2:dots/cm
	mProperty.HDensity = mIBSP->GetWord();				// �����x
	mProperty.VDensity = mIBSP->GetWord();				// �c���x
	mProperty.HThumbnail = mIBSP->GetByte();			// �T���l�C������f��
	mProperty.VThumbnail = mIBSP->GetByte();			// �T���l�C���c��f��
	mProperty.CanDecode |= emIsJFIF;					// JFIF�t�H�[�}�b�g�w��L��
}

// JFXX�t�H�[�}�b�g���
void
CJpegDecoder::AnalyzeJFXX( void )
{
	mProperty.Format = 2;								// JFXX
	mProperty.ExtensionCode = mIBSP->GetByte();			// �g���R�[�h 0x10:JPEG, 0x11:1byte/pixel, 0x13:3bytes/pixel
}

// JPEG�t�ϊ����s
void
CJpegDecoder::DoDecode( void )
{
	// �萔�̌v�Z
	int		VBlocks = GetBlocks( mProperty.VSize );		// �c�u���b�N��
	int		HBlocks = GetBlocks( mProperty.HSize );		// ���u���b�N��
	int		numHUnit = HBlocks / mHMax;					// �����j�b�g��
	if( HBlocks % mHMax )
		numHUnit++;
	int		numVUnit = VBlocks / mVMax;					// �c���j�b�g��
	if( VBlocks % mVMax )
		numVUnit++;

	// ���ʂ̊i�[�̈���m��
	int		rgbSize = (numVUnit * mVMax) * 8 * (numHUnit * mHMax) * 8;	// �����Ɋ܂܂��t���������܂�ł���

	mRgbPA[0] = new u_char[ rgbSize ];					// �Ԑ���
	if( mRgbPA[0] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[0]" ));
	mRgbPA[1] = new u_char[ rgbSize ];					// �ΐ���
	if( mRgbPA[1] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[1]" ));
	mRgbPA[2] = new u_char[ rgbSize ];					// ����
	if( mRgbPA[2] == NULL )
		throw( EMemoryError( rgbSize, "DoDecode:mRgbPA[2]" ));

	// ���ʂ̊i�[�̈�̊m��
	mCompPA[0] = mCompPA[1] = mCompPA[2] = NULL;
	int	unitSize = mHMax * mVMax * 64;					// ���j�b�g���f�[�^�T�C�Y
	NewIntMemory( &mCompPA[0], unitSize, 0, "Disconvert unit1" );
	NewIntMemory( &mCompPA[1], unitSize, 0x80, "Disconvert unit2" );
	NewIntMemory( &mCompPA[2], unitSize, 0x80, "Disconvert unit3" );

	mPreDCA[0] = mPreDCA[1] = mPreDCA[2] = 0;			// �O�u���b�N��DC�����l��������
	int		restartCount = 0;							// �C���^�o���J�E���^

	// ���j�b�g���Ƃɏ���
	for( int uy=0; uy<numVUnit; uy++ ){					// �c���j�b�g�w��
		for( int ux=0; ux<numHUnit; ux++ ){				// �����j�b�g�w��

			DecodeMCU();								// MCU�̏���
			makeRGB( ux, uy );							// RGB�ɕϊ�

			if( mRestartInterval ){						// ���X�^�[�g�C���^�o���L���Ȃ��
				if(++restartCount >= mRestartInterval){	// �C���^�o���I���Ȃ��
					restartCount = 0;					// �J�E���^���Z�b�g
					eMarker		mark = GetMarker();		// �}�[�J���o
					if( mark >= emRST0 && mark <= emRST7 )	// ���X�^�[�g�}�[�J�Ȃ��
						mPreDCA[0] = mPreDCA[1] = mPreDCA[2] = 0;	// �O�u���b�N��DC�����l�����Z�b�g
				}	// if( ++restartCount >= ���
			}	// if( mRestartInterval )
		}	// for ux
	}	// for uy

	delete mCompPA[0];	delete mCompPA[1];	delete mCompPA[2];
}

// MCU�f�R�[�h
void
CJpegDecoder::DecodeMCU( void )
{
	// �������Ƃ�
	for( int sc=0; sc<mProperty.Dimension; sc++ ){
		int	numV = mFCompA[sc].V;						// �����T���v�����O�t�@�N�^
		int	numH = mFCompA[sc].H;						// �����T���v�����O�t�@�N�^
		int	cntY = mVMax/numV;							// ���j�b�g���d����
		int	cntX = mHMax/numH;							// ���j�b�g���d����
		int	vStep = mHMax * 8;							// �c�����̃A�h���X������

		for( int ky=0; ky<numV; ky++ ){					// ���j�b�g���c�u���b�N����
			for( int kx=0; kx<numH; kx++ ){				// ���j�b�g�����u���b�N����
				DecodeHuffmanBlock( sc );				// �G���g���s�[�����̕���
				InverseQuantization( sc );				// �t�ʎq��
				InverseDCT();							// �tDCT�ϊ�

				// �u���b�N�P�ʂ̃f�[�^��\�������ɕϊ����Ԉ����ꂽ���̂����ɖ߂�
				int*	tp = mCompPA[sc] + ky * vStep * 8 + kx * 8;
				for( int yInU=0; yInU<8*cntY; yInU++ )
					for( int xInU=0; xInU<8*cntX; xInU++ )
						tp[ yInU * vStep + xInU ] = mBlockA[(yInU / cntY) * 8 + (xInU / cntX)];
			}	// kx
		}	// ky
	}	// sc
}

// RGB�ϊ�
void
CJpegDecoder::makeRGB(
	int			ux,										// �����j�b�g�ʒu
	int			uy )									// �c���j�b�g�ʒu
{
	int*	yp = mCompPA[0];							// Y
	int*	up = mCompPA[1];							// Cb
	int*	vp = mCompPA[2];							// Cr
	int		numLine = uy * mVMax * 8;					// ���C���F�c���j�b�g���~���j�b�g���C����
	int		offsetV = numLine * mProperty.HSize;		// �c�I�t�Z�b�g�F���C���~�P���C���h�b�g��
	int		offsetH = ux * mHMax * 8;					// ���I�t�Z�b�g�F�����j�b�g���~���j�b�g�����u���b�N���~�u���b�N���h�b�g��
	int		offset = offsetV + offsetH;					// �����I�t�Z�b�g
	u_char*	rp = mRgbPA[0] + offset;					// �ԏ������݈ʒu
	u_char*	gp = mRgbPA[1] + offset;					// �Ώ������݈ʒu
	u_char*	bp = mRgbPA[2] + offset;					// �������݈ʒu
	int		endX = mHMax*8;								// �����[�v�I���|�C���g
	int		endY = mVMax*8;								// �c���[�v�I���|�C���g

	for( int picY=0; picY<endY; picY++ ){
		for( int picX=0; picX<endX; picX++ ){
			if( picX + offsetH >= mProperty.HSize ){	// �t���r�b�g�Ȃ��
				yp += endX - picX;						// �\�[�X�A�h���X���X�V
				up += endX - picX;
				vp += endX - picX;
				break;									// ���̃��C����
			}

			int	index = picY * mProperty.HSize + picX;	// �������݈ʒu

#ifdef GRAY
			gp[index] = bp[index] =	rp[index] = ReviseValue( *yp++ );
#else
	double	v = *yp							+ (*vp - 0x80) * 1.4020;
			rp[index] = ReviseValue( v );				// R
			v = *yp - (*up - 0x80) * 0.3441	- (*vp - 0x80) * 0.7139;
			gp[index] = ReviseValue( v );				// G
			v = *yp + (*up - 0x80) * 1.7718;
			bp[index] = ReviseValue( v );				// B

			yp++; vp++; up++;
#endif
		}	// picX
	}	// picY
}

// 1�u���b�N���̃G���g���s�[�����𕜍��i�t�ʎq�����܂ށj
void
CJpegDecoder::DecodeHuffmanBlock(
	int		sc )										// �����ԍ�
{
// DC��������
	int		diff = 0;									// DC��������
	// �n�t�}������1��̕���
	int category = Decode1HuffmanCode( 0, sc );			// �J�e�S���i�����l�r�b�g���j
	if( category > 0 ){
		diff = mIBSP->GetBits( category );
		if( ( diff & ( 1 << (category-1) )) == 0 )		// ��������
			diff -= (1 << category ) - 1;
	}
	else if( category < 0 )								// �}�[�J
		throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:Error Marker" ) );

	mPreDCA[sc] += diff;
	mDctDataA[0] = mPreDCA[sc];							// DC�����l�ݒ�i���ёւ��s�v�j

// AC��������

	int 	k = 1;
	while( k < 64 ){
		// �n�t�}�������P��̕���
		category = Decode1HuffmanCode( 1, sc );			// ���������O�X�ƃJ�e�S��
		if( category == 0 ){							// EOB
			while( k < 64 )
				mDctDataA[kZigzagT[k++]] = 0;
			break;
		}
		else if( category < 0 ) 						// �}�[�J
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:Error Marker" ) );

		int	run = category >> 4;						// ���������O�X
		category &= 0x0f;								// �J�e�S��

		int	acv = 0;									// AC�����l
		if( category ){
			acv = mIBSP->GetBits( category );			// AC�������o��
			if( (acv & ( 1 << (category-1) )) == 0 )	// ��������
				acv -= (1 << category ) - 1;
		}
		else if( run != 15 )							// EOB�ł�ZRL�łȂ�
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:wrong Huffman value" ) );

		if( run + k > 63 )								// �W������������
			throw( EDataFormatError( mIBSP->GetNextAddress(), "DecodeHuffman:wrong Run length" ) );

		while( run-- > 0 )								// ���������O�X�̐�����0
			mDctDataA[kZigzagT[k++]] = 0;

		mDctDataA[kZigzagT[k++]] = acv;					// �����ϊ�

	}
}

// �n�t�}������1��̕���
int
CJpegDecoder::Decode1HuffmanCode(
	int		tc,											// �e�[�u���N���X 0:DC����, 1:AC����
	int		sc )										// �����ԍ�
{
	// �n�t�}���e�[�u���w��
	SHuffmanDecodeTable	&theHT = mHTA[tc][mSCompA[sc].Td];	// �g�p����n�t�}���e�[�u��

	int		code = 0;									// �n�t�}��������̌��F�ő�l16�r�b�g
	int		length = 0;									// �n�t�}����������̃r�b�g��
	int		next = 0;									// ����1�r�b�g
	int		k = 0;										// �\�̎w��

	while( k < theHT.numOfElement && length < 16 ){
		length++;
		code <<= 1;
		next = mIBSP->GetBits( 1 );
		if( next < 0 )									// �}�[�J��������
			return	next;
		code |= next;

		while( theHT.SizeTP[k] == length ){				// ���ƕ�����̃r�b�g�����������Ԃ�����
			if( theHT.CodeTP[k] == code )				// �q�b�g
				return theHT.ValueTP[k];				// �������ʂ�Ԃ�
			k++;										// ���̕�����
		}
	}

	throw( EDataFormatError( mIBSP->GetNextAddress(), "Decode1HuffmanCode:length over" ) );
}

// �t�ʎq��
void
CJpegDecoder::InverseQuantization( 
	int		sc )											// �����ԍ�
{
	int	*p = mDctDataA;										// DCT�W���f�[�^
	int	*aQT = mQTA[mFCompA[sc].Tq];							// �ʎq���e�[�u��

	for( int i=0; i<64; i++ )
		*p++ *= *aQT++;										// �t�ʎq��
}

// �tDCT�ϊ�
void
CJpegDecoder::InverseDCT( void )
{
	int	sl = mProperty.SamplePrecision == 8 ? 128 : 2048;	// ���x���V�t�g�l�̌���

	for( int y=0; y<8; y++ ){
		for( int x=0; x<8; x++ ){
			double	sum = 0;

			for( int v=0; v<8; v++ ){
				double cv = v==0 ? kDisSqrt2 : 1.0;
				for( int u=0; u<8; u++ ){
					double cu = u==0 ? kDisSqrt2 : 1.0;

					sum += cu * cv * mDctDataA[ v * 8 + u ] * mCosT[u][x] * mCosT[v][y];
				}
			}
			mBlockA[ y*8+x ] = int( sum / 4 + sl );
		}
	}
}


// RGB�Z�o�p�l�␳
u_char
CJpegDecoder::ReviseValue(								// �Ԓl�F�␳�l
	double	v )											// �l
{
	if( v < 0.0 )
		return 0;
	if( v > 255.0 )
		return 255;
	return (u_char)v;
}

// �G���[����RGB�f�[�^�̈�̔p��
void
CJpegDecoder::DeleteRGB( void )
{
	if( mRgbPA[0] != NULL )	delete mRgbPA[0];
	if( mRgbPA[1] != NULL )	delete mRgbPA[1];
	if( mRgbPA[2] != NULL )	delete mRgbPA[2];
}

// int�T�C�Y�̗̈�m��
void
CJpegDecoder::NewIntMemory(
	int**	h,			// �n���h��
	int		s,			// �T�C�Y
	int		v,			// �����l
	char*	mes )		// �G���[���b�Z�[�W
{
	if( *h != NULL )
		delete *h;
	*h = new int[s];
	if( *h == NULL )
		throw( EMemoryError( s, mes ) );
	while( s-- )
		(*h)[s] = v;
}