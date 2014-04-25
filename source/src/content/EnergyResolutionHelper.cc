  /// \file EnergyResolutionHelper.cc
/*
 *
 * EnergyResolutionHelper.cc source template automatically generated by a class generator
 * Creation date : mer. avr. 23 2014
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


#include "arborpfa/content/EnergyResolutionHelper.h"

using namespace pandora;

namespace arborpfa
{

EnergyResolutionFunctionMap EnergyResolutionHelper::m_energyResolutionFunctionMap;

//----------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyResolutionHelper::RegisterEnergyResolutionFunction(const std::string &energyResolutionFunctionName, EnergyResolutionFunction *pEnergyResolutionFunction)
{
	if(NULL == pEnergyResolutionFunction)
		return STATUS_CODE_INVALID_PARAMETER;

	EnergyResolutionFunctionMap::iterator findIter = m_energyResolutionFunctionMap.find(energyResolutionFunctionName);

	// if already present return an error
	if(m_energyResolutionFunctionMap.end() != findIter)
	{
		std::cout << "Energy resolution function '" << findIter->first << "' already registered" << std::endl;

		delete pEnergyResolutionFunction;
		return STATUS_CODE_ALREADY_PRESENT;
	}

	// else register it
	m_energyResolutionFunctionMap.insert(std::pair<std::string, EnergyResolutionFunction *>(energyResolutionFunctionName, pEnergyResolutionFunction));

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyResolutionHelper::GetEnergyResolution(const std::string &energyResolutionFunctionName, float energy, float &energyResolution)
{
	EnergyResolutionFunctionMap::iterator findIter = m_energyResolutionFunctionMap.find(energyResolutionFunctionName);

	if(m_energyResolutionFunctionMap.end() == findIter)
		return STATUS_CODE_NOT_FOUND;

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, findIter->second->GetEnergyResolution(energy, energyResolution));

	return STATUS_CODE_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode EnergyResolutionHelper::ReadSettings(const TiXmlHandle xmlHandle)
{

	for(EnergyResolutionFunctionMap::iterator iter = m_energyResolutionFunctionMap.begin() , endIter = m_energyResolutionFunctionMap.end() ; endIter != iter ; ++iter)
	{
		TiXmlElement *pFunctionElement(xmlHandle.FirstChild(iter->first).Element());

		if(NULL != pFunctionElement)
		{
			const TiXmlHandle functionHandle(pFunctionElement);
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->ReadSettings(functionHandle));
		}
	}

	return STATUS_CODE_SUCCESS;
}



} 
