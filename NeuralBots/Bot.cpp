#include "Bot.h"
#include "World.h"
#include "Bullet.h"

#define BODY_SCALE 30
#define EYE_SCALE 5
#define BOT_VISION_LENGTH 1000

extern Camera g_Camera;

void Bot::Shoot(World* pWorld)
{
	if (!m_ShootCooldown > 0)
	{
		PhysObj* obj = nullptr;
		if (pWorld->CheckLineForCollision(m_Position, m_Position + Vector2D(0, getShootLength()).GetRotated(m_ViewAngle), false, &obj, this))
		{
			if (Bot* bot = dynamic_cast<Bot*>(obj))
			{
				getBrain()->addFitness(1); // add points for kill
				bot->respawn(pWorld);
			}
		}
		
		
		m_ShootCooldown = getShootCooldown();
	}
}

void Bot::respawn(World* pWorld)
{
	Vector2D pos;
	do
	{
		pos = Vector2D((0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_X, (0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_Y);
	}
	while (pWorld->CheckPointForSolid(pos));
	SetPosition(pos);
	getBrain()->addFitness(-0.25); // remove fitness per death
	
	m_ShootCooldown = 0;
}

void Bot::Step(World* pWorld, float dTime)
{
	float side = 0;
	float sideEnemy = 0;
	float dist = 0;
	GetAngleOffsetToNearestEnemy(pWorld, side, dist, sideEnemy);
	m_Inputs[0] = side;
	m_Inputs[1] = dist;
	m_Inputs[2] = sideEnemy;

	m_Inputs[3] = m_ShootCooldown > 0 ? 0.0f : 1.0f;

	arma::mat inputsMat = arma::mat(m_Inputs);

	m_pBrain->query(inputsMat);
	m_Velocity = getLookDir() * m_pBrain->result()[0] * dTime + getRight() * m_pBrain->result()[1] * dTime;
	m_ViewAngle += m_pBrain->result()[2] * dTime / 100.0f;

	if (m_pBrain->result()[3] > 0.5f)
		Shoot(pWorld);

	if (m_ShootCooldown > 0.0f)
		m_ShootCooldown -= dTime / 10.0f;
	else
		m_ShootCooldown = 0.0f;
}

void Bot::Remove()
{
	PhysObj::Remove();
}

double Bot::getShootLength()
{
	return m_pGenome->getGenome()[0] * 2000;
}

double Bot::getShootCooldown()
{
	return m_pGenome->getGenome()[0] * 200;
}

void Bot::Draw(const Camera& camera, float dTime)
{
	// Body
	DrawFilledRectRC(m_Position.x, m_Position.y, BODY_SCALE, BODY_SCALE, m_ViewAngle, RGBColor(100, 100, 150), camera);

	// Eye
	Vector2D p1 = m_Position + Vector2D(7.0f, (BODY_SCALE - EYE_SCALE) / 2.0f).GetRotated(m_ViewAngle);
	Vector2D p2 = m_Position + Vector2D(-7.0f, (BODY_SCALE - EYE_SCALE) / 2.0f).GetRotated(m_ViewAngle);
	DrawFilledRectRC(p1.x, p1.y, 1.5f * EYE_SCALE, EYE_SCALE, m_ViewAngle, m_Color, camera);
	DrawFilledRectRC(p2.x, p2.y, 1.5f * EYE_SCALE, EYE_SCALE, m_ViewAngle, m_Color, camera);

	// eye dir
	Vector2D offset = Vector2D(0, 100).GetRotated(m_ViewAngle);
	DrawLineThinkT(
		m_Position.x + offset.x / 1.5,
		m_Position.y + offset.y / 1.5,
		m_Position.x + offset.x,
		m_Position.y + offset.y, 5, 0,
		RGBColor(255, 255, 255), camera);

	if (m_ShootCooldown > getShootCooldown() - 5.0f)
	{
		Vector2D dir = Vector2D(0, getShootLength()).GetRotated(m_ViewAngle);
		DrawLineThinkT(
        m_Position.x,
        m_Position.y,
        m_Position.x + dir.x,
        m_Position.y + dir.y, 3,
        RGBColor(255, 0, 0), camera);
	}
}


float Bot::GetAngleOffsetToNearestEnemy(World* pWorld, float& side, float& distT, float& enemyVision)
{
	float dist = -1;
	Bot* pBot = NULL;

	for (Bot* pB : pWorld->GetBots())
	{
		if (pB == this)
			continue;

		if (pWorld->CheckLineForCollision(m_Position, pB->GetPosition(), true))
			continue;

		Vector2D dirF = getLookDir();
		Vector2D dir = (pB->GetPosition() - m_Position);
		float ang1 = atan2(dirF.y, dirF.x);
		float ang = ang1 - atan2(dir.y, dir.x);

		if (ang > -M_PI2 && ang < M_PI2 
			&& (dist < 0 || dist > m_Position.Distance(pB->GetPosition()))
			&& m_Position.Distance(pB->GetPosition()) < BOT_VISION_LENGTH)
		{
			dist = m_Position.Distance(pB->GetPosition());
			distT = dist / BOT_VISION_LENGTH;
			side = ang;
			pBot = pB;
		}
	}

	if (!pBot)
	{
		dist = 0;
		return false;
	}

	Vector2D dirF = pBot->getLookDir();
	Vector2D dir = (m_Position - pBot->GetPosition());
	float ang = atan2(dirF.y, dirF.x);
	enemyVision = ang <= M_PI2 ? (ang - atan2(dir.y, dir.x)) / M_PI2 : 0.0f;

	return true;
}


float Bot::GetAngleOffsetToNearestDanger(World* pWorld, float& side)
{
	float dist = -1;
	PhysObj* pDanger = NULL;

	for (PhysObj* pD : pWorld->GetDangers())
	{
		if (Bullet* pBull = dynamic_cast<Bullet*>(pD))
			if (pBull->GetOwner() == this)
				continue;

		Vector2D dirF = getLookDir();
		Vector2D dir = (pD->GetPosition() - m_Position);
		float ang1 = atan2(dirF.y, dirF.x);
		float ang = ang1 - atan2(dir.y, dir.x);

		if (ang > -M_PI2 && ang < M_PI2 
			&& (dist < 0 || dist > m_Position.Distance(pD->GetPosition()) && dist < 150)
			&& (pD->GetPosition() + pD->GetVelocity().GetNormalize() * fmin(250, pD->GetVelocity().Len() * 10)).Distance(m_Position) < 400)
		{
			side = ang / M_PI2;
			dist = m_Position.Distance(pD->GetPosition());
			pDanger = pD;
		}
	}

	if (!pDanger)
	{
		dist = 0;
		return false;
	}

	return true;
}