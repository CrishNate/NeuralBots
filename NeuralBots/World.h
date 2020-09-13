#pragma once
#include "Drawing.h"
#include "Bot.h"
#include <time.h>

#define WORLD_SIZE_X 8000
#define WORLD_SIZE_Y 8000
#define WALLS_THICKNESS 50
#define GRID_SCALE 100

#define BOTS_COUNT 80
#define WALL_COUNT 0
#define MATCH_TIME_PER_GEN 60
#define RANDOM_BOTS_PER_GEN 10

class World
{
public:

	World();

	void LoadRoom();

	// Size
	float width() { return WORLD_SIZE_X; }
	float height() { return WORLD_SIZE_Y; }

	void AddPhysObj(PhysObj* pObj)
	{
		m_PhysObjs.push_back(pObj);
	}

	void AddDanger(PhysObj* pObj)
	{
		m_Dangers.push_back(pObj);
	}

	// Removing functions
	void Remove(PhysObj* pPhysObj)
	{
		for (int i = 0; i < m_PhysObjs.size(); i++)
			if (m_PhysObjs[i] == pPhysObj)
				m_PhysObjs.erase(m_PhysObjs.begin() + i);
		pPhysObj->Remove();
	}

	void Remove(Bot* pBot);
	void RemoveD(PhysObj* pDanger);
	void RemoveSavedBot(Bot* pBot);

	void Draw(const Camera& camera);
	void DrawGraph(int x, int y, int w, int h, int divs);

	void AddBot();
	void AddBot(Bot* pBot1, Bot* pBot2);
	void Step(float speed = 1.0f);

	bool CheckPointForSolid(const Vector2D& point, bool onlyStatic = true);
	bool CheckLineForCollision(const Vector2D& startpos, const Vector2D& endpos, bool onlyStatic, PhysObj* ignore = nullptr);
	bool CheckLineForCollision(const Vector2D& startpos, const Vector2D& endpos, bool onlyStatic, PhysObj** out, PhysObj* ignore = nullptr);
	const std::vector<Bot*>& GetBots() { return m_Bots; }
	const std::vector<PhysObj*>& GetDangers() { return m_Dangers; }
	const std::vector<Bot*>& GetSavedBots() { return m_SavedBots; }

private:

	bool Solve(PhysObj* pObj1, PhysObj* pObj2);
	bool HandlePhysObj(PhysObj* pObj);
	bool GetFitnessesBots(Bot*& pBot1, Bot*& pBot2);
	void ClearSavedNonAliveBots();

	// fields
	std::vector<Bot*> m_Bots;
	std::vector<Bot*> m_SavedBots;
	std::vector<PhysObj*> m_Dangers;
	std::vector<PhysObj*> m_PhysObjs;
	clock_t m_LastTime;
	float m_dTime;

	std::vector<float> m_GraphFitness;
	double m_GrapthMax;
	time_t m_NewGeneration;
};