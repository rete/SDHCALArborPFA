  /// \file IBranchBuilder.h
/*
 *
 * IBranchBuilder.h header template automatically generated by a class generator
 * Creation date : jeu. mai 29 2014
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
 * @author Ete Remi
 * @copyright CNRS , IPNL
 */


#ifndef IBRANCHBUILDER_H
#define IBRANCHBUILDER_H

#include "Pandora/StatusCodes.h"
#include "Helpers/XmlHelper.h"

#include "arborpfa/arbor/ArborTypes.h"
#include "arborpfa/arbor/Arbor.h"
#include "arborpfa/content/Branch.h"

namespace arbor
{
class Tree;

/** 
 * @brief IBranchBuilder class
 */ 
class IBranchBuilder 
{
 protected:

 /**
  * @brief Dtor 
  */
 virtual ~IBranchBuilder() {}

 /**
  * @brief Build branches for the given tree. User should fill the list of branches
  */
 virtual pandora::StatusCode Build(Tree *pTree) = 0;

 /**
  *
  */
 pandora::StatusCode AddPrevious(Branch *pBranch, Object *pPreviousObject) const;

 /**
  *
  */
 pandora::StatusCode AddNext(Branch *pBranch, Object *pNextObject) const;

	/**
	 * @brief Read settings if needed
	 */
 virtual pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle)
 {
 	/* nop */
 	return pandora::STATUS_CODE_SUCCESS;
 }



 private:

	/**
	 *
	 */
	const Arbor *GetArbor() const;

	/**
	 *
	 */
	const ArborContentApiImpl *GetArborContentApiImpl() const;

	/**
		*
		*/
	pandora::StatusCode RegisterArbor(Arbor *pArbor);


	Arbor     *m_pArbor;          ///< The arbor instance to run this the tree builder

	friend class Arbor;
	friend class ArborContentApi;
	friend class ArborContentApiImpl;
	friend class ArborPluginManager;
};

//-----------------------------------------------------------------------------------------

inline pandora::StatusCode IBranchBuilder::AddPrevious(Branch *pBranch, Object *pPreviousObject) const
{
	return pBranch->AddPrevious(pPreviousObject);
}

//-----------------------------------------------------------------------------------------

inline pandora::StatusCode IBranchBuilder::AddNext(Branch *pBranch, Object *pNextObject) const
{
	return pBranch->AddNext(pNextObject);
}

//-----------------------------------------------------------------------------------------

inline const Arbor *IBranchBuilder::GetArbor() const
{
	if(NULL == m_pArbor)
		throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

	return m_pArbor;
}

//-----------------------------------------------------------------------------------------

inline const ArborContentApiImpl *IBranchBuilder::GetArborContentApiImpl() const
{
	if(NULL == m_pArbor)
		throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);

	return m_pArbor->GetArborContentApiImpl();
}

//-----------------------------------------------------------------------------------------

inline pandora::StatusCode IBranchBuilder::RegisterArbor(Arbor *pArbor)
{
	if(NULL == pArbor)
		return pandora::STATUS_CODE_FAILURE;

	m_pArbor = pArbor;

	return pandora::STATUS_CODE_SUCCESS;
}

} 

#endif  //  IBRANCHBUILDER_H