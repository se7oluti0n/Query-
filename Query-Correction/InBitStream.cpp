/******************************************************
CInBitStream �r�b�g�X�g���[���E�N���X Ver.1.1.0
	Copyright 2004 AsWe Co.,Ltd.
	All rights reserved.
The comments are written in Japanese (Shift-JIS).
******************************************************/
#include "stdafx.h"
#include <string.h>	// �W��C++�̏ꍇ
#include "JPEG.h"
#include "InBitStream.h"

CInBitStream::CInBitStream(
	char* 	aBufP,						// �o�b�t�@
	int		size )						// �o�b�t�@�̃T�C�Y
{
	// �f�[�^�o�b�t�@�̐ݒ�
	mBufP = (u_char*)aBufP;				// �o�b�t�@
	mEndOfBufP = mBufP + size;			// �o�b�t�@�̍ŏI�A�h���X

	// ��ԕϐ�������
	mBitPos = 7;						// �ŏ�ʃr�b�g
	mNextFlag = 1;						// �ǂݔ�΂�����
	mReadFlag = 1;						// �A�N�Z�X�G���[����
}

// �ǂݏo���A�h���X�̃C���N�������g�ƃA�N�Z�X�ᔽ�̃`�F�b�N
void
CInBitStream::IncBuf( void )
{
	if( ++mBufP >= mEndOfBufP )			// ���̃A�N�Z�X�ŃG���[
		mReadFlag = 0;
}

// �r�b�g�P�ʂœǂݏo��
int										// �ǂݏo�����r�b�g�l�F�G���[�R�[�h
CInBitStream::GetBits(
	int		numOfBits )					// �ǂݏo���r�b�g��
{
	if( numOfBits <= 0 )
		return 0;						// �G���[

	u_char	c;
	int		r = 0;						// �Ԓl
	while( numOfBits ){
		if( mBitPos < 0 ){				// ���̃o�C�g��ǂݏo���Ƃ�
			mBitPos = 7;				// �ǂݏo���r�b�g�ʒu�X�V
			IncBuf();					// �A�h���X�X�V
			if( mNextFlag == 0 ){		// 0xff�ɑ���0x00��ǂݔ�΂�
				IncBuf();
				mNextFlag = 1;
			}	// mNextFlag == 0

			c = *mBufP;					// �ǂݏo���ʒu�̒l
			if( c == 0xff ){			// �}�[�J�Ɠ�����������
				c = *( mBufP + 1 );		// ���̃o�C�g���`�F�b�N
				if( c != 0 )			// �}�[�J��������
					return - c;			// �G���[�Ƃ��ăR�[�h�𕉂ɂ��ĕԂ�
				mNextFlag = 0;			// ���ɑ���0x00��ǂݔ�΂�����
			}	// c == 0xff
		}	// mBitPos < 0

		// �Ԓl�̍쐬
		r <<= 1;
		r |= ( (*mBufP) & kBitTestMaskT[mBitPos--] ) ? 1 : 0;
										// 1�r�b�g�ǂݏo���Ɠǂݏo���r�b�g�ʒu�X�V
		numOfBits--;					// �ǂݏo���r�b�g���X�V
	}	// while
	return	r;
}

// 1�o�C�g�ǂݏo��
u_char									// �Ԓl�F�ǂݏo��������
CInBitStream::GetByte( void )
{
	if( mReadFlag ){
		if( mBitPos != 7 ){				// �ǂݏo���ʒu���o�C�g�̓r���Ȃ��
			IncBuf();					// ���̃o�C�g��
			mBitPos = 7;				// �ŏ��̃r�b�g����
		}
		u_char r = *mBufP;				// 1�o�C�g�ǂݏo��
		IncBuf();						// �ǂݏo���ʒu�X�V
		mNextFlag = 1;					// �r�b�g�ǂݏo�����Ƀ}�[�J�����o�����ꍇ0�ɂȂ邪�A
										// GetByte()�Ń}�[�J����������̂Ŏ��̃o�C�g��ǂ�ł悢
		return	r;
	}
	throw( EBufferError( "GetByte" ) );	// �o�b�t�@�I�[�o�[
}

// 2�o�C�g�ǂݏo��
u_int									// �Ԓl�F�ǂݏo��������
CInBitStream::GetWord( void )
{
	if( mReadFlag ){
		if( mBitPos != 7 ){				// �o�C�g�̓r������ǂݏo���Ȃ�
			IncBuf();
			mBitPos = 7;
		}
		u_int r = (unsigned)*mBufP << 8;
		IncBuf();
		r |= (unsigned)*mBufP;
		IncBuf();
		return	r;
	}
	throw( EBufferError( "GetWord" ) );
}

// n�o�C�g�ǂݏo��
void
CInBitStream::CopyByte(
	char*	disP,						// �i�[��A�h���X
	int		n )							// �ǂݏo���o�C�g��
{
	if( mBitPos != 7 ){					// �o�C�g�̓r������ǂݏo���Ȃ�
		IncBuf();
		mBitPos = 7;
	}
	if( mBufP + n > mEndOfBufP )
		throw( EBufferError( "CopyByte" ) );
	memcpy( disP, mBufP, n );
	if( (mBufP += n) >= mEndOfBufP )		// ���̃A�N�Z�X�ŃG���[
		mReadFlag = 0;
}

// n�o�C�g�i�߂�
void
CInBitStream::SkipByte(
	int		n )							// �ǂݐi�߂�o�C�g��
{
	if( mBufP + n > mEndOfBufP )			// ���̎��_�ŃA�N�Z�X�G���[���m���Ȃ��
		throw( EBufferError( "SkipByte" ) );
	if( (mBufP += n) >= mEndOfBufP )		// ���̃A�N�Z�X�ŃG���[
		mReadFlag = 0;
	mBitPos = 7;
}

// ���̓ǂݏo���A�h���X
char*									// ���̃A�h���X
CInBitStream::GetNextAddress( void )
{
	if( mBitPos == 7 )					// �ǂݏo���r���łȂ��Ȃ�
		return (char*)mBufP;
	else if( mBufP < mEndOfBufP )
		return (char*)(mBufP + 1);
	else
		return NULL;
}