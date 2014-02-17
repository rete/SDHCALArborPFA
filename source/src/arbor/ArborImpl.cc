/*
 *
 * ArborImpl.cc source template automatically generated by a class generator
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
#include "arborpfa/arbor/ArborImpl.h"
#include "arborpfa/arbor/Arbor.h"
#include "arborpfa/content/ArborObjectManager.h"
#include "arborpfa/content/ConnectorManager.h"

using namespace pandora;

namespace arborpfa
{

ArborImpl::ArborImpl(Arbor *pArbor) :
		m_pArbor(pArbor)
{

}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborImpl::PrepareConnectors() const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArbor->m_pArborObjectManager->CreateInitialLists());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArbor->m_pConnectorManager->CreateInitialLists());

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborImpl::ResetEvent() const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArbor->m_pArborObjectManager->ResetForNextEvent());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArbor->m_pConnectorManager->ResetForNextEvent());

	return STATUS_CODE_SUCCESS;
}





} 

