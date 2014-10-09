/*
 *
 * ArborHelper.cc source template automatically generated by a class generator
 * Creation date : lun. mars 10 2014
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


#include "arborpfa/content/ArborHelper.h"

#include "Objects/Cluster.h"
#include "Objects/CartesianVector.h"
#include "Helpers/ClusterHelper.h"

#include "arborpfa/content/Object.h"
#include "arborpfa/content/Cluster.h"
#include "arborpfa/content/Connector.h"

using namespace pandora;

namespace arbor
{

//-----------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetCentroid(const arbor::Cluster *pCluster, pandora::CartesianVector &centroid)
{
	if(NULL == pCluster)
		return STATUS_CODE_FAILURE;

	centroid.SetValues(0.f, 0.f, 0.f);
	const ObjectList clusterObjectList = pCluster->GetObjectList();

	for(arbor::ObjectList::const_iterator iter = clusterObjectList.begin() , endIter = clusterObjectList.end() ; endIter != iter ; ++iter)
	{
		centroid += (*iter)->GetPosition();
	}

	centroid = centroid * (1.0/clusterObjectList.size());

	return STATUS_CODE_SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetCentroidDifference(const arbor::Cluster *pCluster1, const arbor::Cluster *pCluster2, float &centroidDifference)
{

	CartesianVector centroid1(0.f, 0.f, 0.f);
	CartesianVector centroid2(0.f, 0.f, 0.f);

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::GetCentroid(pCluster1, centroid1));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::GetCentroid(pCluster2, centroid2));

	centroidDifference = (centroid1 - centroid2).GetMagnitude();

	return STATUS_CODE_SUCCESS;
}

//--------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetClosestDistanceApproach(const arbor::Cluster *pCluster1, const arbor::Cluster *pCluster2, float &closestDistance)
{
	if(NULL == pCluster1 || NULL == pCluster2)
		return STATUS_CODE_INVALID_PARAMETER;

	if(0 == pCluster1->GetNObjects() || 0 == pCluster2->GetNObjects())
		return STATUS_CODE_FAILURE;

	const ObjectList objectList1 = pCluster1->GetObjectList();
	const ObjectList objectList2 = pCluster2->GetObjectList();

	closestDistance = std::numeric_limits<float>::max();

	for(arbor::ObjectList::const_iterator iter1 = objectList1.begin() , endIter1 = objectList1.end() ; endIter1 != iter1 ; ++iter1)
	{
		Object *pObject1 = *iter1;

		for(arbor::ObjectList::const_iterator iter2 = objectList2.begin() , endIter2 = objectList2.end() ; endIter2 != iter2 ; ++iter2)
		{
			Object *pObject2 = *iter2;

			float distance = (pObject1->GetPosition() - pObject2->GetPosition()).GetMagnitude();

			if(closestDistance > distance)
			{
				closestDistance = distance;
			}
		}
	}

	return STATUS_CODE_SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::FitPoints(const CartesianPointList &pointList, pandora::ClusterHelper::ClusterFitResult &clusterFitResult)
{
 try
 {
		const unsigned int nFitPoints(pointList.size());

		if (nFitPoints < 2)
			return STATUS_CODE_FAILURE;

		clusterFitResult.Reset();

		float xSum = 0.f;
		float ySum = 0.f;
		float zSum = 0.f;
		float xzSum = 0.f;
		float yzSum = 0.f;
		float zzSum = 0.f;

  float fitParam1 = 0.f;
  float fitParam2 = 0.f;
  float fitParam3 = 0.f;
  float fitParam4 = 0.f;

		for(CartesianPointList::const_iterator iter = pointList.begin(), iterEnd = pointList.end(); iter != iterEnd; ++iter)
		{
			CartesianVector position = *iter;

   xSum += position.GetX();
   ySum += position.GetY();
   zSum += position.GetZ();

   xzSum += position.GetX()*position.GetZ();
   yzSum += position.GetY()*position.GetZ();
   zzSum += position.GetZ()*position.GetZ();
		}

  fitParam1 = (zzSum*xSum - xzSum*zSum) / (nFitPoints*zzSum-zSum*zSum);
  fitParam2 = (xzSum*nFitPoints - xSum*zSum) / (nFitPoints*zzSum-zSum*zSum);
  fitParam3 = (zzSum*ySum - yzSum*zSum) / (nFitPoints*zzSum-zSum*zSum);
  fitParam4 = (yzSum*nFitPoints - ySum*zSum) / (nFitPoints*zzSum-zSum*zSum);

  float chi2 = 0.f;

  for(CartesianPointList::const_iterator iter = pointList.begin(), iterEnd = pointList.end(); iter != iterEnd; ++iter)
		{
  	CartesianVector position = *iter;

			CartesianVector x0(fitParam1, fitParam3, 0.f);
	  CartesianVector x1(fitParam1 + fitParam2, fitParam3 + fitParam4, 1.f);
	  CartesianVector u = (x1-x0);
		 u = u.GetUnitVector() * (position-x1).GetMagnitude() * std::cos(u.GetCosOpeningAngle(position - x1));

			chi2 += (position - (x1+u)).GetMagnitude();
		}

  CartesianVector point1(fitParam2 + fitParam1, fitParam4 + fitParam3, 1.f);
  CartesianVector point2(fitParam2*2.f + fitParam1, fitParam4*2.f + fitParam3, 2.f);
  CartesianVector direction = (point2 - point1).GetUnitVector();

  clusterFitResult.SetChi2(chi2);
  clusterFitResult.SetRms(chi2 / static_cast<float>(nFitPoints));
  clusterFitResult.SetDirection(direction);
  clusterFitResult.SetSuccessFlag(true);
 }
 catch (StatusCodeException &statusCodeException)
 {
     std::cout << "ArborHelper: linear fit failed. " << std::endl;
     clusterFitResult.SetSuccessFlag(false);
     return statusCodeException.GetStatusCode();
 }

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetReferenceDirection(const Object *pObject,	float backwardConnectorWeight,
		float forwardConnectorWeight, pandora::CartesianVector &meanBackwardDirection)
{
	meanBackwardDirection = pandora::CartesianVector(0.f, 0.f, 0.f);

	if(NULL == pObject)
	{
		std::cout << "ERROR : ArborHelper::GetReferenceDirection() : pObject is NULL" << std::endl;
		return STATUS_CODE_INVALID_PARAMETER;
	}

	const ConnectorList &connectorList = pObject->GetConnectorList();
	const CartesianVector &objectPosition = pObject->GetPosition();

	for(ConnectorList::const_iterator iter = connectorList.begin() , endIter = connectorList.end() ; endIter != iter ; ++iter)
	{
		const Connector *pConnector = *iter;
		const Object *pOtherObject = NULL;

		if(pObject == pConnector->GetFirst())
		{
			pOtherObject = pConnector->GetSecond();
		}
		else
		{
			pOtherObject = pConnector->GetFirst();
		}

		CartesianVector differencePosition = pOtherObject->GetPosition() - objectPosition;

		if(!pOtherObject->IsBackwardConnector(pConnector))
		{
			meanBackwardDirection += differencePosition * - forwardConnectorWeight;
		}
		else
		{
			meanBackwardDirection += differencePosition * backwardConnectorWeight;
		}
	}

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetReferenceDirection(const Object *pObject,	float backwardConnectorWeight,
		float forwardConnectorWeight, unsigned int depth, unsigned int numberOfForwardLayers, pandora::CartesianVector &meanBackwardDirection)
{
	meanBackwardDirection = pandora::CartesianVector(0.f, 0.f, 0.f);

	if(NULL == pObject)
	{
		std::cout << "ERROR : ArborHelper::GetReferenceDirection() : pObject is NULL" << std::endl;
		return STATUS_CODE_INVALID_PARAMETER;
	}

	if(depth == 0)
		return STATUS_CODE_INVALID_PARAMETER;

	if(numberOfForwardLayers == 0)
		return STATUS_CODE_INVALID_PARAMETER;

	const ConnectorList &backwardConnectorList = pObject->GetBackwardConnectorList();
	const ConnectorList &forwardConnectorList = pObject->GetForwardConnectorList();
	const CartesianVector &objectPosition = pObject->GetPosition();

	unsigned int startingDepth = depth - 1;
	unsigned int maxForwardLayer = pObject->GetPseudoLayer() + numberOfForwardLayers;

	for(ConnectorList::const_iterator iter = backwardConnectorList.begin() , endIter = backwardConnectorList.end() ; endIter != iter ; ++iter)
	{
		const Connector *pConnector = *iter;
		const Object *pOtherObject = NULL;

		if(pObject == pConnector->GetFirst())
		{
			pOtherObject = pConnector->GetSecond();
		}
		else
		{
			pOtherObject = pConnector->GetFirst();
		}

		CartesianVector differencePosition = pOtherObject->GetPosition() - objectPosition;
		meanBackwardDirection += differencePosition * backwardConnectorWeight;
	}

	CartesianVector meanForwardDirection(0.f, 0.f, 0.f);

	for(ConnectorList::const_iterator iter = forwardConnectorList.begin() , endIter = forwardConnectorList.end() ; endIter != iter ; ++iter)
	{
		const Connector *pConnector = *iter;
		const Object *pOtherObject = NULL;

		if(pObject == pConnector->GetFirst())
		{
			pOtherObject = pConnector->GetSecond();
		}
		else
		{
			pOtherObject = pConnector->GetFirst();
		}

		unsigned int otherPseudoLayer = pOtherObject->GetPseudoLayer();

		if(otherPseudoLayer > maxForwardLayer)
			continue;

		CartesianVector differencePosition = pOtherObject->GetPosition() - objectPosition;
		meanForwardDirection += differencePosition * forwardConnectorWeight;

		PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::RecursiveReferenceDirection(pOtherObject, forwardConnectorWeight,
				startingDepth, maxForwardLayer, meanForwardDirection));
	}

	if(meanForwardDirection == CartesianVector(0.f, 0.f, 0.f))
		return STATUS_CODE_SUCCESS;

	meanBackwardDirection += meanForwardDirection * -1.f;

	if(meanBackwardDirection == CartesianVector(0.f, 0.f, 0.f))
		return STATUS_CODE_FAILURE;

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::RecursiveReferenceDirection(const Object *pObject,	float forwardConnectorWeight,
		unsigned int &currentDepth, unsigned int maxForwardLayer, pandora::CartesianVector &meanForwardDirection)
{
	// stop iteration if the maximum depth is reached
	if(currentDepth == 0)
		return STATUS_CODE_SUCCESS;

	unsigned int currentRecursiveDepth = currentDepth - 1;

	if(NULL == pObject)
		return STATUS_CODE_INVALID_PARAMETER;

	if(pObject->GetForwardConnectorList().empty())
		return STATUS_CODE_SUCCESS;

	const ConnectorList &forwardConnectorList = pObject->GetForwardConnectorList();
	const CartesianVector &objectPosition = pObject->GetPosition();

	for(ConnectorList::const_iterator iter = forwardConnectorList.begin() , endIter = forwardConnectorList.end() ; endIter != iter ; ++iter)
	{
		const Connector *pConnector = *iter;
		const Object *pOtherObject = NULL;

		if(pObject == pConnector->GetFirst())
		{
			pOtherObject = pConnector->GetSecond();
		}
		else
		{
			pOtherObject = pConnector->GetFirst();
		}

		unsigned int otherPseudoLayer = pOtherObject->GetPseudoLayer();

		if(otherPseudoLayer > maxForwardLayer)
			continue;

		CartesianVector differencePosition = pOtherObject->GetPosition() - objectPosition;
		meanForwardDirection += differencePosition * forwardConnectorWeight;

		PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::RecursiveReferenceDirection(pOtherObject, forwardConnectorWeight,
				currentRecursiveDepth, maxForwardLayer, meanForwardDirection));
	}

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetKappaParameter(float distance, float angle,
		float distancePower, float anglePower, float &kappaParameter)
{
	kappaParameter = std::pow(angle, anglePower) * std::pow(distance, distancePower);

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::BuildOrderedObjectList(const ObjectList &objectList, OrderedObjectList &orderedObjectList)
{
	if(objectList.empty())
		return STATUS_CODE_INVALID_PARAMETER;

	for(ObjectList::const_iterator iter = objectList.begin() , endIter = objectList.end() ;
			endIter != iter ; ++iter)
	{
		PseudoLayer pseudoLayer = (*iter)->GetPseudoLayer();
		orderedObjectList[pseudoLayer].insert(*iter);
	}

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::GetMeanDirection(const Object *pObject, ConnectorDirection connectorDirection,
		pandora::CartesianVector &direction, unsigned int connectorDepth, unsigned int pseudoLayerDepth)
{
	if(NULL == pObject)
		return STATUS_CODE_INVALID_PARAMETER;

	ConnectorList connectorList;
	unsigned int maxPseudoLayer(0);

	if(connectorDirection == FORWARD)
	{
		connectorList = pObject->GetForwardConnectorList();
		maxPseudoLayer = pseudoLayerDepth == std::numeric_limits<unsigned int>::max() ? pseudoLayerDepth : pObject->GetPseudoLayer() + pseudoLayerDepth;
	}
	else
	{
		connectorList = pObject->GetBackwardConnectorList();
		maxPseudoLayer = pseudoLayerDepth >= pObject->GetPseudoLayer() ? 0 : pObject->GetPseudoLayer() - pseudoLayerDepth;
	}

	if(connectorList.empty())
		return STATUS_CODE_UNCHANGED;

	const pandora::CartesianVector objectPosition(pObject->GetPosition());

	for(ConnectorList::const_iterator iter = connectorList.begin() , endIter = connectorList.end() ;
			endIter != iter ; ++iter)
	{
		Connector *pConnector = *iter;
		Object *pOtherObject = NULL;

		if(pConnector->GetFirst() == pObject)
			pOtherObject = pConnector->GetSecond();
		else
			pOtherObject = pConnector->GetFirst();

		if(pOtherObject->GetPseudoLayer() > maxPseudoLayer)
			continue;

		const pandora::CartesianVector otherObjectPosition(pOtherObject->GetPosition());

		direction += (otherObjectPosition - objectPosition);

		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::RecursiveDirection(pOtherObject, connectorDirection, direction, connectorDepth-1, maxPseudoLayer));
	}

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborHelper::RecursiveDirection(const Object *pObject, ConnectorDirection connectorDirection,
		pandora::CartesianVector &direction, unsigned int currentDepth, unsigned int maxPseudoLayer)
{
	if(NULL == pObject)
		return STATUS_CODE_INVALID_PARAMETER;

	if(0 == currentDepth)
		return STATUS_CODE_SUCCESS;

	const pandora::PseudoLayer pseudoLayer(pObject->GetPseudoLayer());

	ConnectorList connectorList;

	if(connectorDirection == FORWARD)
	{
		connectorList = pObject->GetForwardConnectorList();
	}
	else
	{
		connectorList = pObject->GetBackwardConnectorList();
	}

	if(connectorList.empty())
		return STATUS_CODE_SUCCESS;

	const pandora::CartesianVector objectPosition(pObject->GetPosition());

	for(ConnectorList::const_iterator iter = connectorList.begin() , endIter = connectorList.end() ;
			endIter != iter ; ++iter)
	{
		Connector *pConnector = *iter;
		Object *pOtherObject = NULL;

		if(pConnector->GetFirst() == pObject)
			pOtherObject = pConnector->GetSecond();
		else
			pOtherObject = pConnector->GetFirst();

		if(pOtherObject->GetPseudoLayer() > maxPseudoLayer)
			continue;

		const pandora::CartesianVector otherObjectPosition(pOtherObject->GetPosition());

		direction += (otherObjectPosition - objectPosition);

		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ArborHelper::RecursiveDirection(pOtherObject, connectorDirection, direction, currentDepth-1, maxPseudoLayer));
	}

	return STATUS_CODE_SUCCESS;
}

} 

