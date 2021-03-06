  /// \file MipTrackCandidateTaggingAlgorithm.cc
/*
 *
 * MipTrackCandidateTaggingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : lun. mars 16 2015
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


#include "arborpfa/algorithm/MipTrackCandidateTaggingAlgorithm.h"

// arbor
#include "arborpfa/arbor/AlgorithmHeaders.h"

// pandora
#include "Pandora/PandoraInternal.h"

namespace arbor
{

//-----------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipTrackCandidateTaggingAlgorithm::RunArborAlgorithm()
{
	const ObjectList *pObjectList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentObjectList(*this, pObjectList));

	if(pObjectList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	for(ObjectList::const_iterator iterI = pObjectList->begin(), endIterI = pObjectList->end() ;
			endIterI != iterI ; ++iterI)
	{
		Object *pObjectI = *iterI;

		// reset the mip flag before processing
		if(pObjectI->GetFlag(MIP_OBJECT))
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pObjectI->SetFlag(MIP_OBJECT, false));

		// ignore big clusters or objects built from big split clusters
		if(pObjectI->GetNCaloHits() > m_maxNHitTrackCandidate)// || pObjectI->GetFlag(SPLIT_FROM_BIG_CLUSTER))
			continue;

		unsigned int neighborsCounter = 0;
		unsigned int bigNeighborsCounter = 0;

		for(ObjectList::const_iterator iterJ = pObjectList->begin(), endIterJ = pObjectList->end() ;
				endIterJ != iterJ ; ++iterJ)
		{
			Object *pObjectJ = *iterJ;

			if(pObjectI == pObjectJ)
				continue;

			if(pObjectI->GetPseudoLayer() != pObjectJ->GetPseudoLayer())
				continue;

			// we found a neighbor in the same pseudo layer
			if((pObjectI->GetPosition() - pObjectJ->GetPosition()).GetMagnitude() < m_intraLayerNeighborDistance)
			{
				neighborsCounter++;

				if(pObjectJ->GetNCaloHits() > m_maxNHitTrackCandidate || pObjectJ->GetFlag(SPLIT_FROM_BIG_CLUSTER))
					bigNeighborsCounter++;
			}
		}

		if(neighborsCounter < m_maxNNeighbors && bigNeighborsCounter < m_maxNBigNeighbors)
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pObjectI->SetFlag(MIP_OBJECT, true));

	}

	return pandora::STATUS_CODE_SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipTrackCandidateTaggingAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_maxNHitTrackCandidate = 4;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxNHitTrackCandidate", m_maxNHitTrackCandidate));

	m_maxNNeighbors = 2;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
				    "MaxNNeighbors", m_maxNNeighbors));

	m_maxNBigNeighbors = 2;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		    "MaxNBigNeighbors", m_maxNBigNeighbors));

	m_intraLayerNeighborDistance = 50.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
				    "IntraLayerNeighborDistance", m_intraLayerNeighborDistance));

	return pandora::STATUS_CODE_SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------------------

} 

