/*
 *
 * ArborConnectorClusteringAlgorithm.h header template automatically generated by a class generator
 * Creation date : jeu. f�vr. 20 2014
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


#ifndef ARBORCONNECTORCLUSTERINGALGORITHM_H
#define ARBORCONNECTORCLUSTERINGALGORITHM_H

#include "arborpfa/algorithm/ArborAlgorithm.h"
#include "arborpfa/algorithm/ArborAlgorithmFactory.h"

#include "arborpfa/arbor/AlgorithmHeaders.h"

#include "TEveElement.h"

namespace arbor
{

/** 
 * @brief ArborConnectorClusteringAlgorithm class
 */ 
class ArborConnectorClusteringAlgorithm : public ArborAlgorithm
{
 public:

		/**
			* @brief Factory class for instantiating algorithm
			*/
		class Factory : public ArborAlgorithmFactory
		{
			public:
				ArborAlgorithm *CreateArborAlgorithm() const;
		};

		/**
			* @brief Run the algorithm
			*/
		pandora::StatusCode RunArborAlgorithm();

		/**
			* @brief Read settings from Xml handler
			*/
		pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

		/**
		 * @brief Seed the initial connectors. </br>
		 * Part of them will be removed/sorted/re-weighted in the following part </br>
		 * Called after arbor object creation
		 */
		pandora::StatusCode SeedInitialConnectors();

		/**
		 * @brief Clean the connectors. </br>
		 * Keep at least one connector in the backward direction (arbor principle) by taking a decision among
		 * all backward connectors. </br>
		 * Called after arbor objects creation and connector seeding.
		 */
		pandora::StatusCode CleanConnectors();

		/**
		 *
		 */
		pandora::StatusCode FirstConnectorCleaning();

		/**
		 *
		 */
		pandora::StatusCode SecondConnectorCleaning();

		/**
		 * @brief Cluster all the connected arbor objects. </br>
		 * Called after connector cleaning
		 */
		pandora::StatusCode DoClustering();

		/**
		 * @brief Clear all content, all the objects allocated on the heap.
		 */
		pandora::StatusCode ClearContent();

//-----------------------------------------------------------------------------------------------------------------

		/**
		 *
		 */
		pandora::StatusCode DrawConnectors() const;

		/**
		 *
		 */
		pandora::StatusCode DrawEveArrow(TEveElement *pParentCollection, const pandora::CartesianVector &from, const pandora::CartesianVector &to, int color) const;

 protected:

		// list and function names
		std::string        m_trackListName;  // for track-cluster association
		std::string        m_hcalEnergyResolutionFunctionName;

		// algorithm tools
		const ObjectList                            *m_pObjectList;
		ObjectList                                    m_abjectList;
		OrderedObjectList                             m_orderedObjectList;
		ConnectorList                                 m_connectorList;
		ConnectorList                                 m_secondCleaningConnectors;
		ClusterList                                   m_clusterList;

		// algorithm parameters
		unsigned int       m_referenceDirectionDepth;
		unsigned int       m_referenceDirectionMaximumForwardLayer;

		float               m_maximumDistanceForConnectionCoarse;
		float               m_maximumDistanceForConnectionFine;
		float               m_maximumDistanceForConnectionCoarse2;
		float               m_maximumDistanceForConnectionFine2;
		float               m_angleForSecondCleaningCoarse;
		float               m_angleForSecondCleaningFine;
		float               m_orderParameterAnglePower;
		float               m_orderParameterDistancePower;
		float               m_forwardConnectorWeight;
		float               m_backwardConnectorWeight;

		bool                m_allowForwardConnectionForIsolatedObjects;
		bool                m_showConnectors;
		bool                m_shouldUseIsolatedObjects;
		bool                m_shouldRunSecondCleaning;
}; 

//--------------------------------------------------------------------------------------------------------------------

inline ArborAlgorithm *ArborConnectorClusteringAlgorithm::Factory::CreateArborAlgorithm() const
{
	return new ArborConnectorClusteringAlgorithm();
}

} 

#endif  //  ARBORCONNECTORCLUSTERINGALGORITHM_H
