#include "Scene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
	return HelloWorld::create();
}

bool HelloWorld::init() {
	if (!Scene::init()) return false;

	// ��ȡ��������
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// ��������
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

	// ����λ��
	const float cardSpacing = 40.0f;
	const float cardHeight = 50.0f;
	const float handCare = 100.0f;

	Vec2 leftPosition(visibleSize.width*0.1, visibleSize.height*0.6);
	Vec2 rightPosition(visibleSize.width*0.7, visibleSize.height*0.6);
	Vec2 handPosition(visibleSize.width*0.1, visibleSize.height*0.1);

	// ===== ������������λ�� =====
	_operationZonePosition = Vec2(visibleSize.width * 0.7, visibleSize.height * 0.1);

	// ��������
	_leftCards.pushBack(createCard(CST_CLUBS, 13, leftPosition));
	_leftCards.pushBack(createCard(CST_CLUBS, 3, leftPosition + Vec2(cardSpacing, -cardHeight)));
	_leftCards.pushBack(createCard(CST_DIAMONDS, 3, leftPosition + Vec2(cardSpacing * 2, -cardHeight * 2)));

	_rightCards.pushBack(createCard(CST_CLUBS, 3, rightPosition));
	_rightCards.pushBack(createCard(CST_CLUBS, 3, rightPosition + Vec2(-cardSpacing, -cardHeight)));
	_rightCards.pushBack(createCard(CST_SPADES, 2, rightPosition + Vec2(-cardSpacing * 2, -cardHeight * 2)));

	_handCards.pushBack(createCard(CST_CLUBS, 3, handPosition));
	_handCards.pushBack(createCard(CST_HEARTS, 1, handPosition + Vec2(-handCare, 0)));
	_handCards.pushBack(createCard(CST_CLUBS, 4, handPosition + Vec2(-handCare * 2, 0)));

	// ��ʼ�������� 
	// ����һ�����Ʒ��������
	if (!_handCards.empty()) {
		int n = _handCards.size()-1;
		cardSprite* firstCard = _handCards.at(n);
		_handCards.erase(n);  // ���������Ƴ�
		_operationCards.pushBack(firstCard);  // ��ӵ�������
		firstCard->runAction(MoveTo::create(0.5f, _operationZonePosition));  // �ƶ���������
	}
	rearrangeHandCards();

	// ע�ᴥ���¼�
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	// �������ı����˰�ť - ����������ʽ
	auto undoLabel = Label::createWithTTF("undo", "fonts/simkai.ttf", 60);

	undoLabel->setTextColor(Color4B::BLUE);

	// �ص���ʽ
	_undoButton = MenuItemLabel::create(undoLabel, [this](Ref* sender) {
		this->undoLastAction();
	});

	// ���ð�ťλ��
	_undoButton->setPosition(Vec2(visibleSize.width*0.15, visibleSize.height*0.3));
	_undoButton->setEnabled(false);

	auto menu = Menu::create(_undoButton, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 10);

	return true;
}

// ������������������
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
	card->setTag(static_cast<int>(_allCards.size())); // Ψһ��ʶ
	this->addChild(card,0);
	_allCards.pushBack(card);
	return card;
}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event) {
	Vec2 location = touch->getLocation();
	_selectedCard = nullptr;

	// �������ƶѶ�
	if (!_leftCards.empty()) {
		cardSprite* topLeft = _leftCards.back();
		if (topLeft->getBoundingBox().containsPoint(location)) {
			_selectedCard = topLeft;
		}
	}

	// ����Ҳ��ƶѶ�
	if (!_selectedCard && !_rightCards.empty()) {
		cardSprite* topRight = _rightCards.back();
		if (topRight->getBoundingBox().containsPoint(location)) {
			_selectedCard = topRight;
		}
	}

	// 1. ���ȼ������������ƣ�������ȼ���
	if (!_operationCards.empty()) {
		cardSprite* topOperation = _operationCards.back();
		if (topOperation->getBoundingBox().containsPoint(location)) {
			_selectedCard = topOperation;
		}
	}

	// 2. �����������Ƭ������ӵĹ���
	if (!_selectedCard && !_handCards.empty()) {
		cardSprite* topHandCard = _handCards.back(); // ��ȡ���Ҳ����
		if (topHandCard->getBoundingBox().containsPoint(location)) {
			_selectedCard = topHandCard;
		}
	}


	// ==== ԭ���߼�������ѡ������Ѷ���ʱ��Ч��====
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
	bool isFromHand = _handCards.contains(_selectedCard);  // �ж����Ƿ�����������

	// ===== �޸ĵ㣺����������������֤ =====
	if (isFromHand) {
		// ����������ֱ�������ƶ���������
		shouldMoveToOperation = true;
	}
	else {
		// �����������Ʊ���ԭ����֤����
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
			// ������Ϊ��ʱ�������
			shouldMoveToOperation = true;
		}
	}

	if (shouldMoveToOperation) {

		// ��¼������ʷ
		OperationStep step;
		step.type = OperationStep::MOVE_TO_OPERATION;
		step.targetCard = _selectedCard;

		// ȷ��ԭʼ����
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

		// ���°�ť״̬
		updateUndoButton();


		// ��ԭ�����Ƴ�
		if (_leftCards.contains(_selectedCard)) {
			_leftCards.eraseObject(_selectedCard);
		}
		else if (_rightCards.contains(_selectedCard)) {
			_rightCards.eraseObject(_selectedCard);
		}
		else if (isFromHand) {
			_handCards.eraseObject(_selectedCard);
			rearrangeHandCards();  // ������������
		}

		// ��ӵ�������
		_operationCards.pushBack(_selectedCard);
		_selectedCard->setLocalZOrder(10);
		_selectedCard->runAction(MoveTo::create(0.3f, _operationZonePosition));
	}
	else {
		// ����ԭλ
		_selectedCard->runAction(MoveTo::create(0.2f, _originalPosition));
	}

	// ����״̬
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

// �޸ĺ���룺
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
				rearrangeHandCards();  // ������������λ��
			}
			else {
				// ��������ʹ��ԭʼλ��
				card->runAction(MoveTo::create(0.3f, lastStep.originalPosition));
			}
		}
	}
	updateUndoButton();
}

// ���°�ť״̬����
void HelloWorld::updateUndoButton() {
	bool enable = !_operationHistory.empty();
	_undoButton->setEnabled(enable);

	// ��ȡ��ť��ǩ��������ɫ
	MenuItemLabel* labelItem = dynamic_cast<MenuItemLabel*>(_undoButton);
	if (labelItem) {
		Label* label = dynamic_cast<Label*>(labelItem->getLabel());
		if (label) {
			label->setTextColor(enable ? Color4B::BLUE : Color4B::GRAY);
		}
	}
}