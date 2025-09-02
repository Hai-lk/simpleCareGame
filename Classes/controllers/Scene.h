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
    cocos2d::Vector<cardSprite*> _leftCards;    // �������
    cocos2d::Vector<cardSprite*> _rightCards;    // �Ҳ�����
    cocos2d::Vector<cardSprite*> _handCards;      // ������
    cocos2d::Vector<cardSprite*> _operationCards;  // ����������������
    cocos2d::Vector<cardSprite*> _allCards;       // ���п���
    cardSprite* _selectedCard = nullptr;          // ��ǰѡ�еĿ���
    cocos2d::Vec2 _touchOffset;                   // ����ƫ����
    cocos2d::Vec2 _originalPosition;              // ԭʼλ��

    // ������������λ�úʹ�С
    cocos2d::Rect _operationZoneRect;
    cocos2d::Vec2 _operationZonePosition;

    // ������������������
    void rearrangeHandCards();

	std::stack<OperationStep> _operationHistory;
	cocos2d::MenuItem* _undoButton;   

public:
    static cocos2d::Scene* createScene();

    // ��ʼ��
    virtual bool init() override;
    static HelloWorld* create();

    // �����¼�
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	//���˰�ť
	void undoLastAction(); // �������˺���
	void updateUndoButton(); // ���°�ť״̬

    // ��������
    cardSprite* createCard(CardSuitType suit, int number, cocos2d::Vec2 position);
};

#endif // __HELLOWORLD_SCENE_H__