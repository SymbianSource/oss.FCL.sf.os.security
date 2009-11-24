/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* CPluginDesc class definition
*
*/


/**
 @file 
*/

#include "authcommon_impl.h"

using namespace AuthServer;


// -------- factory functions --------


/**
	Factory function allocates a new instance of CPluginDesc
	from the supplied arguments.

	The supplied values are arbitrary and need not correlate with
	the actual plugins on the device.
	
	@param	aId				The plugin's ID.  This value is used
							to specify the plugin in an authentication
							expression.
							actually available on the system.
	@param	aName			The plugin's name.  The newly-created
							description object allocates its own copy
							of this name, so aName does not have to remain
							in scope after the object has been created.
	@param	aType			The plugin's type.
	@param	aTrainingStatus	The plugin's training status.
	@param	aMinEntropy		The plugin's minimum entropy as a percentage.
	@param	aFalsePositiveRate	The plugin's false positive rate as a
							percentage.
	@param	aFalseNegativeRate	The plugin's false negative rate as a
							percentage.
 */
EXPORT_C CPluginDesc* CPluginDesc::NewL(
	TPluginId aId, const TDesC& aName,
	TAuthPluginType aType, TAuthTrainingStatus aTrainingStatus,
	TEntropy aMinEntropy, TPercentage aFalsePositiveRate,
	TPercentage aFalseNegativeRate)

	{
	CPluginDesc* self = new(ELeave) CPluginDesc(
		aId, aType, aTrainingStatus, aMinEntropy,
		aFalsePositiveRate, aFalseNegativeRate);
	
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	
	return self;
	}

/**
	Record supplied information in this new object,
	See NewL for argument descriptions.
	
	@see NewL
 */
CPluginDesc::CPluginDesc(
	TPluginId aId, TAuthPluginType aType,
	TAuthTrainingStatus aTrainingStatus, TEntropy aMinEntropy,
	TPercentage aFalsePositiveRate, TPercentage aFalseNegativeRate)
:	iId(aId),
	iType(aType),
	iTrainingStatus(aTrainingStatus),
	iMinEntropy(aMinEntropy),
	iFalsePositiveRate(aFalsePositiveRate),
	iFalseNegativeRate(aFalseNegativeRate)
	{
	// empty.
	}


/**
	Allocate a copy of the supplied name.  The
	copy is freed in the destructor.
	
	@param	aName			This plugin's name.
 */
void CPluginDesc::ConstructL(const TDesC& aName)
	{
	iName = aName.AllocL();
	}


/**
	Free resources allocated in ConstructL.
	
	@see ConstructL
 */
CPluginDesc::~CPluginDesc()
    {
    delete iName;
    }


// -------- persistence --------

/**
	Recreate an instance of CPluginDesc from the supplied
	stream.  On success the object is placed on the cleanup stack.
	
	@param	aIn				Stream containing persisted plugin
							description.
	@return					New instance of CPluginDesc, created
							from information in the supplied stream.
 */
EXPORT_C CPluginDesc* CPluginDesc::NewLC(RReadStream& aIn)
	{
	CPluginDesc* self = new(ELeave) CPluginDesc;
	CleanupStack::PushL(self);
	self->InternalizeL(aIn);
	return self;
	}

/**
	Initialize this object from the supplied stream.
	This function is used to copy plugin descriptions
	across IPC boundaries.
	
	@param	aIn				Stream which contains externalized
							plugin descriptor.
	@see ExternalizeL
 */
void CPluginDesc::InternalizeL(RReadStream& aIn)
	{
	iId = aIn.ReadUint32L();
	iName = HBufC::NewL(aIn, KMaxNameLength);
	iType = static_cast<TAuthPluginType>(aIn.ReadUint32L());
	iTrainingStatus = static_cast<TAuthTrainingStatus>(aIn.ReadUint32L());
	iMinEntropy = aIn.ReadUint32L();
	iFalsePositiveRate = aIn.ReadUint32L();
	iFalseNegativeRate = aIn.ReadUint32L();
	}


/**
	Write information about this object to the supplied stream
	so it can be reconstructed later.
	
	This function is used to copy plugin descriptions
	across IPC boundaries.
	
	@see InternalizeL
 */
EXPORT_C void CPluginDesc::ExternalizeL(RWriteStream& aOut) const
	{
	aOut << iId;
	aOut << *iName;
	aOut.WriteInt32L(iType);
	aOut.WriteInt32L(iTrainingStatus);
	aOut << iMinEntropy;
	aOut << iFalsePositiveRate;
	aOut << iFalseNegativeRate;
	}


// -------- accessors --------

    
/**
 * The plugin id.
 * @return The plugin id
 **/
EXPORT_C TPluginId CPluginDesc::Id() const
  {
  return iId;
  }

/**
 * The plugin name.
 *
 * @return The plugin name, ownership of the returned pointer remains the
 * plugin description
 **/
EXPORT_C const TDesC* CPluginDesc::Name() const
  {
  return iName;
  }

/**
 * The plugin type.
 * @return the plugin type
 **/
EXPORT_C TAuthPluginType CPluginDesc::Type() const
  {
  return iType;
  }
	
/**
 * Indicates whether the plugin is trained for none, some or all known
 * identities.
 *
 * @return the training status
 **/
EXPORT_C TAuthTrainingStatus CPluginDesc::TrainingStatus() const
  {
  return iTrainingStatus;
  }

/**
 * How many unique identities the plugin supports.
 * @return The minumum entropy provided by the plugin.
 **/
EXPORT_C TEntropy CPluginDesc::MinEntropy() const
  {
  return iMinEntropy;
  }
	
/**
 * The percentage of times an device holder is falsely identified as an
 * known identity.
 *
 * @return The false positive rate of the plugin
 **/
EXPORT_C TPercentage CPluginDesc::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

/**
 * The percentage of times a known identity is not identified. 
 *
 * @return The false negative rate of the plugin.
 **/
EXPORT_C TPercentage CPluginDesc::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }


