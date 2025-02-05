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
#include "Crate.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
Crate::Crate(const BackBuffer* pBackBuffer)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/crate.bmp", "data/crate_mask.bmp");

	m_pSprite->setBackBuffer(pBackBuffer);

	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite = new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 15);
	m_pExplosionSprite->setBackBuffer(pBackBuffer);
	m_bExplosion = false;
	m_iExplosionFrame = 0;

	bullet = new Sprite("data/crate.bmp", "data/crate_mask.bmp");
	bullet->setBackBuffer(pBackBuffer);
	m_bullet = false;

	bullet2 = new Sprite("data/bullet.bmp", "data/bullet_mask.bmp");
	bullet2->setBackBuffer(pBackBuffer);
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
Crate::~Crate()
{
	delete m_pSprite;
	delete bullet;
	delete bullet2;
	delete m_pExplosionSprite;
}

void Crate::Update(float dt)
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
	switch (m_eSpeedState)
	{
	case SPEED_STOP:
		if (v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if (v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if (m_fTimer > 1.f)
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

void Crate::Draw()
{
	if (m_bullet && !m_bExplosion) {
		bullet->draw();
		bullet2->draw();
	}

	if (!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void Crate::Move()
{
	int ok = 0;

	if (m_pSprite->mPosition.x > 0 && ok == 0)
	{
		m_pSprite->mVelocity.x = -300;
	}
	else {
		ok = 1; m_pSprite->mPosition.x = m_pSprite->mPosition.x + 2000;
	}




}

double Crate::getWidth()
{
	return m_pSprite->width();
}

Vec2& Crate::Position()
{
	return m_pSprite->mPosition;
}

void Crate::GeterPositionX(double& x)
{
	x = m_pSprite->mPosition.x;
}

void Crate::GeterPositionY(double& y)
{
	y = m_pSprite->mPosition.y;
}

Vec2& Crate::Velocity()
{
	return m_pSprite->mVelocity;
}

void Crate::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool Crate::AdvanceExplosion()
{
	if (m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if (m_iExplosionFrame == m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0, 0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

bool Crate::ifExploded()
{
	return m_bExplosion;
}
