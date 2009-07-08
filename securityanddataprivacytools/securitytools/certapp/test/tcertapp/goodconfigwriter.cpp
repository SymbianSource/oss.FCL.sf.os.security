/*
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


#include "goodconfigwriter.h"
#include <iostream>
#include <sstream>
#include "tcertapp_good.h"

// Array for the StatCA- holds all relevant details
const char *swicertemu_array[]=
	{
	"X509",
	"c7014670ad9c8ac296c2ae665c4e78f3d4df4a99"
	};

const char *cert_array[]=
	{
	"X509",
	"ca",
	"7ce306295116207214d425affd185b6d5e48af6f",
	"",
	"1"
	};

// uids required for cacerts
const char *emu_cacertsUid[]=
	{
	"268452523",
	"268478646"
	};


/**
Class required to create files to write data
*/
GoodConfigWriter::GoodConfigWriter(const std::stringstream &aFileName)
{
	std::string name = aFileName.str();
	iFile.open(name.c_str(), std::ios_base::trunc | std::ios_base::out);
	if(iFile.fail())
		{
		std::cout << "Failed to open '" << name.c_str()<< "' for output!" << std::endl;
		exit(-1);
		}
}

GoodConfigWriter::~GoodConfigWriter()
{
	iFile.close();
}

/**
Class definition for creating cacerts config files
*/
FileCertStoreConfigWriter::FileCertStoreConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iEntryCount(0)
{
	iFile << "StartCertStoreEntries" << std::endl;
}

FileCertStoreConfigWriter::~FileCertStoreConfigWriter()
{
	iFile << "EndCertStoreEntries" << std::endl;
}

void FileCertStoreConfigWriter::WriteExtraFileEntry()
{
	iFile << "\tStartEntry " << "\"AugmentData\"" << std::endl;
	iFile << "\t\tDeletable " << "\"true\"" << std::endl;
	iFile << "\t\tFormat " << "\"EX509Certificate\"" << std::endl;
	iFile << "\t\tCertOwnerType " << "\"ECACertificate\"" << std::endl;
	iFile << "\t\tSubjectKeyId " << "auto" << std::endl;
	iFile << "\t\tIssuerKeyId " << "auto" << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	iFile << "\t\t\t#Entry 1" << std::endl;
	iFile << "\t\t\t\tApplication " << "\"0x100042ab\"" << std::endl;
	iFile << "\t\t\t\tApplication " << "\"0x1000a8b6\"" << std::endl;
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << "\"true\"" << std::endl;
	iFile << "\t\tDataFileName " << "\"cert0.der\"" << std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}

void FileCertStoreConfigWriter::WriteFileEntry(const char *aGoodLabel,
										   const char *aGoodDeletable,
										   const char *aGoodFormat,
										   const char *aGoodCertOwnerType,
										   const char *aGoodSubjectKeyId,
										   const char *aGoodIssuerKeyId,
										   const char *aGoodApplication,
										   const char *aGoodTrusted,
										   const char *aGoodDataFileName)
{
	++iEntryCount;
	// Setup default values
	if(!aGoodDeletable) 
	{
	aGoodDeletable = "\"true\"";
	}
	if(!aGoodFormat) aGoodFormat = "\"EX509Certificate\"";
	if(!aGoodCertOwnerType) aGoodCertOwnerType = "\"ECACertificate\"";
	if(!aGoodSubjectKeyId) aGoodSubjectKeyId = "auto";
	if(!aGoodIssuerKeyId) aGoodIssuerKeyId = "auto";
	if(!aGoodApplication) aGoodApplication = "\"Server Authentication\"";
	if(!aGoodTrusted) aGoodTrusted = "\"true\"";
	
	iFile << "\t# Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aGoodLabel <<std::endl;
	iFile << "\t\tDeletable " << aGoodDeletable << std::endl;
	iFile << "\t\tFormat " << aGoodFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aGoodCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aGoodSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aGoodIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	// write down the applications
	for(int i = 0; i<2; i++)
		{
		iFile << "\t\t\t# Entry " << i << std::endl;
		iFile << "\t\t\t\tApplication " << goodApplications[i]<< std::endl;
		}
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aGoodTrusted << std::endl;
	iFile << "\t\tDataFileName " << aGoodDataFileName << std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}

/**
Class definition for generating certclient configuration files
*/
FileCertClientConfigWriter::FileCertClientConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName),iEntryCount(0)
{
	iFile << "StartClientInfo" << std::endl;
}

FileCertClientConfigWriter::~FileCertClientConfigWriter()
{
	iFile << "EndClientInfo" << std::endl;
}

void FileCertClientConfigWriter::WriteCertClientName(const char *aGoodAppName)
{
	iFile << "\t\tName "<< aGoodAppName << std::endl;
}

void FileCertClientConfigWriter::WriteCertClientUid(const char *aGoodUid)
{
	++iEntryCount;
	iFile << "\t#Entry "<< iEntryCount << std::endl;
	iFile << "\t\tUid "<< aGoodUid << std::endl;
}

void FileCertClientConfigWriter::WriteExtraCertClientEntry()
{
	iFile << "\t#Entry "<< iEntryCount << std::endl;
	iFile << "\t\tUid "<< "0x12345678" << std::endl;
	iFile << "\t\tName "<< "Augment_Label" << std::endl;
}

/**
Swi certstore config writer
*/
SwiCertStoreConfigWriter::SwiCertStoreConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iEntryCount(0)
{
	iFile <<"StartSwiCertStoreEntries" << std::endl;
}

SwiCertStoreConfigWriter::~SwiCertStoreConfigWriter()
{
	iFile << "EndSwiCertStoreEntries" << std::endl;
}

void SwiCertStoreConfigWriter::WriteExtraSwiEntry()
{
	iFile << "\tStartEntry " << "\"AugmentData\"" << std::endl;
	iFile << "\t\tFormat " << "\"EX509Certificate\"" << std::endl;
	iFile << "\t\tCertOwnerType " << "\"ECACertificate\"" << std::endl;
	iFile << "\t\tSubjectKeyId " << "auto" << std::endl;
	iFile << "\t\tIssuerKeyId " << "auto" << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	iFile << "\t\t\t# Entry 1" << std::endl;
	iFile << "\t\t\t\tApplication " << "0x1000aaaa" << std::endl;
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << "true" << std::endl;
	iFile << "\t\tDataFileName " << "\"swicertstore_cert0.der\"" << std::endl;
	iFile << "\t\tCapabilitySet " << "{ TCB LocalServices }";
	iFile << "\t\tMandatory " << "true" << std::endl;
	iFile << "\t\tSystemUpgrade " << "false"<< std::endl;
	iFile <<"\tEndEntry" <<std::endl;
}


void SwiCertStoreConfigWriter::WriteSwiEntry(const char *aGoodLabel,
										   const char *aGoodFormat,
										   const char *aGoodCertOwnerType,
										   const char *aGoodSubjectKeyId,
										   const char *aGoodIssuerKeyId,
										   const char *aGoodApplication,
										   const char *aGoodTrusted,
										   const char *aGoodCapabilitySets,
										   const char *aGoodMandatory,
										   const char *aGoodSystemUpgrade)
{
	++iEntryCount;
	// Setup default values
	if(!aGoodFormat) aGoodFormat = "\"EX509Certificate\"";
	if(!aGoodCertOwnerType) aGoodCertOwnerType = "ECACertificate";
	if(!aGoodSubjectKeyId) aGoodSubjectKeyId = "auto";
	if(!aGoodIssuerKeyId) aGoodIssuerKeyId = "auto";
	if(!aGoodApplication) aGoodApplication = "\"Server Authentication\"";
	if(!aGoodTrusted) aGoodTrusted = "\"true\"";
	if(!aGoodCapabilitySets) aGoodCapabilitySets = "TCB";
	if(!aGoodMandatory) aGoodMandatory = "\"true\"";
	if(!aGoodSystemUpgrade) aGoodSystemUpgrade = "\"true\"";

	iFile << "\t#Entry " << iEntryCount << std::endl;
	iFile << "\tStartEntry " << aGoodLabel << std::endl;
	iFile << "\t\tFormat " << aGoodFormat << std::endl;
	iFile << "\t\tCertOwnerType " << aGoodCertOwnerType << std::endl;
	iFile << "\t\tSubjectKeyId " << aGoodSubjectKeyId << std::endl;
	iFile << "\t\tIssuerKeyId " << aGoodIssuerKeyId << std::endl;
	iFile << "\t\tStartApplicationList" << std::endl;
	//write application
	for(int k = 0; k<2 ; k++)
		{
		iFile << "\t\t\t# Entry " << k << std::endl;
		iFile << "\t\t\t\tApplication " << goodUids[k] << std::endl;
		}
	iFile << "\t\tEndApplicationList" << std::endl;
	iFile << "\t\tTrusted " << aGoodTrusted << std::endl;
	iFile << "\t\tDataFileName " << "\"swicertstore_cert0.der\"" << std::endl;
	iFile << "\t\tCapabilitySet " << "{ ";
	for(int i = 0; i< 20; i++)
		{
		iFile << goodCapabilitySets[i];
		iFile <<" ";
		}
	iFile <<"}" <<std::endl;
	iFile << "\t\tMandatory " << aGoodMandatory << std::endl;
	iFile << "\t\tSystemUpgrade " << aGoodSystemUpgrade << std::endl;
	iFile << "\tEndEntry" << std::endl;
}


FileStoreScriptGeneration::FileStoreScriptGeneration(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iCount(1)
{
	
}

FileStoreScriptGeneration::~FileStoreScriptGeneration()
{
}

void FileStoreScriptGeneration:: WriteInitialiseCert(const char *aMode)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Initializing a CUnifiedCertStore" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "init" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<mode>" <<aMode <<"</mode>"<< std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" << "KErrNone" << "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile << std::endl;
}



void  FileStoreScriptGeneration::WriteListcert(const char *aGoodOwnerType)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the list of certificates" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "listcert" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<filter>" << std::endl;
	iFile << "\t\t\t<ownertype>" << aGoodOwnerType	<< "</ownertype>" << std::endl;
	iFile << "\t\t<filter>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	for(int z =0; z<6; z++)
		{
		iFile << "\t\t<CCTCertInfo><label>" <<goodEmuCert_array[z] << "</label></CCTCertInfo>" <<std::endl;
		}
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}

void  FileStoreScriptGeneration::WriteGetCertificateDetails(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the list of certificates" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "listcert" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<filter>" << std::endl;
	iFile << "\t\t\t<label>" << label <<"</label>" <<std::endl;
	WriteDetailsToArray(cert_array);
	iFile << "\t\t<CCTCertInfo><label>" << label << "</label></CCTCertInfo>" <<std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;

}


void FileStoreScriptGeneration::WriteDetailsToArray(const char *array[])
	{
	int i = 0;
	iFile << "\t\t\t<format>" << array[i++]	<< "</format>" << std::endl;
	iFile << "\t\t\t<certowner>" << array[i++] << "</certowner>" << std::endl;
	iFile << "\t\t\t<subjectkeyid>" << array[i++]<< "</subjectkeyid>" << std::endl;
	iFile << "\t\t\t<issuerkeyid>" <<array[i++]<< "</issuerkeyid>" << std::endl;
	iFile << "\t\t\t<deletable>" << array[i++] << "</deletable>" << std::endl;
	iFile << "\t\t</filter>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	}


void  FileStoreScriptGeneration::WriteGetTrust(const char *label, const char *trust)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get Trust certificate" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "gettrusters" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t\t\t<trust>" << trust << "</trust>" <<std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void  FileStoreScriptGeneration::WriteGetApplications(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get applications" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "getapplications" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t\t<label>" << label << "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t\t\t<uid>";

	for(int j = 0; j<2; j++)
		{
		iFile << emu_cacertsUid[j]<<" ";
		}
	iFile <<"</uid>"<<std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void  FileStoreScriptGeneration::WriteRetrieveCerts(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Retrieve Certificate" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "retrieve" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}



//Swi store script generator for emulator tests
SWIStoreScriptGeneration::SWIStoreScriptGeneration(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iCount(1)
{
	
}

SWIStoreScriptGeneration::~SWIStoreScriptGeneration()
{
}

void SWIStoreScriptGeneration:: WriteInitialiseCert()
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Initialise a SWICertStore" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "initswicertstore" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" << "KErrNone" << "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile << std::endl;
}


void  SWIStoreScriptGeneration::WriteListcert(const char *aGoodOwnerType)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the list of certificates" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "listcert" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<filter>" << std::endl;
	iFile << "\t\t\t<ownertype>" << aGoodOwnerType	<< "</ownertype>" << std::endl;
	iFile << "\t\t<filter>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	for(int i =0; i<6; i++)
		{
		iFile << "\t\t<CCTCertInfo><label>" << goodSwiCert_array[i] << "</label><readonly>True</readonly></CCTCertInfo>" <<std::endl;
		}
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}

void SWIStoreScriptGeneration::WriteDetailsToArray(const char *array[])
	{
	int i = 0;
	iFile << "\t\t\t<format>" << array[i++]	<< "</format>" << std::endl;
	iFile << "\t\t\t<subjectkeyid>" << array[i++]<< "</subjectkeyid>" << std::endl;
	iFile << "\t\t</filter>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	}

void  SWIStoreScriptGeneration::WriteGetSystemUpgrade(const char *label, const char *aSystemUpgrade)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the systemupgrade flag" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "getsystemupgrade" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t\t<systemupgrade>" << aSystemUpgrade <<"</systemupgrade>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void  SWIStoreScriptGeneration::WriteRetrieveCerts(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Retrieve Certificate" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "retrieve" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}



void  SWIStoreScriptGeneration::WriteGetApplications(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get applications" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "getapplications" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label << "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	
	iFile << "\t\t\t<uid>";
	for(int j = 0; j<2; j++)
		{
		iFile << emu_cacertsUid[j] << " ";
		}
	iFile <<"</uid>"<<std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void  SWIStoreScriptGeneration::WriteGetTrust(const char *label, const char *trust)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get Trust certificate" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "gettrusters" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t\t\t<trust>" << trust << "</trust>" <<std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}



void  SWIStoreScriptGeneration::WriteGetCapabilities(const char *label)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the capabilities" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "getcapabilities" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;

	for(int i=0; i<20; i++)
		{
		iFile << "\t\t<capability>" << goodCapabilitySets[i] <<"</capability>" << std::endl;
		}
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void  SWIStoreScriptGeneration::WriteGetMandatoryFlag(const char *label, const char *aMandatory)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get the mandatory flag" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "getmandatory" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<label>" << label<< "</label>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t\t<mandatory>" << aMandatory <<"</mandatory>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


//Cert client script generator
CertClientsStoreScriptGeneration::CertClientsStoreScriptGeneration(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName), iCount(1)
{
}

CertClientsStoreScriptGeneration::~CertClientsStoreScriptGeneration()
{
}

void CertClientsStoreScriptGeneration::WriteInitialiseCertClient()
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Initialise a CertClientStore" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "InitManager" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" << "KErrNone" << "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile << std::endl;
}

void CertClientsStoreScriptGeneration::WriteGetCount(const int aApp_uidIndex)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get Count of Applications" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "AppCount" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<count>" << aApp_uidIndex << "</count>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void CertClientsStoreScriptGeneration::WriteGetApplicationsList()
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Getting the application list" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "GetApplications" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	for(int i = 0 ; i<4 ; i++)
		{
		iFile << "\t\t<uid>" << gooddecimalUid_array[i] << "</uid>"<<"<appname>" << goodcertclient_array[i] << "</appname>" << std::endl;
		}
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void CertClientsStoreScriptGeneration::WriteGetAppWithUid(const char *goodlabel,const char *uid)
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Get application with given id" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "GetApp" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t\t<uid>" << uid << "</uid>"<< std::endl;
	iFile << "\t\t<appname>" << goodlabel << "</appname>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" <<"KErrNone"<< "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile <<std::endl;
}


void CertClientsStoreScriptGeneration::WriteDestroyManager()
{
	iFile << "# TEST" << iCount++ << std::endl;
	iFile <<"<action>"<<std::endl;
	iFile << "\t<actionname>" << "Destroy the manager" <<"</actionname>"<< std::endl;
	iFile << "\t<actiontype>" << "DestroyManager" <<"</actiontype>"<< std::endl;
	iFile << "\t<actionbody>" << std::endl;
	iFile << "\t</actionbody>" << std::endl;
	iFile << "\t<actionresult>" << std::endl;
	iFile << "\t\t<return>" << "KErrNone" << "</return>" << std::endl;
	iFile << "\t</actionresult>" << std::endl;
	iFile <<"</action>"<<std::endl;
	iFile << std::endl;
}


/**
Class definition for creating cacerts
*/
EmptyFileConfigWriter::EmptyFileConfigWriter(const std::stringstream &aFileName)
	: GoodConfigWriter(aFileName)
{
	iFile << "StartCertStoreEntries" << std::endl;
}

EmptyFileConfigWriter::~EmptyFileConfigWriter()
{
	iFile << "EndCertStoreEntries" << std::endl;
}

// End of file
