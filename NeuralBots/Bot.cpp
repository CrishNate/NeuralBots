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
	int shoot = 0;
	GetAngleOffsetToNearestEnemy(pWorld, sideE, shoot);
	GetAngleOffsetToNearestDanger(pWorld, sideD);

	int hitR = pWorld->CheckPointForSolid(m_Position + Vector2D(m_Width / 2 + 2, 0)) ? 1 : 0;
	int hitL = pWorld->CheckPointForSolid(m_Position - Vector2D(m_Width / 2 + 2, 0)) ? 1 : 0;

	inputs.push_back(sideE);
	inputs.push_back(sideD);
	inputs.push_back(hitR);
	inputs.push_back(hitL);
	inputs.push_back(shoot);

	std::vector<float> outputs = m_pBrain->Run(inputs);

	m_ViewAngle += outputs[1] / 10.0f * dTime / 10.0f;
	if (m_ViewAngle > M_2PI) { m_ViewAngle -= M_2PI; }
	if (m_ViewAngle < -M_2PI) { m_ViewAngle += M_2PI; }

	ApplyForce(Vector2D(outputs[0] * dTime / 100.0f, 0));

	// Jump
	if (outputs[2] > 0.9)
	{
		if (hitR)
			SetVelocity(Vector2D(-40, -70));
		else if (hitL)
			SetVelocity(Vector2D(40, -70));
		else if (pWorld->CheckPointForSolid(m_Position + Vector2D(0, m_Height / 2 + 2)))
			ApplyForce(Vector2D(0, -70));
	}

	// Shooting
	if (outputs[3] > 0.8 && !m_ShootCooldown)
	{
		Vector2D dir = GetLookDir();
		Bullet* pBullet = new Bullet(m_Position, dir);
		pBullet->SetOwner(this);
		pWorld->AddPhysObj(pBullet);
		pWorld->AddDanger(pBullet);

		m_ShootCooldown = 100;
	}

	// Reflection
	if (outputs[4] > 0.8 && !m_ReflectionCooldown)
	{
		for (PhysObj* pObj : pWorld->GetDangers())
		{
			Vector2D refl = pObj->GetPosition() - m_Position;
			if (refl.Len() < 100)
			{
				refl.Normalize();
				refl *= fmax(100, pObj->GetVelocity().Len());

				pObj->SetVelocity(refl);
			}
		}

		m_ReflectAnim = 20;
		m_ReflectionCooldown = 100;
	}

	// Countdown
	if (m_ShootCooldown > 0) m_ShootCooldown -= dTime / 10.0f;
	else m_ShootCooldown = 0;

	if (m_ReflectionCooldown > 0) m_ReflectionCooldown -= dTime / 10.0f;
	else m_ReflectionCooldown = 0;

	if (m_ReflectAnim > 0)
	{
		m_ReflectAnim += dTime / 1.0f;
		if (m_ReflectAnim > 100)
			m_ReflectAnim = 0;
	}
}

void Bot::Draw(const Camera& camera, float dTime)
{
	m_Anim += dTime * m_Velocity.x / 50.0f * dTime;
	if (m_Anim > M_2PI) m_Anim -= M_2PI;
	if (m_Anim < -M_2PI) m_Anim += M_2PI;

	// Shield
	if (m_ReflectAnim)
	{
		RGBColor color = m_Color;
		color.a = 255 - m_ReflectAnim * 2.55;
		DrawOutlinePartcircle(m_Position.x, m_Position.y, 50, M_PI / 3.0f, m_ViewAngle, 5.0f, color, camera);
	}

	if (m_pBrain->GetGeneration() > 0)
	{
		DrawFilledRectC(m_Position.x, m_Position.y - LEG_HEIGHT - BODY_SCALE / 2, 4, BODY_SCALE / 2, RGBColor(100, 100, 150), camera);
		DrawOutlineCircle(m_Position.x, m_Position.y - LEG_HEIGHT - BODY_SCALE, 5, m_Color, camera);

		for (int i = 1; i < m_pBrain->GetGeneration() / 1.0f; i++)
		{
			DrawOutlinePartcircle(m_Position.x, m_Position.y - LEG_HEIGHT - BODY_SCALE, 5 + i * 2, M_PI / 3.0f, -M_PI / 2.0f, 1.0f, m_Color, camera);
		}
	}

	// Body
	DrawFilledRectC(m_Position.x, m_Position.y - LEG_HEIGHT, BODY_SCALE, BODY_SCALE, RGBColor(100, 100, 150), camera);

	// Eye
	Vector2D offset = Vector2D(10, 0).GetRotated(m_ViewAngle);
	DrawFilledRectC(m_Position.x + offset.x, m_Position.y - LEG_HEIGHT - 5, 1.5f * EYE_SCALE + 5, EYE_SCALE + 5, RGBColor(100, 100, 150) / 2, camera);
	DrawFilledRectC(m_Position.x + offset.x, m_Position.y - LEG_HEIGHT - 5, 1.5f * EYE_SCALE, EYE_SCALE, m_Color, camera);

	// Foots
	Vector2D offsetL = Vector2D(-cos(m_Anim) * 10 * m_Side, -fmax(0, sin(m_Anim) * 5)) * fmin(1, abs(m_Velocity.x / 5.0f));
	Vector2D offsetR = Vector2D(cos(m_Anim) * 10 * m_Side, -fmax(0, -sin(m_Anim) * 5)) * fmin(1, abs(m_Velocity.x / 5.0f));

	DrawFilledRectC(m_Position.x + offsetL.x, m_Position.y + BODY_SCALE / 2 + LEG_HEIGHT + offsetL.y, 1.5f * FOOT_WIDTH, 6, RGBColor(80, 80, 120), camera);
	DrawFilledRectC(m_Position.x + offsetR.x, m_Position.y + BODY_SCALE / 2 + LEG_HEIGHT + offsetR.y, 1.5f * FOOT_WIDTH, 6, RGBColor(80, 80, 120), camera);

	// Hand
	Vector2D handpos = Vector2D(25, 0).GetRotated(m_ViewAngle);
	DrawFilledRectC(m_Position.x + handpos.x, m_Position.y + handpos.y, HAND_SCALE, HAND_SCALE, RGBColor(80, 80, 120), camera);
	DrawFilledRectC(m_Position.x + handpos.x, m_Position.y + handpos.y, HAND_SCALE / 4.0f, HAND_SCALE / 4.0f, m_Color, camera);

	// eye dir
	offset = Vector2D(100, 0).GetRotated(m_ViewAngle);
	DrawLineThinkT(
		m_Position.x + offset.x / 1.5,
		m_Position.y - LEG_HEIGHT + offset.y / 1.5,
		m_Position.x + offset.x,
		m_Position.y - LEG_HEIGHT + offset.y, 5, 0,
		RGBColor(255, 255, 255), camera);
}

float Bot::GetAngleOffsetToNearestEnemy(World* pWorld, int& side, int& shoot)
{
	float dist = -1;
	Bot* pBot = NULL;

	for (Bot* pB : pWorld->GetBots())
	{
		if (pB == this)
			continue;

		if (pWorld->CheckLineForCollision(m_Position, pB->GetPosition()))
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
		shoot = (abs(ang) < (M_PI / 40.0f)) ? 1 : 0;

		//if (ang > 0)
		//if (shoot)
			//DrawLineThink(m_Position.x, m_Position.y, m_Position.x + dir.x, m_Position.y + dir.y, 1, RGBColor(0, 0, 255), g_Camera);
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
		if (Bullet* pBull = dynamic_cast<Bullet*>(pD))
			if (pBull->GetOwner() == this)
				continue;

		if ((dist == -1 || dist > m_Position.Distance(pD->GetPosition()))
			&& (pD->GetPosition() + pD->GetVelocity().GetNormalize() * fmin(250, pD->GetVelocity().Len() * 10)).Distance(m_Position) < 400)
		{
			dist = m_Position.Distance(pD->GetPosition());
			pDanger = pD;
		}
	}

	if (!pDanger)
		return false;

	if (dist < 150)
	{
		side = 1;

		//DrawLineThink(m_Position.x, m_Position.y, pDanger->GetPosition().x, pDanger->GetPosition().y, 1, RGBColor(255, 0, 0), g_Camera);
	}

	return true;
}