#include "cardSprite.h"
#include <string>

USING_NS_CC;

cardSprite * cardSprite::createCard(CardSuitType suit, int number)
{
	cardSprite* cardSprite = cardSprite::create();
	if (cardSprite && cardSprite->initWithCard(suit, number)) {
		return cardSprite;
	}
	CC_SAFE_DELETE(cardSprite);

	return nullptr;
}

bool cardSprite::init() {
	// 基础初始化（可选：加载默认图片）
	if (!Sprite::init()) {
		return false;
	}
	return true;
}


//根据花色和数字创建卡牌
bool cardSprite::initWithCard(CardSuitType suit, int number) {

	//读取卡片点数
	cardNumber = number;

	// 1. 添加主卡牌背景图片
	const char* bgFile = "card_general.png"; // 卡牌背景图
	if (!this->initWithFile(bgFile)) {
		CCLOG("Error loading background: %s", bgFile);
		return false;
	}

	// 3. 根据花色添加图标
	std::string suitFile;
	switch (suit) {
	case CST_CLUBS:    suitFile = "suits/club.png"; break;
	case CST_DIAMONDS: suitFile = "suits/diamond.png"; break;
	case CST_HEARTS:   suitFile = "suits/heart.png"; break;
	case CST_SPADES:   suitFile = "suits/spade.png"; break;
	default:           suitFile = "suits/default.png";
	}

	// 创建花色精灵并添加为子节点
	Sprite* suitSprite = Sprite::create(suitFile);
	if (suitSprite) {
		suitSprite->setPosition(Vec2(this->getContentSize().width - 40, this->getContentSize().height - 40));
		this->addChild(suitSprite, 1); 
	}

	//颜色判断
	std::string numColor;
	if (suit > 0 && suit < 3) numColor = "red";
	else  numColor = "black";

	//数字判断
	std::string numberName;
	if (number >= 2 && number <= 10) {
		numberName = std::to_string(number);
	}
	else if (number == 1) {
		numberName = "A";
	}
	else if (number == 11) {
		numberName = "J";
	}
	else if (number == 12) {
		numberName = "Q";
	}
	else if (number == 13) {
		numberName = "K";
	}
	else {
		CCLOG("Wrong number entered");
	}


	// 创建数字精灵并添加为子节点
	std::string smallNumFile = "number/small_"+numColor+"_"+ numberName +".png";
	Sprite* smallNumSprite = Sprite::create(smallNumFile);
	if (smallNumSprite) {
		smallNumSprite->setPosition(Vec2(40, this->getContentSize().height - 40));
		this->addChild(smallNumSprite, 1); // Z-order设为1
	}

	std::string bigNumFile = "number/big_" + numColor + "_" + numberName + ".png";
	Sprite* bigNumSprite = Sprite::create(bigNumFile);
	if (bigNumSprite) {
		bigNumSprite->setPosition(Vec2(this->getContentSize().width/2, this->getContentSize().height/2-20));
		this->addChild(bigNumSprite, 1); // Z-order设为1
	}
	return true;
}