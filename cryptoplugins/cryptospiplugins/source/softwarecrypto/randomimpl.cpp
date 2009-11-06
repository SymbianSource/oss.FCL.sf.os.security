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


/**
 @file
*/

#include <e32std.h>
#include <e32debug.h>

#include "randomimpl.h"
#include "pluginentry.h"
#include "pluginconfig.h"

#include "randsvr.h"
#include "randcliserv.h"
#include "randsvrimpl.h"

_LIT(KRandomServerImg,"z:\\sys\\bin\\randsvr.exe");		// DLL/EXE name
_LIT(KRandomServerConnect, "Randsvr connect");
_LIT(KRandomServerGet, "Randsvr get");

const TUid KServerUid3={0x100066dc};


using namespace SoftwareCrypto;


CRandomImpl* CRandomImpl::NewL(void)
	{
	CRandomImpl* self = new(ELeave)CRandomImpl();
	return self;
	}

CRandomImpl* CRandomImpl::NewLC(void)
	{
	CRandomImpl* self = NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CRandomImpl::GenerateRandomBytesL(TDes8& aDest)
	{
	TRandomImpl::Random(aDest);
	}

CRandomImpl::CRandomImpl(void)
	{
	}

void TRandomImpl::Random(TDes8& aDestination)
	{
	RRandomSessionImpl rs;
	TRAPD(ret,rs.ConnectL());
	if (ret != KErrNone)
		{
		User::Panic(KRandomServerConnect, ret);
		}
	TInt err=rs.GetRandom(aDestination);
	if (err != KErrNone)
		{
		User::Panic(KRandomServerGet, err);
		}
	rs.Close();
	}

void CRandomImpl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	TInt randomNum = sizeof(KRandomCharacteristics)/sizeof(TRandomCharacteristics*);
	for (TInt i = 0; i < randomNum; i++)
		{
		if (KRandomCharacteristics[i]->cmn.iImplementationUID == ImplementationUid().iUid)
			{
			aPluginCharacteristics = KRandomCharacteristics[i];
			break;
			}
		}
	}

CExtendedCharacteristics* CRandomImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CRandomImpl::GetExtendedCharacteristicsL()
	{
	return CRandomImpl::CreateExtendedCharacteristicsL();
	}

TUid CRandomImpl::ImplementationUid() const
	{
	return KCryptoPluginRandomUid;
	}

CRandomImpl::~CRandomImpl()
	{
	}

void CRandomImpl::Close()
	{
	delete this;
	}

// All crypto plugins must implement this, to reset
// hardware if required. Do nothing in this version
void CRandomImpl::Reset()
	{
	}

RRandomSessionImpl::RRandomSessionImpl(void)
	{
	}

static TInt StartServer()
// Borrowed from AndrewT's server startup code.
// Start the server process/thread which lives in an EPOCEXE object
//
	{
	
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KRandomServerImg, KNullDesC, serverUid);

	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;

	}

void RRandomSessionImpl::ConnectL(void)
	{
	TInt retry=2;
	for (;;)
		{
		// Magic number 1 below is the number of asynchronous message slots
		TInt r = CreateSession(KRandomServerName,TVersion(0,0,0), 1);
		if (r == KErrNone) return;
		// We used to leave with KErrNone, but this is inefficient and
		// provokes an emulator problem in User::Leave which causes tpbe to crash
		// if (r == KErrNone) User::Leave(r);   // Connected okay
		if (r != KErrNotFound && r != KErrServerTerminated)
			   User::Leave(r);   // Something else happened
		if (--retry == 0)
			User::Leave(r);      // Give up after a while
		r = StartServer();       // Try starting again
		if (r != KErrNone && r != KErrAlreadyExists)
			User::Leave(r);
		}
	}

TInt RRandomSessionImpl::GetRandom(TDes8& aDestination)
	{
	TInt desclength = aDestination.Length();
	for ( TInt i = 0; i < desclength; i += KRandomBlockSize)
		{
		TInt getlen = Min(KRandomBlockSize, desclength - i);
		TPtr8 buffer(&aDestination[i], KRandomBlockSize, KRandomBlockSize);
		TInt err = SendReceive(CRandomSession::KRandomRequest, TIpcArgs(&buffer, getlen));
		if (err != KErrNone)
			{
			return err;
			}
		}
	return KErrNone;
	}

// Methods which are not supported can be excluded from the coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

TAny* CRandomImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;
	}
