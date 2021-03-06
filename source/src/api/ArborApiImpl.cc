/*
 *
 * ArborApiImpl.cc source template automatically generated by a class generator
 * Creation date : lun. f�vr. 10 2014
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


// arborpfa
#include "arborpfa/arbor/Arbor.h"
#include "arborpfa/api/ArborApiImpl.h"
//#include "arborpfa/content/ObjectManager.h"
#include "arborpfa/content/ArborPluginManager.h"
#include "arborpfa/algorithm/ArborAlgorithmFactory.h"

// parent algorithms
#include "arborpfa/algorithm/ObjectCreationParentAlgorithm.h"
#include "arborpfa/algorithm/ConnectorParentAlgorithm.h"
#include "arborpfa/algorithm/ConnectorClusteringParentAlgorithm.h"
#include "arborpfa/algorithm/DummyClusteringAlgorithm.h"

// impl algorithms
#include "arborpfa/algorithm/SimpleObjectCreationAlgorithm.h"
#include "arborpfa/algorithm/IsolationTaggingAlgorithm.h"
#include "arborpfa/algorithm/ConnectorSeedingAlgorithm.h"
#include "arborpfa/algorithm/KappaConnectorCleaningAlgorithm.h"
#include "arborpfa/algorithm/TreeClusteringAlgorithm.h"
#include "arborpfa/algorithm/TopologicalTrackAssociationAlgorithm.h"
#include "arborpfa/algorithm/EnergyDrivenTrackAssociationAlgorithm.h"
#include "arborpfa/algorithm/NeutralTreeMergingAlgorithm.h"
#include "arborpfa/algorithm/SmallNeutralFragmentMergingAlgorithm.h"
#include "arborpfa/algorithm/PointingClusterAssociationAlgorithm.h"
#include "arborpfa/algorithm/MipTrackCandidateTaggingAlgorithm.h"
#include "arborpfa/algorithm/PrimaryTrackConnectionAlgorithm.h"
#include "arborpfa/algorithm/ArborClusterConverterAlgorithm.h"
//#include "arborpfa/algorithm/ArborReclusteringAlgorithm.h"

// monitoring
#include "arborpfa/algorithm/ArborMonitoringAlgorithm.h"
#include "arborpfa/algorithm/PfoMonitoringAlgorithm.h"

// plugins
#include "arborpfa/content/SdhcalEnergyFunction.h"
#include "arborpfa/content/EcalSdhcalEnergyFunction.h"

// pandora
#include "Pandora/Pandora.h"
#include "Api/PandoraApi.h"

using namespace pandora;

namespace arbor
{

//---------------------------------------------------------------------------------------------------------------

ArborApiImpl::ArborApiImpl(Arbor *pArbor) :
		m_pArbor(pArbor)
{

}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::PrepareEvent() const
{
	return m_pArbor->PrepareEvent();
}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::RegisterAlgorithmFactory(Arbor &arbor, const std::string &algorithmType, ArborAlgorithmFactory *pFactory) const
{
	 PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterAlgorithmFactory(*arbor.GetPandora(), algorithmType, pFactory));
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pFactory->RegisterArbor(&arbor));

		return STATUS_CODE_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::RegisterArborAlgorithms(Arbor &arbor) const
{
	// parent algorithms
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ObjectCreationParent",
			  new arbor::ObjectCreationParentAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ConnectorClusteringParent",
			  new arbor::ConnectorClusteringParentAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ConnectorParent",
			  new arbor::ConnectorParentAlgorithm::Factory));

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "DummyClustering",
			  new arbor::DummyClusteringAlgorithm::Factory));

	// object creation algorithm
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "SimpleObjectCreation",
			  new arbor::SimpleObjectCreationAlgorithm::Factory));

	// additional event preparation algorithms
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "IsolationTagging",
				  new arbor::IsolationTaggingAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "MipTrackCandidateTagging",
				  new arbor::MipTrackCandidateTaggingAlgorithm::Factory));

	// connector algorithms
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ConnectorSeeding",
				  new arbor::ConnectorSeedingAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "KappaConnectorCleaning",
				  new arbor::KappaConnectorCleaningAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "TreeClustering",
					  new arbor::TreeClusteringAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "PrimaryTrackConnection",
					  new arbor::PrimaryTrackConnectionAlgorithm::Factory));

	// associations algorithms
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "TopologicalTrackAssociation",
			  new arbor::TopologicalTrackAssociationAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "EnergyDrivenTrackAssociation",
			  new arbor::EnergyDrivenTrackAssociationAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "NeutralTreeMerging",
			  new arbor::NeutralTreeMergingAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "SmallNeutralFragmentMerging",
			  new arbor::SmallNeutralFragmentMergingAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "PointingClusterAssociation",
			  new arbor::PointingClusterAssociationAlgorithm::Factory));

	// additional algorithms
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ArborClusterConverter",
			  new arbor::ArborClusterConverterAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ArborMonitoring",
				  new arbor::ArborMonitoringAlgorithm::Factory));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ArborPfoMonitoring",
				  new arbor::PfoMonitoringAlgorithm::Factory));

//	// reclustering
//	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RegisterAlgorithmFactory(arbor, "ArborReclustering",
//				  new arbor::ArborReclusteringAlgorithm::Factory));

	return pandora::STATUS_CODE_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::RegisterArborPlugins(Arbor &arbor) const
{
 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterEnergyFunction(arbor, "SdhcalEnergyFunction",
     new SdhcalEnergyFunction()));

 PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->RegisterEnergyFunction(arbor, "EcalSdhcalEnergyFunction",
     new EcalSdhcalEnergyFunction()));

	return pandora::STATUS_CODE_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::RegisterEnergyFunction(Arbor &pArbor, const std::string &energyFunctionName, IEnergyFunction *pEnergyFunction) const
{
	return pArbor.m_pArborPluginManager->RegisterEnergyFunction(energyFunctionName, pEnergyFunction);
}

//---------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborApiImpl::ResetEvent() const
{
	return m_pArbor->ResetEvent();
}

//---------------------------------------------------------------------------------------------------------------

} 

