#include "Game.h"


Scene* Game::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();

#if DEBUG_PHYSICS
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif // DEBUG_PHYSICS
    
    // 'layer' is an autorelease object
    auto layer = Game::create();
	layer->setColor(CORNFLOWER_BLUE);

    // add layer as a child to scene
    scene->addChild(layer);

	// Init physics
	layer->SetPhysicsWorld(scene->getPhysicsWorld());

    // return the scene
    return scene;
}


void Game::SetPhysicsWorld(PhysicsWorld* world) 
{ 
	m_world = world; 

	// Gravity
	m_world->setGravity(Vec2(0, GRAVITY));
}


// on "init" you need to initialize your instance
bool Game::init()
{
    //////////////////////////////
    // 1. super init first
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255)))
    {
        return false;
    }
    
	m_isGameOver = false;
  
	// Background
	InitGroundAndSkyline();

	// Bird
	InitBird();

	// Input
	InitInput();

	// Init and auto-generate pipes
	memset(m_sprPipe, 0, sizeof(Sprite*) * NUM_ACTIVE_PIPES);
	schedule(CC_SCHEDULE_SELECTOR(Game::GeneratePipes), 1);

    return true;
}


void Game::InitGroundAndSkyline()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Texture2D::TexParams texParams = { GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST, GL_REPEAT, GL_CLAMP_TO_EDGE };

	// Ground
	Texture2D* groundTexture = Director::getInstance()->getTextureCache()->addImage("ground.png");
	Size groundTextureSize = groundTexture->getContentSizeInPixels();
	groundTexture->generateMipmap();
	groundTexture->setTexParameters(&texParams);
	m_sprGround = Sprite::createWithTexture(groundTexture, Rect(0, 0, visibleSize.width + groundTextureSize.width, groundTextureSize.height));
	m_sprGround->setAnchorPoint(Vec2(0, 0));

	Sequence* scrollGround = Sequence::create(MoveTo::create(groundTextureSize.width / MAP_SCROLL_SPEED, Vec2(-groundTextureSize.width, m_sprGround->getPositionY())),
											  MoveTo::create(0, Vec2(0, m_sprGround->getPositionY())),
											  NULL);

	m_sprGround->runAction(Repeat::create(scrollGround, -1));
	this->addChild(m_sprGround);

	// Skyline
	Texture2D* skylineTexture = Director::getInstance()->getTextureCache()->addImage("skyline.png");
	Size skylineTextureSize = skylineTexture->getContentSizeInPixels();
	skylineTexture->generateMipmap();
	skylineTexture->setTexParameters(&texParams);
	m_sprSkyline = Sprite::createWithTexture(skylineTexture, Rect(0, 0, visibleSize.width + skylineTextureSize.width, skylineTextureSize.height));
	m_sprSkyline->setAnchorPoint(Vec2(0, 0));
	m_sprSkyline->setPosition(Vec2(0, groundTextureSize.height));

	Sequence* scrollSkyline = Sequence::create(MoveTo::create(skylineTextureSize.width / (MAP_SCROLL_SPEED >> 1), Vec2(-skylineTextureSize.width, m_sprSkyline->getPositionY())),
											   MoveTo::create(0, Vec2(0, m_sprSkyline->getPositionY())),
											   NULL);

	m_sprSkyline->runAction(Repeat::create(scrollSkyline, -1));
	this->addChild(m_sprSkyline);

	// Ground collision
	PhysicsBody* body = PhysicsBody::createEdgeBox(Size(visibleSize.width, groundTextureSize.height));
	body->setContactTestBitmask(0xFFFFFFFF);
	Node* edgeNode = Node::create();
	edgeNode->setPosition(Point(visibleSize.width / 2, groundTextureSize.height / 2));
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);
}


void Game::InitBird()
{
	// Texture
	m_sprBird = Sprite::create("bird.png");
	m_sprBird->getTexture()->setAliasTexParameters();
	m_sprBird->setAnchorPoint(Vec2(0, 0));
	m_sprBird->setPosition(BIRD_START_X, Director::getInstance()->getVisibleSize().height - BIRD_START_Y);

	// Physics
	PhysicsBody* body = PhysicsBody::createBox(Size(m_sprBird->getContentSize().width / 2, m_sprBird->getContentSize().height));
	body->setContactTestBitmask(0xFFFFFFFF);
	m_sprBird->setPhysicsBody(body);

	//adds contact event listener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [&](PhysicsContact& contact)
	{
		m_sprBird->pause();
		m_sprGround->pause();
		m_sprSkyline->pause();

		for (int i = 0; i < NUM_ACTIVE_PIPES; i++)
		{
			if (m_sprPipe[i])
			{
				m_sprPipe[i]->pause();
			}
		}

		m_isGameOver = true;

		return true;
	};

	getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	// Anim
	Vector<SpriteFrame*> birdAnimFrames(2);
	birdAnimFrames.pushBack(SpriteFrame::createWithTexture(m_sprBird->getTexture(), Rect(0, 0, m_sprBird->getTextureRect().size.width / 2, m_sprBird->getTextureRect().size.height)));
	birdAnimFrames.pushBack(SpriteFrame::createWithTexture(m_sprBird->getTexture(), Rect(m_sprBird->getTextureRect().size.width / 2, 0, m_sprBird->getTextureRect().size.width / 2, m_sprBird->getTextureRect().size.height)));
	m_sprBird->runAction(Animate::create(Animation::createWithSpriteFrames(birdAnimFrames, BIRD_ANIM, -1)));

	this->addChild(m_sprBird);
}


void Game::GeneratePipes(float dt)
{
	if (m_isGameOver)
	{
		return;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	int startX = Director::getInstance()->getVisibleSize().width;
	int startY = Director::getInstance()->getVisibleSize().height;

	for (int i = 0; i < NUM_ACTIVE_PIPES; i++)
	{
		if (m_sprPipe[i] && m_sprPipe[i]->getPositionX() > startX)
		{
			startX = m_sprPipe[i]->getPositionX();
		}
	}

	for (int i = 0; i < NUM_ACTIVE_PIPES; i++)
	{
		if (m_sprPipe[i] == NULL)
		{
			// Load texture
			m_sprPipe[i] = Sprite::create("pipe.png");
			m_sprPipe[i]->getTexture()->setAliasTexParameters();

			// Random pipe type
			int frameId = rand() % 2;
			int frameWidth = m_sprPipe[i]->getTextureRect().size.width / 2;
			int frameHeight = m_sprPipe[i]->getTextureRect().size.height;

			// Random pipe length
			int height = (frameHeight >> 2) + rand() % (frameHeight >> 1);
			int maxHeight = (visibleSize.height - m_sprGround->getTexture()->getContentSize().height - (m_sprBird->getTextureRect().size.height * 4));
			height = (height > maxHeight ? maxHeight : height);

			// Random distance between two nearby pipes
			int x = startX + ((rand() % (NUM_ACTIVE_PIPES)) + NUM_ACTIVE_PIPES) * frameWidth;
			startX = x;

			// Load texture
			SpriteFrame* frame = SpriteFrame::create("pipe.png", Rect(frameId * frameWidth, (frameId == 0) ? 0 : (frameHeight - height), frameWidth, height));
			m_sprPipe[i] = Sprite::createWithSpriteFrame(frame);
			int y = (frameId == 0) ? (m_sprGround->getTexture()->getContentSize().height) : (visibleSize.height - height);

			// Init physics
			Rect pipeRect = m_sprPipe[i]->getTextureRect();
			PhysicsBody* body = PhysicsBody::createBox(Size(pipeRect.size.width, pipeRect.size.height));
			body->setContactTestBitmask(0xFFFFFFFF);
			body->setDynamic(false);
			m_sprPipe[i]->setPhysicsBody(body);
			m_sprPipe[i]->setPosition(Vec2(x + pipeRect.size.width / 2, y + pipeRect.size.height / 2));

			// Moving anim
			this->addChild(m_sprPipe[i]);
			m_sprPipe[i]->runAction(Sequence::create(MoveTo::create((float)(x + frameWidth + pipeRect.size.width / 2) / MAP_SCROLL_SPEED, Vec2(-frameWidth + pipeRect.size.width / 2, y + pipeRect.size.height / 2)), NULL));
		}
		else
		{
			if (m_isGameOver || m_sprPipe[i]->numberOfRunningActions() == 0)
			{
				// Cleanup
				m_sprPipe[i]->removeFromParentAndCleanup(true);
				m_sprPipe[i] = NULL;
			}
		}
	}
}


void Game::InitInput()
{
	EventListenerTouchOneByOne* inputListener = EventListenerTouchOneByOne::create();

	inputListener->setSwallowTouches(true);

	inputListener->onTouchBegan = [&](Touch* touch, Event* event)
	{
		if (m_isGameOver)
		{
			// Cleanup
			m_sprBird->removeFromParentAndCleanup(true);

			for (int i = 0; i < NUM_ACTIVE_PIPES; i++)
			{
				if (m_sprPipe[i])
				{
					m_sprPipe[i]->removeFromParentAndCleanup(true);
					m_sprPipe[i] = NULL;
				}
			}

			// Re-init
			InitBird();
			m_sprGround->resume();
			m_sprSkyline->resume();
			m_isGameOver = false;
		}
		else
		{
			m_sprBird->getPhysicsBody()->setVelocity(Vec2(0, LIFT_FORCE));
		}

		return true;
	};

	getEventDispatcher()->addEventListenerWithFixedPriority(inputListener, 1);
}
