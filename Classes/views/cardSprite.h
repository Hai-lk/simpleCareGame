#ifndef _CARD_SPRITE_H_
#define _CARD_SPRITE_H_
#include "cocos2d.h"
#include <string>

enum CardSuitType
{
	CST_NONE = -1,
	CST_CLUBS,      // 梅花
	CST_DIAMONDS,   // 方块
	CST_HEARTS,     // 红桃
	CST_SPADES,     // 黑桃
	CST_NUM_CARD_SUIT_TYPES
};

enum CardFaceType
{
	CFT_NONE = -1,
	CFT_ACE,
	CFT_TWO,
	CFT_THREE,
	CFT_FOUR,
	CFT_FIVE,
	CFT_SIX,
	CFT_SEVEN,
	CFT_EIGHT,
	CFT_NINE,
	CFT_TEN,
	CFT_JACK,
	CFT_QUEEN,
	CFT_KING,
	CFT_NUM_CARD_FACE_TYPES
};

class cardSprite : public cocos2d::Sprite
{
public:
	// 创建指定卡片(花色和数字)
	static cardSprite* createCard(CardSuitType suit, int number);
	int getNumber() { return cardNumber; };

private:
	bool init() override;

	int cardNumber = 0;

	// 新增初始化方法
	bool initWithCard(CardSuitType suit, int number);

	CREATE_FUNC(cardSprite);
	
};
#endif // !_MY_SPRITE_H_

