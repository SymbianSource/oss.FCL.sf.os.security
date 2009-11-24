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
*
*/


#include "authserver/authtypes.h"

_LIT(KPluginName, "Test Plugin #1");
_LIT(KPluginDescription, "Returns known id and data for each method.");

_LIT8(KIdentifyData, "ABABABABABABABABABAB");
_LIT8(KTrainData, "BABABABABABABABABABA");
_LIT8(KRetrainData, "CCCCCCCCCCCCCCCCCCCC");

static const TInt KEntropy = 999;
static const TInt KFalsePos = 100;
static const TInt KFalseNeg = 10;
