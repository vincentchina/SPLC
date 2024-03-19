#pragma once
#include "LADVIew.h"
#include <list>

#define UI_COMMAND_HISTORY 10

using namespace std;


enum CommandType
{
	cmdUndo,
	cmdRedo,
};
enum ActionType
{
	actionAdd,
	actionDelete,
	actionModify,
};

//////////////////////////////////////////////////////////////////////////
//CUICommandElement

class CUICommandNode;
class CUICommandHistory;

class CUICommandElement
{
	friend CUICommandNode;
	friend CUICommandHistory;

public:
	CUICommandElement();
	CUICommandElement(CUICommandElement& cp);
	CUICommandElement(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected, BOOL bModify);

	~CUICommandElement();

protected:
	list<libSch::CoilInfo> m_listElement;
};

//////////////////////////////////////////////////////////////////////////
//CUICommandNode

class CUICommandHistory;

class CUICommandNode
{
	friend CUICommandHistory;

public:
	CUICommandNode(ActionType type);
	CUICommandNode(CUICommandElement* pBefore, CUICommandElement* pAfter, ActionType type);

	~CUICommandNode();

public:
	void Begin(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected);
	void End();

protected:
	ActionType m_ActionType;
	CUICommandElement* m_pBefore;
	CUICommandElement* m_pAfter;
	CArray<libSch::CoilInfo,libSch::CoilInfo>* m_pAllSelected;
	//libSch::CoilInfo m_pControl;
};

//////////////////////////////////////////////////////////////////////////
//CUICommandHistory


class CUICommandHistory
{
public:
	CUICommandHistory(void);
	~CUICommandHistory(void);

public:
	void Begin(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected, ActionType type);
	void End();
	void Cancel();

	void Undo();
	void Redo();
	BOOL CanUndo();
	BOOL CanRedo();

protected:
	BOOL AddUICommand(CUICommandNode* pNode);
	void UICommandAction(CommandType type);

	void ActionAdd(CUICommandElement* pAfter);
	void ActionModify(CUICommandElement* pAfter);
	void ActionDelete(CUICommandElement* pBefore);

private:
	CUICommandNode* m_pNode;
	int m_nCommandIndex;
	CList<CUICommandNode*, CUICommandNode*> m_lstCommandNodes;
};