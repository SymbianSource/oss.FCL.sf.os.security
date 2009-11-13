#ifndef __CONFIGWRITER_H__
#define __CONFIGWRITER_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <string>
#include <fstream>
#include <sstream>

class GoodConfigWriter
	{
public:
	GoodConfigWriter(const std::stringstream &aFileName);
	~GoodConfigWriter();
	std::ofstream iFile;
	};

class FileCertStoreConfigWriter : public GoodConfigWriter
	{
public:
	FileCertStoreConfigWriter(const std::stringstream &aFileName);
	~FileCertStoreConfigWriter();

	void WriteFileEntry(const char *aGoodLabel= "\"Root5CA\"",
					const char *aGoodDeletable = "\"true\"",
					const char *aGoodFormat= "\"EX509Certificate\"",
					const char *aGoodCertOwnerType = "\"ECACertificate\"",
					const char *aGoodSubjectKeyId ="auto",
					const char *aGoodIssuerKeyId = "auto",
					const char *aGoodApplication = "\"SW Install\"",
					const char *aGoodTrusted = "\"true\"",
					const char *aGoodDataFileName  = "\"cert0.der\"");
	void WriteExtraFileEntry();
	
private:
	int iEntryCount;
	};

class FileCertClientConfigWriter : public GoodConfigWriter
	{
public:
	FileCertClientConfigWriter(const std::stringstream &aFileName);
	~FileCertClientConfigWriter();

	void WriteCertClientName(const char *arrayVal);
	void WriteCertClientUid(const char *arrayVal);
	void WriteExtraCertClientEntry();
private:
	int iEntryCount;
	};


class SwiCertStoreConfigWriter : public GoodConfigWriter
	{
public:
	SwiCertStoreConfigWriter(const std::stringstream &aFileName);
	~SwiCertStoreConfigWriter();

	void WriteSwiEntry(const char *aGoodLabel= "\"certificate_Label\"",
					/*const char *aGoodDeletable = "\"true\"",*/
					const char *aGoodFormat= "EX509Certificate",
					const char *aGoodCertOwnerType = "\"ECACertificate\"",
					const char *aGoodSubjectKeyId ="auto",
					const char *aGoodIssuerKeyId = "auto",
					const char *aGoodApplication = "\"SW Install\"",
					const char *aGoodTrusted = "\"true\"",
					const char *aGoodCapabilitySet = "TCB",
					const char *aGoodMandatory = "\"true\"",
					const char *aGoodSystemUpgrade = "\"true\"");
	void WriteExtraSwiEntry();
private:
	int iEntryCount;
	};



// to generate script files for the file certstore to run tests on emulator

class FileStoreScriptGeneration : public GoodConfigWriter
	{
public:
	FileStoreScriptGeneration(const std::stringstream &aFileName);
	~FileStoreScriptGeneration();
	
	void WriteInitialiseCert(const char *aMode);
	void WriteListcert(const char *aGoodOwnerType);
	void WriteGetCertificateDetails(const char *label);
	void WriteGetTrust(const char *label, const char *trust);
	void WriteGetApplications(const char *label);
	void WriteRetrieveCerts(const char *label);
	void WriteDetailsToArray(const char *array[]);

private:
	int iCount;
	};


// to generate script files for the cert cleint store to run tests on emulator

class CertClientsStoreScriptGeneration : public GoodConfigWriter
	{
public:
	CertClientsStoreScriptGeneration(const std::stringstream &aFileName);
	~CertClientsStoreScriptGeneration();
	
	void WriteInitialiseCertClient();
	void WriteGetCount(const int index);
	void WriteGetApplicationsList();
	void WriteGetAppWithUid(const char *label, const char *uid);
	void WriteDestroyManager();
	
private:
	int iCount;
	};


// to generate script files for the swicertstore to run tests on emulator

class SWIStoreScriptGeneration : public GoodConfigWriter
	{
public:
	SWIStoreScriptGeneration(const std::stringstream &aFileName);
	~SWIStoreScriptGeneration();
	
	void WriteInitialiseCert();
	void WriteListcert(const char *aGoodOwnerType);
	void WriteGetSystemUpgrade(const char *label , const char *aSystemUpgrade);
	void WriteGetCapabilities(const char *label);
	void WriteGetMandatoryFlag(const char *label, const char *aMandatory);
	void WriteGetTrust(const char *label, const char *trust);
	void WriteGetApplications(const char *label);
	void WriteRetrieveCerts(const char *label);
	void WriteDetailsToArray(const char *array[]);
private:
	int iCount;
	};

class EmptyFileConfigWriter : public GoodConfigWriter
	{
public:
	EmptyFileConfigWriter(const std::stringstream &aFileName);
	~EmptyFileConfigWriter();
	};

#endif
