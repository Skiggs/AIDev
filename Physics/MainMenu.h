#pragma once

#include<SFML/Graphics.hpp>
#include<vector>
#include"State.h"
#include"Game.h"
#include"WallData.h"
#include"Collision.h"
#include"Wall.h"
#include"Enemy.h"
#include"Player.h"
#include <SFML/Audio.hpp>
#include"Item.h"
#include "Node.h"
#include"Message.h"
#include"HandlerManager.h"
#include"MessageDispatcher.h"




enum class COLLISIONSIDE
{
	NONE,
	LEFT,
	RIGHT,
	UPWARDS,
	DOWNWARDS,
};



class MainMenuState : public State
{
public:
	MainMenuState(GameDataRef data);
	void Init();
	static bool checkEnemyToPlayer(int i);
	void HandleInput();
	void Update(float dt);
	void Draw(float dt);
	void LoadLevel();
	void DrawLines();
	void CheckCollisionWithWalls();
	void CheckDistWithDoors(int index = -1);
	bool CheckEnemiesToPlayerLine(int index);
	void MoveEnemies(float dt);
	void PlayerAttack(int i = -1);
	void RemoveDeadEnemies();
	void OpenDoorbyEnemy(int index,Player* plyr = nullptr);
	void ReadjustEnemyVector(Enemy* enemy);
	void CheckEnemyCloseToDoor();
	void FindDynamicWayPtForEnemy(int i,sf::Vector2f*);
	void CheckWhereIsEnemy(int i);
	static void SetEnemyDirection(int index, sf::Vector2f pos);
	static void MoveEnemy(int i);
	void FindEnemyByName(std::string, Item*);
	void LoadItemsInLevel(std::string fileName, ItemData *itemDetails);
	void CheckWhatIsNearToItems();
	static void PickItem(int index = -1);
	void ThrowItem(int index=-1);
	void MovePlayer();
	void GetDispatchedMessages();
	void MoveItems();
	void updateGridInsideRectangle();
	void UpdatePathofUpdatedMap(float dt);
	void MakeDirtyRect(float minX, float maxX, float minY, float maxY);
	bool seeingThroughWindow(int index);
	static bool isWeaponInDangerZone(int index, sf::Vector2f weaponPos);
	bool CheckEnemyToItems(int i,int j);
	void IncreaseTargetItemLine();
	void ReduceNextHitTime(float dt);
	void DecreaseTargetItemLine();
	void RemoveItems();
	void RunEnemiesState();
	void DecideToTakeCover(int index);
	bool CheckEnemyToPointIntersection(sf::Vector2f, sf::Vector2f);
	void ReadjustItemsList(Item *item);
	static std::pair<int,Item*> FindNearestWeapon(int index);
	void EnemyToReviveCalc(int index);
	Item* ItemsWithFlags(ItemTrait);
	static sf::Vector2f _lastSeenPositionOfPlayer;
	bool hasMapUpdated = false;

private:
	GameDataRef _data;
	COLLISIONSIDE m_side_collided;
	COLLISIONSIDE m_face_side;
	sf::Text Maintext;
	std::vector<WallWinDoorData> m_wallDataPool;
	std::vector<EnemyTraits> m_enemyDataPool;
	std::vector<ItemData> m_ItemDataPool;
	sf::CircleShape cir;
	static std::vector<Wall*> _walls;
	static std::vector<Window*> _windows;
	static std::vector<Door*> _doors;
	//static std::vector<RectWall*> _RectWalls;
	static std::vector<Enemy*> _enemies;
	std::vector<Enemy*> _Deadenemies;
	static std::vector<Item*> _items;
	float angle,degrees;
	
	sf::Vector2f pt1, pt2, pt3, pt4, pt5;
	sf::Vector2f closestPointToDoor, closestPointToDoorToEnemy, closestPointToWallToEnemy;
    Door* _doorCloseToPlayer, *_doorCloseToEnemy;
	static Player* player;
	sf::SoundBuffer buffer;
	sf::Sound knockSound;
	Collision collision;
	bool is_collided = false;
	bool CanDoorOpen = false;
	std::array<std::array<sf::Vector2f, 2>, 4> _linesToDraw;
	float rectanglePts[4];
	float oldrectanglePts[4];
	//PathFinder pathFinder;
	MessageDispatcher msgDisptch;
	HandlerManager handlerManager;
	
	static int GlobalIDCounter;

};