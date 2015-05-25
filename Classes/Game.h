#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"


#define BIRD_START_X				50
#define BIRD_START_Y				50
#define CORNFLOWER_BLUE				Color3B(100, 149, 237)
#define BIRD_ANIM					0.1f
#define MAP_SCROLL_SPEED			400
#define GRAVITY						-500
#define LIFT_FORCE					400
#define NUM_ACTIVE_PIPES			5


USING_NS_CC;


class Game : public LayerColor
{
private:
	// Global
	bool m_isGameOver;

	// Sprites
	Sprite* m_sprBird;
	Sprite* m_sprSkyline;
	Sprite* m_sprGround;
	Sprite* m_sprPipe[NUM_ACTIVE_PIPES];

	// Physics
	PhysicsWorld* m_world;

public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);

	void SetPhysicsWorld(PhysicsWorld* world);
	void InitBird();
	void InitGroundAndSkyline();
	void InitInput();
	void GeneratePipes(float dt);
};

#endif // __GAME_SCENE_H__
