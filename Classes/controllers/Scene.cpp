#include "Scene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
	return HelloWorld::create();
}

bool HelloWorld::init() {
	if (!Scene::init()) return false;

	// 获取可视区域
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 创建背景
	auto bottomBackground = LayerColor::create(Color4B(128, 0, 128, 255));
	bottomBackground->setContentSize(Size(visibleSize.width, visibleSize.height*0.4));
	bottomBackground->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	bottomBackground->setPosition(Vec2(0, 0));
	this->addChild(bottomBackground, -10);

	auto topBackground = LayerColor::create(Color4B(170, 121, 66, 255));
	topBackground->setContentSize(Size(visibleSize.width, visibleSize.height*0.7));
	topBackground->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	topBackground->setPosition(Vec2(0, visibleSize.height*0.4));
	this->addChild(topBackground, -10);

	// 定义位置
	const float cardSpacing = 40.0f;
	const float cardHeight = 50.0f;
	const float handCare = 100.0f;

	Vec2 leftPosition(visibleSize.width*0.1, visibleSize.height*0.6);
	Vec2 rightPosition(visibleSize.width*0.7, visibleSize.height*0.6);
	Vec2 handPosition(visibleSize.width*0.1, visibleSize.height*0.1);

	// ===== 新增：操作区位置 =====
	_operationZonePosition = Vec2(visibleSize.width * 0.7, visibleSize.height * 0.1);

	// 创建卡牌
	_leftCards.pushBack(createCard(CST_CLUBS, 13, leftPosition));
	_leftCards.pushBack(createCard(CST_CLUBS, 3, leftPosition + Vec2(cardSpacing, -cardHeight)));
	_leftCards.pushBack(createCard(CST_DIAMONDS, 3, leftPosition + Vec2(cardSpacing * 2, -cardHeight * 2)));

	_rightCards.pushBack(createCard(CST_CLUBS, 3, rightPosition));
	_rightCards.pushBack(createCard(CST_CLUBS, 3, rightPosition + Vec2(-cardSpacing, -cardHeight)));
	_rightCards.pushBack(createCard(CST_SPADES, 2, rightPosition + Vec2(-cardSpacing * 2, -cardHeight * 2)));

	_handCards.pushBack(createCard(CST_CLUBS, 3, handPosition));
	_handCards.pushBack(createCard(CST_HEARTS, 1, handPosition + Vec2(-handCare, 0)));
	_handCards.pushBack(createCard(CST_CLUBS, 4, handPosition + Vec2(-handCare * 2, 0)));

	// 初始化操作区 
	// 将第一张手牌放入操作区
	if (!_handCards.empty()) {
		int n = _handCards.size()-1;
		cardSprite* firstCard = _handCards.at(n);
		_handCards.erase(n);  // 从手牌区移除
		_operationCards.pushBack(firstCard);  // 添加到操作区
		firstCard->runAction(MoveTo::create(0.5f, _operationZonePosition));  // 移动到操作区
	}
	rearrangeHandCards();

	// 注册触摸事件
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// 创建纯文本回退按钮 - 修正创建方式
	auto undoLabel = Label::createWithTTF("undo", "fonts/simkai.ttf", 60);

	undoLabel->setTextColor(Color4B::BLUE);

	// 回调方式
	_undoButton = MenuItemLabel::create(undoLabel, [this](Ref* sender) {
		this->undoLastAction();
	});

	// 设置按钮位置
	_undoButton->setPosition(Vec2(visibleSize.width*0.15, visibleSize.height*0.3));
	_undoButton->setEnabled(false);

	auto menu = Menu::create(_undoButton, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 10);

	return true;
}

// 新增：重新排列手牌
void HelloWorld::rearrangeHandCards() {
	const float cardSpacing = 40.0f;
	Vec2 handPosition(Director::getInstance()->getVisibleSize().width * 0.3,
		Director::getInstance()->getVisibleSize().height * 0.1);

	for (int i = 0; i < _handCards.size(); ++i) {
		Vec2 newPosition = handPosition + Vec2(cardSpacing * i, 0);
		_handCards.at(i)->runAction(MoveTo::create(0.3f, newPosition));
	}
}

cardSprite* HelloWorld::createCard(CardSuitType suit, int number, Vec2 position) {
	cardSprite* card = cardSprite::createCard(suit, number);
	card->setPosition(position);
	card->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	card->setTag(static_cast<int>(_allCards.size())); // 唯一标识
	this->addChild(card,0);
	_allCards.pushBack(card);
	return card;
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
	Vec2 location = touch->getLocation();
	_selectedCard = nullptr;

	// 检查左侧牌堆顶
	if (!_leftCards.empty()) {
		cardSprite* topLeft = _leftCards.back();
		if (topLeft->getBoundingBox().containsPoint(location)) {
			_selectedCard = topLeft;
		}
	}

	// 检查右侧牌堆顶
	if (!_selectedCard && !_rightCards.empty()) {
		cardSprite* topRight = _rightCards.back();
		if (topRight->getBoundingBox().containsPoint(location)) {
			_selectedCard = topRight;
		}
	}

	// 1. 首先检查操作区顶部牌（最高优先级）
	if (!_operationCards.empty()) {
		cardSprite* topOperation = _operationCards.back();
		if (topOperation->getBoundingBox().containsPoint(location)) {
			_selectedCard = topOperation;
		}
	}

	// 2. 检查手牌区卡片（新添加的规则）
	if (!_selectedCard && !_handCards.empty()) {
		cardSprite* topHandCard = _handCards.back(); // 获取最右侧的牌
		if (topHandCard->getBoundingBox().containsPoint(location)) {
			_selectedCard = topHandCard;
		}
	}


	// ==== 原有逻辑（仅当选中桌面堆顶牌时生效）====
	if (_selectedCard) {
		_originalPosition = _selectedCard->getPosition();
		_touchOffset = _selectedCard->getPosition() - location;
		_selectedCard->setScale(1.1f);
		return true;
	}
	return false;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* event) {
	if (_selectedCard) {
		Vec2 location = touch->getLocation();
		_selectedCard->setPosition(location + _touchOffset);
	}
}

void HelloWorld::onTouchEnded(Touch* touch, Event* event) {
	if (!_selectedCard) return;

	bool shouldMoveToOperation = false;
	bool isFromHand = _handCards.contains(_selectedCard);  // 判断牌是否来自手牌区

	// ===== 修改点：手牌区的牌跳过验证 =====
	if (isFromHand) {
		// 手牌区的牌直接允许移动到操作区
		shouldMoveToOperation = true;
	}
	else {
		// 非手牌区的牌保持原有验证规则
		if (!_operationCards.empty()) {
			cardSprite* operationTop = _operationCards.back();
			int selectedValue = _selectedCard->getNumber();
			int operationValue = operationTop->getNumber();
			int diff = abs(selectedValue - operationValue);

			if (diff == 1) {
				shouldMoveToOperation = true;
			}
		}
		else {
			// 操作区为空时允许放入
			shouldMoveToOperation = true;
		}
	}

	if (shouldMoveToOperation) {

		// 记录操作历史
		OperationStep step;
		step.type = OperationStep::MOVE_TO_OPERATION;
		step.targetCard = _selectedCard;

		// 确定原始容器
		if (isFromHand) {
			step.originalContainer = &_handCards;
		}
		else if (_leftCards.contains(_selectedCard)) {
			step.originalContainer = &_leftCards;
		}
		else if (_rightCards.contains(_selectedCard)) {
			step.originalContainer = &_rightCards;
		}

		step.originalPosition = _originalPosition;
		_operationHistory.push(step);

		// 更新按钮状态
		updateUndoButton();


		// 从原区域移除
		if (_leftCards.contains(_selectedCard)) {
			_leftCards.eraseObject(_selectedCard);
		}
		else if (_rightCards.contains(_selectedCard)) {
			_rightCards.eraseObject(_selectedCard);
		}
		else if (isFromHand) {
			_handCards.eraseObject(_selectedCard);
			rearrangeHandCards();  // 重新排列手牌
		}

		// 添加到操作区
		_operationCards.pushBack(_selectedCard);
		_selectedCard->setLocalZOrder(10);
		_selectedCard->runAction(MoveTo::create(0.3f, _operationZonePosition));
	}
	else {
		// 返回原位
		_selectedCard->runAction(MoveTo::create(0.2f, _originalPosition));
	}

	// 重置状态
	_selectedCard->setScale(1.0f);
	_selectedCard = nullptr;
}

HelloWorld* HelloWorld::create() {
	HelloWorld *pRet = new(std::nothrow) HelloWorld();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

// 修改后代码：
void HelloWorld::undoLastAction() {
	if (_operationHistory.empty()) return;

	OperationStep lastStep = _operationHistory.top();
	_operationHistory.pop();

	if (lastStep.type == OperationStep::MOVE_TO_OPERATION) {
		cardSprite* card = dynamic_cast<cardSprite*>(lastStep.targetCard);
		if (card) {
			_operationCards.eraseObject(card);
			lastStep.originalContainer->pushBack(card);

			if (lastStep.originalContainer == &_handCards) {
				rearrangeHandCards();  // 处理所有手牌位置
			}
			else {
				// 仅非手牌使用原始位置
				card->runAction(MoveTo::create(0.3f, lastStep.originalPosition));
			}
		}
	}
	updateUndoButton();
}

// 更新按钮状态函数
void HelloWorld::updateUndoButton() {
	bool enable = !_operationHistory.empty();
	_undoButton->setEnabled(enable);

	// 获取按钮标签并设置颜色
	MenuItemLabel* labelItem = dynamic_cast<MenuItemLabel*>(_undoButton);
	if (labelItem) {
		Label* label = dynamic_cast<Label*>(labelItem->getLabel());
		if (label) {
			label->setTextColor(enable ? Color4B::BLUE : Color4B::GRAY);
		}
	}
}