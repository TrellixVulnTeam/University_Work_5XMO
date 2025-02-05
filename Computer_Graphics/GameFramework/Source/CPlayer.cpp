//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include <fstream>


//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer) :mBackBuffer(pBackBuffer)
, mFacingDirection(DIRECTION::DIR_FORWARD)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff,0x00, 0xff));
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 4);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;

	bullet = new Sprite("data/bullet.bmp", "data/bullet_mask.bmp");
	bullet->setBackBuffer(pBackBuffer);
	m_bullet = false;

	bullet2 = new Sprite("data/bullet.bmp", "data/bullet_mask.bmp");
	bullet2->setBackBuffer(pBackBuffer);
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete bullet;
	delete bullet2;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt)
{
	// Update sprite
	bullet->update(dt);
	bullet2->update(dt);
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{  
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if (m_bullet && !m_bExplosion) {
		bullet->draw();
		bullet2->draw();
	}
	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void CPlayer::Move(ULONG ulDirection)
{
	if (ulDirection & CPlayer::DIR_LEFT) 
		m_pSprite->mVelocity.x -= .1;
	if (m_pSprite->mPosition.x < 50 && m_pSprite->mPosition.x > 0) {
		m_pSprite->mVelocity.x = 0;
		m_pSprite->mPosition.x++;
	}

	

	if (ulDirection & CPlayer::DIR_RIGHT) 
		m_pSprite->mVelocity.x += .1;
	if (m_pSprite->mPosition.x > 730) {
		m_pSprite->mVelocity.x = 0;
		m_pSprite->mPosition.x--;
	}
	

	if (ulDirection & CPlayer::DIR_FORWARD) 
		m_pSprite->mVelocity.y -= .1;
	if (m_pSprite->mPosition.y < 70) {
		m_pSprite->mVelocity.y = 0;
		m_pSprite->mPosition.y++;
	}
	

	if (ulDirection & CPlayer::DIR_BACKWARD) 
		m_pSprite->mVelocity.y += .1;
	if (m_pSprite->mPosition.y > 500) {
		m_pSprite->mVelocity.y = 0;
		m_pSprite->mPosition.y--;
	}
	
}


Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

void CPlayer::GetPositionY(double& y)
{
	y = m_pSprite->mPosition.y;
}

void CPlayer::GetPositionX(double& x)
{
	x = m_pSprite->mPosition.x;
}

void CPlayer::SetPositionY(double y)
{
	m_pSprite->mPosition.y = y;
}

void CPlayer::SetPositionX(double x)
{
	m_pSprite->mPosition.x = x;
}

int CPlayer::getWidth()
{
	return m_pSprite->width();
}

void CPlayer::Stop()
{
	m_pSprite->mVelocity = Vec2(0, 0);
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

void CPlayer::Shoot() {
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	if (bullet->mPosition.y < desktop.top) {
		bullet->mPosition.x = m_pSprite->mPosition.x - 20;
		bullet->mPosition.y = m_pSprite->mPosition.y - 30;

		bullet2->mPosition.x = m_pSprite->mPosition.x + 20;
		bullet2->mPosition.y = m_pSprite->mPosition.y - 30;

	}
	bullet->mVelocity.y = -1000;
	bullet2->mVelocity.y = -1000;
	m_bullet = true;
}

void CPlayer::RotateLeft()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/leftPlaneImg.bmp", "data/leftPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_LEFT;
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/rightPlaneImg.bmp", "data/rightPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_RIGHT;
		break;
	case DIRECTION::DIR_LEFT:
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		m_pSprite = new Sprite("data/downPlaneImg.bmp", "data/downPlaneMask.bmp");
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/upPlaneImg.bmp", "data/upPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_FORWARD;
		break;
	}
	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}

void CPlayer::RotateRight()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	delete m_pSprite;

	switch (mFacingDirection)
	{
	case DIRECTION::DIR_FORWARD:
		m_pSprite = new Sprite("data/rightPlaneImg.bmp", "data/rightPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_RIGHT;
		break;
	case DIRECTION::DIR_BACKWARD:
		m_pSprite = new Sprite("data/leftPlaneImg.bmp", "data/leftPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_LEFT;
		break;
	case DIRECTION::DIR_LEFT:
		m_pSprite = new Sprite("data/upPlaneImg.bmp", "data/upPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_FORWARD;
		break;
	case DIRECTION::DIR_RIGHT:
		m_pSprite = new Sprite("data/downPlaneImg.bmp", "data/downPlaneMask.bmp");
		mFacingDirection = DIRECTION::DIR_BACKWARD;
		break;
	}

	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(mBackBuffer);
}

Vec2& CPlayer::PositionBullet()
{
	return bullet->mPosition;
}

int CPlayer::getWidthBullet()
{
	return 20;
}

bool CPlayer::ifExploded()
{
	return m_bExplosion;
}
