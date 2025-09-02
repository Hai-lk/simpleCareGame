#ifndef __SCENE_H__
#define __SCENE_H__

#include "cocos2d.h"
#include "views/cardSprite.h"
#include <vector>
#include <stack>

struct OperationStep {
	enum ActionType { MOVE_TO_OPERATION };
	ActionType type;
	cocos2d::Ref* targetCard;
	cocos2d::Vector<cardSprite*>* originalContainer;
	cocos2d::Vec2 originalPosition;
};

class HelloWorld : public cocos2d::Scene {
private:
    cocos2d::Vector<cardSprite*> _leftCards;    // 左侧牌组
    cocos2d::Vector<cardSprite*> _rightCards;    // 右侧牌组
    cocos2d::Vector<cardSprite*> _handCards;      // 手牌区
    cocos2d::Vector<cardSprite*> _operationCards;  // 新增：操作区牌组
    cocos2d::Vector<cardSprite*> _allCards;       // 所有卡牌
    cardSprite* _selectedCard = nullptr;          // 当前选中的卡牌
    cocos2d::Vec2 _touchOffset;                   // 触摸偏移量
    cocos2d::Vec2 _originalPosition;              // 原始位置

    // 新增：操作区位置和大小
    cocos2d::Rect _operationZoneRect;
    cocos2d::Vec2 _operationZonePosition;

    // 新增：重新排列手牌
    void rearrangeHandCards();

	std::stack<OperationStep> _operationHistory;
	cocos2d::MenuItem* _undoButton;   

public:
    static cocos2d::Scene* createScene();

    // 初始化
    virtual bool init() override;
    static HelloWorld* create();

    // 触摸事件
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	//回退按钮
	void undoLastAction(); // 新增回退函数
	void updateUndoButton(); // 更新按钮状态

    // 创建卡牌
    cardSprite* createCard(CardSuitType suit, int number, cocos2d::Vec2 position);
};

#endif // __HELLOWORLD_SCENE_H__