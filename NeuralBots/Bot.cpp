#include "Bot.h"
#include "World.h"

extern Camera g_Camera;

void Bot::Step(World* pWorld, float dTime)
{
	std::vector<float> inputs = {
		frand(1, -1),
		1,
		-1,
		static_cast<float>(cos(clock() / 400.0f)),
		static_cast<float>(sin(clock() / 600.0f))
	};

	int sideE = 0;
	int	sideD = 0;
	GetAngleOffsetToNearestEnemy(pWorld, sideE);
	GetAngleOffsetToNearestDanger(pWorld, sideD);

	int hitR = pWorld->CheckPointForSolid(m_Position + Vector2D(m_Width / 2 + 10, 0)) ? 1 : 0;
	int hitL = pWorld->CheckPointForSolid(m_Position - Vector2D(m_Width / 2 + 10, 0)) ? 1 : 0;

	inputs.push_back(sideE);
	inputs.push_back(sideD);
	inputs.push_back(hitR);
	inputs.push_back(hitL);

	std::vector<float> outputs = m_pBrain->Run(inputs);

	m_ViewAngle += outputs[1] / 10.0f * dTime / 10.0f;
	if (m_ViewAngle > M_2PI) { m_ViewAngle -= M_2PI; }
	if (m_ViewAngle < -M_2PI) { m_ViewAngle += M_2PI; }

	ApplyForce(Vector2D(outputs[0] * dTime / 100.0f, 0));

	if (outputs[2] > 0.9)
	{
		if (pWorld->CheckPointForSolid(m_Position + Vector2D(0, m_Height / 2 + 2)))
			SetVelocity(Vector2D(m_Velocity.x, -40));
	}

	if (outputs[3] > 0.8 && !m_ShootCooldown)
	{
		Vector2D dir = GetLookDir();
		Bullet* pBullet = new Bullet(m_Position + dir * 50, dir);
		pWorld->AddPhysObj(pBullet);
		pWorld->AddDanger(pBullet);

		m_ShootCooldown = 100;
	}

	if (m_ShootCooldown > 0)
		m_ShootCooldown -= dTime / 10.0f;
	else
		m_ShootCooldown = 0;
}

float Bot::GetAngleOffsetToNearestEnemy(World* pWorld, int& side)
{
	float dist = -1;
	Bot* pBot = NULL;

	for (Bot* pB : pWorld->GetBots())
	{
		if (pB == this)
			continue;

		if (dist == -1 || dist > m_Position.Distance(pB->GetPosition()))
		{
			dist = m_Position.Distance(pB->GetPosition());
			pBot = pB;
		}
	}

	if (!pBot)
		return false;

	Vector2D dirF = Vector2D(1, 0).GetRotated(m_ViewAngle);
	Vector2D dir = (pBot->GetPosition() - m_Position);
	float ang1 = atan2(dirF.y, dirF.x);
	float ang = ang1 - atan2(dir.y, dir.x);

	if (dist && ang > -M_PI * 0.5f && ang < M_PI * 0.5f)
	{
		side = ang > 0 ? 1 : -1;

		//if (ang > 0)
		//	DrawLineThink(m_Position.x, m_Position.y, m_Position.x + dir.x, m_Position.y + dir.y, 1, RGBColor(0, 0, 255), g_Camera);
		//else
		//	DrawLineThink(m_Position.x, m_Position.y, m_Position.x + dir.x, m_Position.y + dir.y, 1, RGBColor(255, 0, 0), g_Camera);
	}

	return true;
}


float Bot::GetAngleOffsetToNearestDanger(World* pWorld, int& side)
{
	float dist = -1;
	PhysObj* pDanger = NULL;

	for (PhysObj* pD : pWorld->GetDangers())
	{
		if ((dist == -1 || dist > m_Position.Distance(pD->GetPosition()))
			&& (pD->GetPosition() + pD->GetVelocity().GetNormalize() * fmin(250, pD->GetVelocity().Len() * 10)).Distance(m_Position) < 400)
		{
			dist = m_Position.Distance(pD->GetPosition());
			pDanger = pD;
		}
	}

	if (!pDanger)
		return false;

	if (dist < 200)
	{
		side = 1;
	}

	return true;
}