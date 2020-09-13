#include "World.h"
#include "Bullet.h"

#include <ostream>


World::World()
	: m_dTime(1.0f)
	, m_GrapthMax(5)
	, m_LastTime(clock())
{
	m_PhysObjs.clear();

	// creating border walls
	PhysObj* pPhys = new PhysObj(Vector2D(WORLD_SIZE_X / 2, 0), WORLD_SIZE_X, WALLS_THICKNESS);
	pPhys->SetStatic();
	AddPhysObj(pPhys);

	pPhys = new PhysObj(Vector2D(0, WORLD_SIZE_Y / 2), WALLS_THICKNESS, WORLD_SIZE_Y);
	pPhys->SetStatic();
	AddPhysObj(pPhys);

	pPhys = new PhysObj(Vector2D(WORLD_SIZE_X / 2, WORLD_SIZE_Y), WORLD_SIZE_X, WALLS_THICKNESS);
	pPhys->SetStatic();
	AddPhysObj(pPhys);

	pPhys = new PhysObj(Vector2D(WORLD_SIZE_X, WORLD_SIZE_Y / 2), WALLS_THICKNESS, WORLD_SIZE_Y);
	pPhys->SetStatic();
	AddPhysObj(pPhys);

	LoadRoom();

	m_NewGeneration = time(nullptr) + MATCH_TIME_PER_GEN;
}

void World::LoadRoom()
{
	for (int i = 0; i < WALL_COUNT; i++)
	{
		float w = round(frand(GRID_SCALE, 500) / GRID_SCALE) * GRID_SCALE;
		float h = round(frand(GRID_SCALE, 500) / GRID_SCALE) * GRID_SCALE;

		PhysObj* pPhys = new PhysObj(
			Vector2D(
			round(frand(0, WORLD_SIZE_X) / GRID_SCALE) * GRID_SCALE + w / 2,
			round(frand(0, WORLD_SIZE_Y) / GRID_SCALE) * GRID_SCALE + h / 2),
			w, h);

		assert(pPhys);
		pPhys->SetStatic();
		AddPhysObj(pPhys);
	}

	for (int i = 0; i < BOTS_COUNT; ++i)
	{
		AddBot();
	}
}


void World::DrawGraph(int x, int y, int w, int h, int divs)
{
	DrawFilledRect(x, y, w, h, RGBColor(110, 130, 150));
	int px = 0;
	int py = 0;
	for (size_t i = std::max(0, (int)m_GraphFitness.size() - divs); i < m_GraphFitness.size(); i++)
	{
		if (m_GrapthMax < m_GraphFitness[i])
			m_GrapthMax = m_GraphFitness[i];

		int x1 = x + i * w / divs;
		int y1 = y + h - m_GraphFitness[i] / m_GrapthMax * h;
		if (i > 0)
			DrawLineThink(x1, y1, px, py, 4, RGBColor(0, 200, 0));

		px = x1;
		py = y1;
	}
}


void World::Draw(const Camera& camera)
{
	// Drawing Objects
	for (PhysObj* pPhysObj : m_PhysObjs)
		pPhysObj->Draw(camera, m_dTime);
}


bool World::CheckPointForSolid(const Vector2D& point, bool onlyStatic)
{
	for (PhysObj* pPhysObj : m_PhysObjs)
	{
		if (onlyStatic && !pPhysObj->IsStatic())
			continue;

		Vector2D p = pPhysObj->GetPosition();
		Vector2D s = Vector2D( pPhysObj->GetWidth(), pPhysObj->GetHeight() ) / 2.0f;

		if (point.x > (p.x - s.x) && point.x < (p.x + s.x)
			&& point.y > (p.y - s.y) && point.y < (p.y + s.y))
			return true;
	}
	return false;
}

// LINE/LINE
bool lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* intersectionX = NULL, float* intersectionY = NULL) {

	// calculate the direction of the lines
	float uA = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uB = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 
		&& uB >= 0 && uB <= 1) 
	{
		// optionally, draw a circle where the lines meet
		*intersectionX = x1 + (uA * (x2 - x1));
		*intersectionY = y1 + (uA * (y2 - y1));
		return true;
	}
	return false;
}

// LINE/RECTANGLE
bool lineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh, float* intersectionX = NULL, float* intersectionY = NULL) {

	// check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below
	if (lineLine(x1, y1, x2, y2, rx, ry, rx, ry + rh, intersectionX, intersectionY))
		return true;

	if (lineLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh, intersectionX, intersectionY))
		return true;

	if (lineLine(x1, y1, x2, y2, rx, ry, rx + rw, ry, intersectionX, intersectionY))
		return true;

	if (lineLine(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh, intersectionX, intersectionY))
		return true;

	return false;
}


bool World::CheckLineForCollision(const Vector2D& startpos, const Vector2D& endpos, bool onlyStatic, PhysObj* ignore)
{
	float length = -1;

	for (PhysObj* pPhysObj : m_PhysObjs)
	{
		if (pPhysObj == ignore)
			continue;
		if (onlyStatic && !pPhysObj->IsStatic())
			continue;

		Vector2D pos = pPhysObj->GetPosition();
		Vector2D size = Vector2D(pPhysObj->GetWidth(), pPhysObj->GetHeight());
	}
	return false;
}

bool World::CheckLineForCollision(const Vector2D& startpos, const Vector2D& endpos, bool onlyStatic, PhysObj** out, PhysObj* ignore)
{
	float length = -1;
	bool hit = false;
	
	for (PhysObj* pPhysObj : m_PhysObjs)
	{
		if (pPhysObj == ignore)
			continue;
		if (onlyStatic && !pPhysObj->IsStatic())
			continue;

		Vector2D pos = pPhysObj->GetPosition();
		Vector2D size = Vector2D(pPhysObj->GetWidth(), pPhysObj->GetHeight());
		float ix, iy;
		if (lineRect(startpos.x, startpos.y, endpos.x, endpos.y, pos.x - size.x / 2, pos.y - size.y / 2, size.x, size.y, &ix, &iy))
		{
			if (length > Dist(Vector2D(ix, iy), startpos) || length < 0)
			{
				length = Dist(Vector2D(ix, iy), startpos);
				*out = pPhysObj;
			}

			hit = true;
		}
	}
	
	return hit;
}

bool World::Solve(PhysObj* pObj1, PhysObj* pObj2)
{
	// Skipping if both of objects is static
	if (pObj1->IsStatic() && pObj2->IsStatic())
		return false;

	if (Bullet* pBull = dynamic_cast<Bullet*>(pObj1))
	{
		if (Bot* pBot = dynamic_cast<Bot*>(pObj2))
			if (pBot == pBull->GetOwner())
				return false;
	}
	else if (Bullet* pBull = dynamic_cast<Bullet*>(pObj2))
	{
		if (Bot* pBot = dynamic_cast<Bot*>(pObj1))
			if (pBot == pBull->GetOwner())
				return false;
	}

	Vector2D n = pObj1->GetPosition() - pObj2->GetPosition();

	// Calculate overlap on x axis
	float x_overlap = (pObj1->GetWidth() + pObj2->GetWidth()) * 0.5f - abs(n.x);

	Vector2D penetration = Vector2D(0, 0);

	// SAT test on x axis
	if (x_overlap > 0)
	{
		// Calculate overlap on y axis
		float y_overlap = (pObj1->GetHeight() + pObj2->GetHeight()) * 0.5f - abs(n.y);

		// SAT test on y axis
		if (y_overlap > 0)
		{
			// Find out which axis is axis of least penetration
			if (x_overlap < y_overlap)
			{
				// Point towards B knowing that n points from A to B
				char dir = n.x < 0 ? -1 : 1;
				//x_overlap -= 0.01;
				penetration = Vector2D(x_overlap * dir, 0);
			}
			else
			{
				// Point toward B knowing that n points from A to B
				char dir = n.y < 0 ? -1 : 1;
				//y_overlap -= 0.01;
				penetration = Vector2D(0, y_overlap * dir);
			}
		}
	}

	if (penetration == Vector2D(0, 0))
		return false;

	Vector2D dir = penetration.GetNormalize();

	if (!pObj1->IsStatic())
	{
		Vector2D pVel = pObj1->GetVelocity();
		pObj1->SetPosition(pObj1->GetPosition() + penetration);
		pObj1->SetVelocity(pVel);

		// friction
		pObj1->SetVelocity(Vector2D(pObj1->GetVelocity().x * 0.9f, pObj1->GetVelocity().y));
	}

	if (!pObj2->IsStatic())
	{
		Vector2D pVel = pObj2->GetVelocity();
		pObj2->SetPosition(pObj2->GetPosition() - penetration);
		pObj2->SetVelocity(pVel);

		// friction
		pObj1->SetVelocity(Vector2D(pObj1->GetVelocity().x * 0.9f, pObj1->GetVelocity().y));
	}

	return true;
}


bool World::HandlePhysObj(PhysObj* pObj)
{
	// Removing objects that is out of bounds
	Vector2D pos = pObj->GetPosition();
	if (pos.x < 0 || pos.x > WORLD_SIZE_X
		|| pos.y < 0 || pos.y > WORLD_SIZE_Y)
	{
		if (Bullet* pB = dynamic_cast<Bullet*>(pObj))
			RemoveD(pB);
		else if (Bot* pB = dynamic_cast<Bot*>(pObj))
			Remove(pB);
		else
			Remove(pObj);

		return false;
	}

	return true;
}


// Getting two best bots from simulation by their fitness
bool World::GetFitnessesBots(Bot*& pBot1, Bot*& pBot2)
{
	double fitness1 = -1;
	double fitness2 = -1;
	for (Bot* pBot : m_SavedBots)
	{
		if (pBot->getBrain()->getFitness() > fitness1)
		{
			fitness1 = pBot->getBrain()->getFitness();
			pBot1 = pBot;
		}
	}

	for (Bot* pBot : m_SavedBots)
	{
		if (pBot->getBrain()->getFitness() > fitness2 
			&& pBot != pBot1)
		{
			fitness2 = pBot->getBrain()->getFitness();
			pBot2 = pBot;
		}
	}

	if (pBot1 == NULL || pBot2 == NULL)
		return false;

	return true;
}


// Clear saved bots
void World::ClearSavedNonAliveBots()
{
	for (int i = 0; i < m_SavedBots.size(); i++)
	{
		Bot* pBot = m_SavedBots[i];

		if (std::find(m_Bots.begin(), m_Bots.end(), pBot) == m_Bots.end())
		{
			pBot->Remove();
			m_SavedBots.erase(m_SavedBots.begin() + i);
			i--;
		}
	}
}


void World::Step(float speed)
{
	float m_dTime = (clock() - m_LastTime) * speed;
	m_LastTime = clock();

	if (time(nullptr) > m_NewGeneration || m_Bots.size() == 0)
	{
		m_NewGeneration = time(nullptr) + MATCH_TIME_PER_GEN;
		Bot* pBot1;
		Bot* pBot2;
		if (GetFitnessesBots(pBot1, pBot2))
		{
			// graph info
			m_GraphFitness.push_back(pBot1->getBrain()->getFitness());
			if (m_GraphFitness.size() > 100)
				m_GraphFitness.erase(m_GraphFitness.begin());
			
			for (int i = m_SavedBots.size() - 1; i > 0; i--)
			{
				if (pBot1 != m_SavedBots[i] &&
					pBot2 != m_SavedBots[i])
				{
					Remove(m_SavedBots[i]);
					RemoveSavedBot(m_SavedBots[i]);
				}
			}
			
			for (int i = 0; i < BOTS_COUNT - RANDOM_BOTS_PER_GEN; ++i)
			{
				AddBot(pBot1, pBot2);
			}

			for (int i = 0; i < RANDOM_BOTS_PER_GEN; ++i)
			{
				AddBot();
			}
			
			Remove(pBot1);
			RemoveSavedBot(pBot1);
			Remove(pBot2);
			RemoveSavedBot(pBot2);
			
			std::cout << "Reproduced bot created\n";
		}
	}

	// Handling Cell Creature
	for (Bot* pBot : m_Bots)
	{
		pBot->Step(this, m_dTime);
	}

	// Solving Phys Objects
	for (int i = m_PhysObjs.size() - 1; i > 0; i--)
	{
		PhysObj* pObj1 = m_PhysObjs[i];
		pObj1->RunPhys(m_dTime);

		if (!HandlePhysObj(pObj1))
			continue;

		for (int j = m_PhysObjs.size() - 1; j > i; j--)
		{
			PhysObj* pObj2 = m_PhysObjs[j];
			assert(pObj1 && pObj2);
			
			Solve(pObj1, pObj2);
		}
	}
}


void World::AddBot()
{
	while (true)
	{
		Vector2D pos = Vector2D((0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_X, (0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_Y);
		if (!CheckPointForSolid(pos, true))
		{
			Bot* pBot = new Bot(pos);
			m_Bots.push_back(pBot);
			m_PhysObjs.push_back(pBot);
			m_SavedBots.push_back(pBot);
			break;
		}
	}
}


void World::AddBot(Bot* pBot1, Bot* pBot2)
{
	while (true)
	{
		Vector2D pos = Vector2D((0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_X, (0.5 + frand(-0.5, 0.5)) * WORLD_SIZE_Y);
		if (!CheckPointForSolid(pos, true))
		{
			Bot* pBot = new Bot(pos, pBot1, pBot2);
			m_Bots.push_back(pBot);
			m_PhysObjs.push_back(pBot);
			m_SavedBots.push_back(pBot);
			break;
		}
	}
}

void World::Remove(Bot* pBot)
{
	if (find(m_PhysObjs.begin(), m_PhysObjs.end(), pBot) != m_PhysObjs.end())
		m_PhysObjs.erase(std::remove(m_PhysObjs.begin(), m_PhysObjs.end(), pBot), m_PhysObjs.end());

	if (find(m_Bots.begin(), m_Bots.end(), pBot) != m_Bots.end())
		m_Bots.erase(std::remove(m_Bots.begin(), m_Bots.end(), pBot), m_Bots.end());
	//pBot->Remove();
}


void World::RemoveSavedBot(Bot* pBot)
{
	if (pBot == nullptr) return;
	
	if (find(m_SavedBots.begin(), m_SavedBots.end(), pBot) != m_SavedBots.end())
		m_SavedBots.erase(std::remove(m_SavedBots.begin(), m_SavedBots.end(), pBot), m_SavedBots.end());
	pBot->Remove();
}


void World::RemoveD(PhysObj* pDanger)
{
	m_PhysObjs.erase(std::remove(m_PhysObjs.begin(), m_PhysObjs.end(), pDanger), m_PhysObjs.end());
	m_Dangers.erase(std::remove(m_Dangers.begin(), m_Dangers.end(), pDanger), m_Dangers.end());
	pDanger->Remove();
}