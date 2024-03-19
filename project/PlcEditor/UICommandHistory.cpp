#include "StdAfx.h"
#include "UICommandHistory.h"

//////////////////////////////////////////////////////////////////////////
//CUICommandElement

CUICommandElement::CUICommandElement()
{
}

CUICommandElement::CUICommandElement(CUICommandElement& cp)
{
	list<libSch::CoilInfo>::iterator iter;
	if( !cp.m_listElement.empty() )
	{
		iter = cp.m_listElement.begin();
		while( iter != cp.m_listElement.end() )
		{
			libSch::CoilInfo pInfo;
			pInfo.pos = (*iter).pos;
			pInfo.pCoil = libSch::CCoilBlock::CopyCoil( (*iter).pCoil );
			m_listElement.push_back( pInfo );
			iter++;
		}
	}
}
CUICommandElement::CUICommandElement(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected, BOOL bModify)
{
	for(int i=0; i<arrSelected.GetSize(); i++)
	{
		libSch::CoilInfo pInfo;
		pInfo.pos = arrSelected[i].pos;
		pInfo.pCoil = libSch::CCoilBlock::CopyCoil( arrSelected[i].pCoil );
		m_listElement.push_back( pInfo );
	}
}


CUICommandElement::~CUICommandElement()
{
	list<libSch::CoilInfo>::iterator iter;
	if( !m_listElement.empty() )
	{
		iter = m_listElement.begin();
		while( iter != m_listElement.end() )
		{
			//CKUIBuilder::DeleteUI( *iter );
			m_listElement.erase(iter++);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//CUICommandNode

CUICommandNode::CUICommandNode(ActionType type)
	: m_ActionType(type), m_pBefore(NULL), m_pAfter(NULL), m_pAllSelected(NULL)
	//, m_pControl(NULL)
{

}

CUICommandNode::CUICommandNode(CUICommandElement* pBefore, CUICommandElement* pAfter, ActionType type)
	: m_ActionType(type), m_pAllSelected(NULL)//, m_pControl(NULL)
{
	m_pBefore = ( pBefore ? new CUICommandElement(*pBefore) : NULL );
	m_pAfter = ( pAfter ? new CUICommandElement(*pAfter) : NULL );

	switch(type)
	{
	case actionAdd:
		break;
	case actionModify:
		//RemoveSameProperties(m_pBefore->m_pElementXml, m_pAfter->m_pElementXml);
		break;
	case actionDelete:
		break;
	}
}

CUICommandNode::~CUICommandNode()
{
	if(m_pBefore)
		delete m_pBefore;
	if(m_pAfter)
		delete m_pAfter;
}

void CUICommandNode::Begin(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected)
{
	ASSERT(m_pBefore == NULL && m_pAfter == NULL);
	ASSERT(m_pAllSelected == NULL);

	m_pAllSelected = &arrSelected;
	switch(m_ActionType)
	{
	case actionAdd:
		m_pBefore = NULL;
		break;
	case actionModify:
		m_pBefore = new CUICommandElement(arrSelected, TRUE);
		break;
	case actionDelete:
		m_pBefore = new CUICommandElement(arrSelected, FALSE);
		break;
	}
}

void CUICommandNode::End()
{
	ASSERT(m_pAfter == NULL);
	ASSERT(m_pAllSelected);

	switch(m_ActionType)
	{
	case actionAdd:
		m_pAfter = new CUICommandElement(*m_pAllSelected, FALSE);
		break;
	case actionModify:
		m_pAfter = new CUICommandElement(*m_pAllSelected, TRUE);
		//RemoveSameProperties(m_pBefore->m_pElementXml, m_pAfter->m_pElementXml);
		break;
	case actionDelete:
		m_pAfter = NULL;
		break;
	}

	m_pAllSelected = NULL;
}

//////////////////////////////////////////////////////////////////////////
//CUICommandHistory

CUICommandHistory::CUICommandHistory(void)
	: m_pNode(NULL), m_nCommandIndex(0)
{
}

CUICommandHistory::~CUICommandHistory(void)
{
	while(!m_lstCommandNodes.IsEmpty())
		delete m_lstCommandNodes.RemoveHead();
}

void CUICommandHistory::Begin(CArray<libSch::CoilInfo,libSch::CoilInfo>& arrSelected, ActionType type)
{
	ASSERT(m_pNode == NULL);

	m_pNode = new CUICommandNode(type);
	m_pNode->Begin(arrSelected);
}

void CUICommandHistory::Cancel()
{
	delete m_pNode;
	m_pNode = NULL;
}
void CUICommandHistory::End()
{
	ASSERT(m_pNode);

	m_pNode->End();
	AddUICommand(m_pNode);
	m_pNode = NULL;
}


BOOL CUICommandHistory::AddUICommand(CUICommandNode* pNode)
{
	switch(pNode->m_ActionType)
	{
	case actionAdd:
		if(pNode->m_pAfter == NULL)
			return FALSE;
		break;
	case actionModify:
		if(pNode->m_pBefore==NULL || pNode->m_pAfter==NULL)
			return FALSE;
		break;
	case actionDelete:
		if(pNode->m_pBefore == NULL)
			return FALSE;
		break;
	}

	int count = m_lstCommandNodes.GetCount();
	while(m_nCommandIndex < count--)
		delete m_lstCommandNodes.RemoveTail();
	if(m_lstCommandNodes.GetCount() >= UI_COMMAND_HISTORY)
		delete m_lstCommandNodes.RemoveHead();
	m_lstCommandNodes.AddTail(pNode);
	m_nCommandIndex = m_lstCommandNodes.GetCount();

	return TRUE;
}

void CUICommandHistory::Undo()
{
	UICommandAction(cmdUndo);
}

void CUICommandHistory::Redo()
{
	UICommandAction(cmdRedo);
}

void CUICommandHistory::UICommandAction(CommandType type)
{
	CUICommandNode* pOldNode;
	CUICommandNode* pNewNode;

	if(type == cmdRedo)
	{
		if(!CanRedo())
			return;

		pOldNode = m_lstCommandNodes.GetAt(m_lstCommandNodes.FindIndex(m_nCommandIndex));
		pNewNode = new CUICommandNode(pOldNode->m_pBefore, pOldNode->m_pAfter, pOldNode->m_ActionType);
		m_nCommandIndex++;
	}
	else
	{
		if(!CanUndo())
			return;

		m_nCommandIndex--;
		pOldNode = m_lstCommandNodes.GetAt(m_lstCommandNodes.FindIndex(m_nCommandIndex));
		ActionType action;
		switch(pOldNode->m_ActionType)
		{
		case actionAdd:
			action = actionDelete;
			break;
		case actionModify:
			action = actionModify;
			break;
		case actionDelete:
			action = actionAdd;
			break;
		}
		pNewNode = new CUICommandNode(pOldNode->m_pAfter, pOldNode->m_pBefore, action);
	}

	switch(pNewNode->m_ActionType)
	{
	case actionAdd:
		ActionAdd(pNewNode->m_pAfter);
		break;
	case actionModify:
		ActionModify(pNewNode->m_pAfter);
		break;
	case actionDelete:
		ActionDelete(pNewNode->m_pBefore);
		break;
	}

	delete pNewNode;
}

BOOL CUICommandHistory::CanUndo()
{
	return m_nCommandIndex != 0;
}

BOOL CUICommandHistory::CanRedo()
{
	return m_nCommandIndex != m_lstCommandNodes.GetCount();
}

void CUICommandHistory::ActionAdd(CUICommandElement* pAfter)
{
	//g_pMainFrame->GetActiveUIView()->ClearUITracker();
	list<libSch::CoilInfo>::iterator iter;
	if( !pAfter->m_listElement.empty() )
	{
		iter = pAfter->m_listElement.begin();
		while( iter != pAfter->m_listElement.end() )
		{
			//g_pMainFrame->GetActiveUIView()->NewUI( *iter );
			//pBefore->m_listElement.erase(iter++);
			iter++;
		}
	}
}

void CUICommandHistory::ActionModify(CUICommandElement* pAfter)
{
	list<libSch::CoilInfo>::iterator iter;
	if( !pAfter->m_listElement.empty() )
	{
		iter = pAfter->m_listElement.begin();
		while( iter != pAfter->m_listElement.end() )
		{
			//g_pMainFrame->GetActiveUIView()->ModifyUI( (*iter)->GetID(), (*iter) );
			//pBefore->m_listElement.erase(iter++);
			iter++;
		}
	}
}

void CUICommandHistory::ActionDelete(CUICommandElement* pBefore)
{
	list<libSch::CoilInfo>::iterator iter;
	if( !pBefore->m_listElement.empty() )
	{
		iter = pBefore->m_listElement.begin();
		while( iter != pBefore->m_listElement.end() )
		{
			//g_pMainFrame->GetActiveUIView()->DeleteUI( (*iter)->GetID() );
			//pBefore->m_listElement.erase(iter++);
			iter++;
		}
	}
}
