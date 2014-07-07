  /// \file ArborReclusteringAlgorithm.cc
/*
 *
 * ArborReclusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mer. juin 18 2014
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


#include "arborpfa/algorithm/ArborReclusteringAlgorithm.h"

// pandora
#include "Api/PandoraContentApi.h"

// arborpfa
#include "arborpfa/api/ArborContentApi.h"
#include "arborpfa/arbor/AlgorithmHeaders.h"

namespace arbor
{

pandora::StatusCode ArborReclusteringAlgorithm::RunArborAlgorithm()
{
	// grab the cluster content
	const arbor::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentClusterList(*this, pClusterList));
	std::string currentClusterListName;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentClusterListName(*this, currentClusterListName));

	// set the current energy resolution function to use in this algorithm
	std::string originalEnergyResolutionFunctionName;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentEnergyResolutionFunctionName(*this, originalEnergyResolutionFunctionName));

	if(originalEnergyResolutionFunctionName != m_energyResolutionFunctionName)
	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::SetCurrentEnergyResolutionFunction(*this, m_energyResolutionFunctionName));

	// set the current energy estimator to use in this algorithm
	std::string originalEnergyEstimatorName;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetCurrentEnergyEstimatorName(*this, originalEnergyEstimatorName));

	if(originalEnergyEstimatorName != m_energyEstimatorName)
	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::SetCurrentEnergyEstimator(*this, m_energyEstimatorName));

	arbor::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
	std::sort(clusterVector.begin(), clusterVector.end(), arbor::Cluster::SORT_BY_SEED_PSEUDO_LAYER);

	const unsigned int nClusters(clusterVector.size());

	for(unsigned int i=0 ; i<nClusters ; i++)
	{
		arbor::Cluster *pCluster = clusterVector.at(i);

		if(NULL == pCluster)
			continue;

		const pandora::Track *pTrack = NULL;
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pCluster->GetAssociatedTrack(pTrack));

		if(NULL == pTrack)
			continue;

		const float trackMomemtumAtCalorimeter = pTrack->GetTrackStateAtCalorimeter().GetMomentum().GetMagnitude();

		float calorimeterEnergyResolution(0.f);
		PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::GetEnergyResolution(*this, trackMomemtumAtCalorimeter, calorimeterEnergyResolution));

		float estimatedClusterEnergy(0.f);
		PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::EstimateEnergy(*this, pCluster, estimatedClusterEnergy));

		const float minEnergyResolution = trackMomemtumAtCalorimeter - (m_nResolutionTrackClusterComparison*trackMomemtumAtCalorimeter*calorimeterEnergyResolution);
		const float maxEnergyResolution = trackMomemtumAtCalorimeter + (m_nResolutionTrackClusterComparison*trackMomemtumAtCalorimeter*calorimeterEnergyResolution);

		const bool hasEnergyExcess  = (maxEnergyResolution < estimatedClusterEnergy);
		const bool hasMissingEnergy = (minEnergyResolution > estimatedClusterEnergy);
		const bool shouldRecluster  = (hasEnergyExcess || hasMissingEnergy);
		const float chi2 = std::pow(estimatedClusterEnergy - trackMomemtumAtCalorimeter, 2) / trackMomemtumAtCalorimeter;

		ARBOR_PRINT( "Cluster with estimated energy of " << estimatedClusterEnergy << " GeV "
				<< (shouldRecluster ? "is not" : "is") << " in energy resolution range." << std::endl );
		if(shouldRecluster)
			ARBOR_PRINT( "Should run a reclustering algorithm" << std::endl );

		ARBOR_PRINT( "original chi2 : " << chi2 << std::endl );

		if(!shouldRecluster)
			continue;

		arbor::ClusterList reclusterClusterList;
		reclusterClusterList.insert(pCluster);

		for(unsigned int j=0 ; j<nClusters ; j++)
		{
			arbor::Cluster *pNearbyCluster = clusterVector.at(j);

			if(pNearbyCluster == pCluster || NULL == pNearbyCluster)
				continue;

			float contactDistance(0.f);

			if(pandora::STATUS_CODE_SUCCESS != ArborHelper::GetClosestDistanceApproach(pCluster, pNearbyCluster, contactDistance))
				continue;

			if(m_contactDistanceThreshold > contactDistance)
				continue;

			const pandora::Track *pNearbyClusterTrack = NULL;
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pNearbyCluster->GetAssociatedTrack(pNearbyClusterTrack));

			if(NULL != pNearbyClusterTrack)
				continue;

			reclusterClusterList.insert(pNearbyCluster);
		}

		ARBOR_PRINT( "For this cluster, " << reclusterClusterList.size() - 1 << " nearby clusters "
				"will be added to the reclustering step" << std::endl );

		// Do not run a re-clustering if there is no nearby cluster and
		// a missing energy in the cluster has been detected.
		// It means that we can't do anything more to save the cluster ...
		if(1 == reclusterClusterList.size() && hasMissingEnergy)
		{
			ARBOR_PRINT( "Can't do more for this cluster ..." << std::endl );
			continue;
		}

		// Initialize the reclustering with these cluster.
  std::string originalClustersListName;
  PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::InitializeReclustering(*this,
      reclusterClusterList, originalClustersListName));

  std::string bestReclusteringListName(originalClustersListName);
  float bestReclusteringChi2(chi2);
//  unsigned int bestNReclusteringClusters = reclusterClusterList.size();

  for(StringVector::iterator algoIter = m_clusteringAlgorithmList.begin() , algoEndIter = m_clusteringAlgorithmList.end() ;
  		algoEndIter != algoIter ; ++algoIter)
  {
  	std::string reclusterListName;
  	const arbor::ClusterList *pReclusterList = NULL;
  	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::RunReclusteringAlgorithm(*this, *algoIter,
  	  pReclusterList, reclusterListName, false));

  	if(pReclusterList->empty())
  		continue;

  	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

  	const arbor::Cluster *pAssociatedCluster = NULL;
  	pandora::StatusCode statusCode = this->FindClusterAssociatedToTrack(pReclusterList, pTrack, pAssociatedCluster);

  	if(pandora::STATUS_CODE_SUCCESS != statusCode && pandora::STATUS_CODE_NOT_FOUND != statusCode)
  		return statusCode;

  	if(pandora::STATUS_CODE_NOT_FOUND == statusCode)
  		continue;

 		float newEstimatedClusterEnergy(0.f);
 		PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::EstimateEnergy(*this, pAssociatedCluster, newEstimatedClusterEnergy));

  	float chi2 = std::pow(newEstimatedClusterEnergy - trackMomemtumAtCalorimeter, 2) / trackMomemtumAtCalorimeter;
 		const bool hasEnergyExcess2  = (maxEnergyResolution < newEstimatedClusterEnergy);
 		const bool hasMissingEnergy2 = (minEnergyResolution > newEstimatedClusterEnergy);
 		const bool inResolutionRange  = !(hasEnergyExcess2 || hasMissingEnergy2);

  	ARBOR_PRINT( "newEstimatedClusterEnergy : " << newEstimatedClusterEnergy << std::endl );
  	ARBOR_PRINT( "current chi2 : " << chi2 << std::endl );
  	ARBOR_PRINT( "pReclusterList->size() : " << pReclusterList->size() << std::endl );

			if(chi2 < bestReclusteringChi2)
			{
				bestReclusteringListName = reclusterListName;
				bestReclusteringChi2 = chi2;
			}
  	else
  	{
  		continue;
  	}

  	if(chi2 < m_chi2ForAutomaticClusterSelection)
  		break;
  }

  ARBOR_PRINT( "=== FINAL CHOICE ===" << std::endl );
  ARBOR_PRINT( "bestReclusteringListName : " << bestReclusteringListName << std::endl );
  ARBOR_PRINT( "bestReclusteringChi2 : " << bestReclusteringChi2 << std::endl );

  PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::EndReclustering(*this,
  		bestReclusteringListName));
	}

	// restore the original functions if they are different
	if(originalEnergyResolutionFunctionName != m_energyResolutionFunctionName)
		 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::SetCurrentEnergyResolutionFunction(*this, originalEnergyResolutionFunctionName));

	if(originalEnergyEstimatorName != m_energyEstimatorName)
		 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::SetCurrentEnergyEstimator(*this, originalEnergyEstimatorName));

 return pandora::STATUS_CODE_SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborReclusteringAlgorithm::FindClusterAssociatedToTrack(const arbor::ClusterList *pClusterList,
		const pandora::Track *pAssociatedTrack, const arbor::Cluster *&pAssociatedCluster)
{
	if(NULL == pAssociatedTrack)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	for(arbor::ClusterList::const_iterator iter = pClusterList->begin() , endIter = pClusterList->end() ; endIter != iter ; ++iter)
	{
		const arbor::Cluster *pCluster = *iter;
		const pandora::Track *pTrack = NULL;

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pCluster->GetAssociatedTrack(pTrack));

		if(NULL == pTrack)
			continue;

		if(pAssociatedTrack == pTrack)
		{
			pAssociatedCluster = pCluster;
			return pandora::STATUS_CODE_SUCCESS;
		}
	}

	return pandora::STATUS_CODE_NOT_FOUND;
}

//--------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborReclusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
	     "clusteringAlgorithms", m_clusteringAlgorithmList));

	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
	     "AssociationAlgorithm", m_associationAlgorithmName));

	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "EnergyResolutionFunctionName", m_energyResolutionFunctionName));

	 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "EnergyEstimatorName", m_energyEstimatorName));

	 m_nResolutionTrackClusterComparison = 2.f;
	 PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	 	   "NResolutionTrackClusterComparison", m_nResolutionTrackClusterComparison));

	 m_contactDistanceThreshold = 10.f;
	 PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	 	   "ContactDistanceThreshold", m_contactDistanceThreshold));

	 m_chi2ForAutomaticClusterSelection = 1.f;
	 PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	 	   "Chi2ForAutomaticClusterSelection", m_chi2ForAutomaticClusterSelection));

	return pandora::STATUS_CODE_SUCCESS;
}


} 

