  /// \file ConnectorManager.cc
/*
 *
 * ConnectorManager.cc source template automatically generated by a class generator
 * Creation date : ven. f�vr. 7 2014
 *
 * This file is part of ArborPFA libraries.
 * 
 * ArborPFA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborPFA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborPFA.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "arborpfa/content/ConnectorManager.h"

#include "arborpfa/content/ConnectorImpl.h"

namespace pandora
{

/*********************************************
 * Manager<Connector> template implementation
 *********************************************/

template<>
const std::string Manager<arborpfa::Connector>::NULL_LIST_NAME = "NullList";

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
Manager<arborpfa::Connector>::Manager() :
    m_currentListName(NULL_LIST_NAME)
{

}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
Manager<arborpfa::Connector>::~Manager()
{

}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::GetList(const std::string &listName, const ObjectList *&pObjectList) const
{
    NameToListMap::const_iterator iter = m_nameToListMap.find(listName);

    if (m_nameToListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pObjectList = iter->second;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::GetCurrentList(const ObjectList *&pObjectList, std::string &listName) const
{
    listName = m_currentListName;
    return this->GetList(listName, pObjectList);
}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
inline StatusCode Manager<arborpfa::Connector>::GetCurrentListName(std::string &listName) const
{
    if (m_currentListName.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    listName = m_currentListName;
    return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const ObjectList *&pObjectList, std::string &listName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() != iter)
    {
        listName = iter->second.m_parentListName;
    }
    else
    {
        listName = m_currentListName;
    }

    return this->GetList(listName, pObjectList);
}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
inline StatusCode Manager<arborpfa::Connector>::GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &listName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return this->GetCurrentListName(listName);

    listName = iter->second.m_parentListName;
    return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    return this->GetAlgorithmInputListName(pAlgorithm, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &listName)
{
    if (m_nameToListMap.end() == m_nameToListMap.find(listName))
        return STATUS_CODE_NOT_FOUND;

    if (m_savedLists.end() == m_savedLists.find(listName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_currentListName = listName;

    for (AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = listName;
    }

    return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode Manager<arborpfa::Connector>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{

 AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

 if (m_algorithmInfoMap.end() == iter)
     return STATUS_CODE_NOT_FOUND;

 temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

 if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
     return STATUS_CODE_ALREADY_PRESENT;

 m_nameToListMap[temporaryListName] = new ObjectList;
 m_currentListName = temporaryListName;

 return STATUS_CODE_SUCCESS;

}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::RegisterAlgorithm(const Algorithm *const pAlgorithm)
{
    if (m_algorithmInfoMap.end() != m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_ALREADY_PRESENT;

    AlgorithmInfo algorithmInfo;
    algorithmInfo.m_parentListName = m_currentListName;
    algorithmInfo.m_numberOfListsCreated = 0;

    if (!m_algorithmInfoMap.insert(AlgorithmInfoMap::value_type(pAlgorithm, algorithmInfo)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToListMap::iterator iter = m_nameToListMap.find(*listNameIter);

        if (m_nameToListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        delete iter->second;
        m_nameToListMap.erase(iter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::EraseAllContent()
{
    for (NameToListMap::iterator iter = m_nameToListMap.begin(); iter != m_nameToListMap.end();)
    {
        delete iter->second;
        m_nameToListMap.erase(iter++);
    }

    m_currentListName = NULL_LIST_NAME;
    m_nameToListMap.clear();
    m_savedLists.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::CreateInitialLists()
{
    if (!m_nameToListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToListMap[NULL_LIST_NAME] = new ObjectList;
    m_savedLists.insert(NULL_LIST_NAME);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::ResetForNextEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->EraseAllContent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode Manager<arborpfa::Connector>::DropCurrentList()
{
    m_currentListName = NULL_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}



/***********************************************************
 * AlgorithmObjectManager<Connector> template implementation
 ***********************************************************/

template<>
AlgorithmObjectManager<arborpfa::Connector>::AlgorithmObjectManager() :
    Manager<arborpfa::Connector>(),
    m_canMakeNewObjects(false)
{

}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
AlgorithmObjectManager<arborpfa::Connector>::~AlgorithmObjectManager()
{

}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    m_canMakeNewObjects = true;
    return Manager<arborpfa::Connector>::CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::MoveObjectsBetweenLists(const std::string &targetListName, const std::string &sourceListName,
    const ObjectList *pObjectSubset)
{
    Manager<arborpfa::Connector>::NameToListMap::iterator sourceListIter = Manager<arborpfa::Connector>::m_nameToListMap.find(sourceListName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == sourceListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    Manager<arborpfa::Connector>::NameToListMap::iterator targetListIter = Manager<arborpfa::Connector>::m_nameToListMap.find(targetListName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == targetListIter)
        return STATUS_CODE_FAILURE;

    if (NULL == pObjectSubset)
    {
        for (ObjectList::iterator iter = sourceListIter->second->begin(), iterEnd = sourceListIter->second->end();
            iter != iterEnd; ++iter)
        {
            if (!targetListIter->second->insert(*iter).second)
                return STATUS_CODE_ALREADY_PRESENT;
        }

        sourceListIter->second->clear();
    }
    else
    {
        if ((sourceListIter->second == pObjectSubset) || (targetListIter->second == pObjectSubset))
            return STATUS_CODE_INVALID_PARAMETER;

        for (ObjectList::const_iterator iter = pObjectSubset->begin(), iterEnd = pObjectSubset->end(); iter != iterEnd; ++iter)
        {
            ObjectList::iterator objectIter = sourceListIter->second->find(*iter);

            if (sourceListIter->second->end() == objectIter)
                return STATUS_CODE_NOT_FOUND;

            if (!targetListIter->second->insert(*objectIter).second)
                return STATUS_CODE_ALREADY_PRESENT;

            sourceListIter->second->erase(objectIter);
        }
    }

    m_canMakeNewObjects = false;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::MoveObjectsToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
    std::string &temporaryListName, const ObjectList &objectsToMove)
{
    if (objectsToMove.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MoveObjectsBetweenLists(temporaryListName, originalListName, &objectsToMove));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::SaveObjects(const std::string &targetListName, const std::string &sourceListName)
{
    Manager<arborpfa::Connector>::NameToListMap::iterator targetObjectListIter = Manager<arborpfa::Connector>::m_nameToListMap.find(targetListName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<arborpfa::Connector>::m_nameToListMap[targetListName] = new ObjectList;
        Manager<arborpfa::Connector>::m_savedLists.insert(targetListName);
    }

    return this->MoveObjectsBetweenLists(targetListName, sourceListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::SaveObjects(const std::string &targetListName, const std::string &sourceListName, const ObjectList &objectsToSave)
{
    if (objectsToSave.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    Manager<arborpfa::Connector>::NameToListMap::iterator targetObjectListIter = Manager<arborpfa::Connector>::m_nameToListMap.find(targetListName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<arborpfa::Connector>::m_nameToListMap[targetListName] = new ObjectList;
        Manager<arborpfa::Connector>::m_savedLists.insert(targetListName);
    }

    return this->MoveObjectsBetweenLists(targetListName, sourceListName, &objectsToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::TemporarilyReplaceCurrentList(const std::string &listName)
{
    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == Manager<arborpfa::Connector>::m_nameToListMap.find(listName))
        return STATUS_CODE_NOT_FOUND;

    m_canMakeNewObjects = false;
    Manager<arborpfa::Connector>::m_currentListName = listName;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::DeleteObject(arborpfa::Connector *pT, const std::string &listName)
{
    Manager<arborpfa::Connector>::NameToListMap::iterator listIter = Manager<arborpfa::Connector>::m_nameToListMap.find(listName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    ObjectList::iterator deletionIter = listIter->second->find(pT);

    if (listIter->second->end() == deletionIter)
        return STATUS_CODE_NOT_FOUND;

    delete pT;
    listIter->second->erase(deletionIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::DeleteObjects(const ObjectList &objectList, const std::string &listName)
{
    Manager<arborpfa::Connector>::NameToListMap::iterator listIter = Manager<arborpfa::Connector>::m_nameToListMap.find(listName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    if (listIter->second == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (ObjectList::const_iterator objectIter = objectList.begin(), objectIterEnd = objectList.end(); objectIter != objectIterEnd; ++objectIter)
    {
        ObjectList::iterator deletionIter = listIter->second->find(*objectIter);

        if (listIter->second->end() == deletionIter)
            return STATUS_CODE_NOT_FOUND;

        delete *objectIter;
        listIter->second->erase(deletionIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::DeleteTemporaryObjects(const Algorithm *const pAlgorithm, const std::string &temporaryListName)
{
    if (Manager<arborpfa::Connector>::m_savedLists.end() != Manager<arborpfa::Connector>::m_savedLists.find(temporaryListName))
        return STATUS_CODE_NOT_ALLOWED;

    Manager<arborpfa::Connector>::AlgorithmInfoMap::const_iterator algorithmIter = Manager<arborpfa::Connector>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<arborpfa::Connector>::m_algorithmInfoMap.end() == algorithmIter)
        return STATUS_CODE_NOT_FOUND;

    if (algorithmIter->second.m_temporaryListNames.end() == algorithmIter->second.m_temporaryListNames.find(temporaryListName))
        return STATUS_CODE_NOT_ALLOWED;

    Manager<arborpfa::Connector>::NameToListMap::iterator listIter = Manager<arborpfa::Connector>::m_nameToListMap.find(temporaryListName);

    if (Manager<arborpfa::Connector>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_FAILURE;

    for (ObjectList::iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
        delete *iter;

    listIter->second->clear();
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::GetResetDeletionObjects(const Algorithm *const pAlgorithm, ObjectList &objectList) const
{
    Manager<arborpfa::Connector>::AlgorithmInfoMap::const_iterator algorithmIter = Manager<arborpfa::Connector>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<arborpfa::Connector>::m_algorithmInfoMap.end() == algorithmIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        Manager<arborpfa::Connector>::NameToListMap::const_iterator listIter = Manager<arborpfa::Connector>::m_nameToListMap.find(*listNameIter);

        if (Manager<arborpfa::Connector>::m_nameToListMap.end() == listIter)
            return STATUS_CODE_FAILURE;

        objectList.insert(listIter->second->begin(), listIter->second->end());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    m_canMakeNewObjects = false;
    return Manager<arborpfa::Connector>::ResetCurrentListToAlgorithmInputList(pAlgorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &listName)
{
    m_canMakeNewObjects = false;
    return Manager<arborpfa::Connector>::ReplaceCurrentAndAlgorithmInputLists(pAlgorithm, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    ObjectList objectList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetResetDeletionObjects(pAlgorithm, objectList));

    for (ObjectList::const_iterator iter = objectList.begin(), iterEnd = objectList.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_canMakeNewObjects = false;
    return Manager<arborpfa::Connector>::ResetAlgorithmInfo(pAlgorithm, isAlgorithmFinished);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::EraseAllContent()
{
    for (Manager<arborpfa::Connector>::NameToListMap::iterator listIter = Manager<arborpfa::Connector>::m_nameToListMap.begin(), listIterEnd = Manager<arborpfa::Connector>::m_nameToListMap.end();
        listIter != listIterEnd; ++listIter)
    {
        for (ObjectList::iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
            delete (*iter);
    }

    m_canMakeNewObjects = false;
    return Manager<arborpfa::Connector>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<>
StatusCode AlgorithmObjectManager<arborpfa::Connector>::DropCurrentList()
{
    m_canMakeNewObjects = false;
    return Manager<arborpfa::Connector>::DropCurrentList();
}

}

// template initialization
template class pandora::Manager<arborpfa::Connector>;
template class pandora::AlgorithmObjectManager<arborpfa::Connector>;


/*********************************
 * ConnectorManager implementation
 *********************************/

#include "arborpfa/algorithm/ArborAlgorithm.h"

using namespace pandora;

namespace arborpfa
{

ConnectorManager::ConnectorManager()
 : AlgorithmObjectManager<Connector>()
{

}

ConnectorManager::~ConnectorManager()
{

}


pandora::StatusCode ConnectorManager::CreateConnector(ArborObject *pObj1, ArborObject *pObj2, Connector *&pConnector, float weight)
{
	pConnector = NULL;

	try
	{
  if (!m_canMakeNewObjects)
   throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

  NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

  if (m_nameToListMap.end() == iter)
       throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

  pConnector = new ConnectorImpl(pObj1, pObj2, weight);

  if (NULL == pConnector)
       throw StatusCodeException(STATUS_CODE_FAILURE);

  if (!iter->second->insert(pConnector).second)
       throw StatusCodeException(STATUS_CODE_FAILURE);

  return STATUS_CODE_SUCCESS;
	}
	catch(StatusCodeException &e)
	{
  std::cout << "Failed to create connector: " << e.ToString() << std::endl;
  delete pConnector;
  return e.GetStatusCode();
	}

}



pandora::StatusCode ConnectorManager::CreateTemporaryListAndSetCurrent(const ArborAlgorithm *pAlgorithm, std::string &temporaryListName)
{
	return AlgorithmObjectManager<arborpfa::Connector>::CreateTemporaryListAndSetCurrent(pAlgorithm, temporaryListName);
}


pandora::StatusCode	ConnectorManager::ReplaceCurrentAndAlgorithmInputLists(const ArborAlgorithm *algorithm, const std::string &newListName)
{
	return AlgorithmObjectManager<arborpfa::Connector>::ReplaceCurrentAndAlgorithmInputLists(algorithm, newListName);
}


}


