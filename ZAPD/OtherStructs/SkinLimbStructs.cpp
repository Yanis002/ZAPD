#include "SkinLimbStructs.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZDisplayList.h"
#include "ZFile.h"

/* SkinVertex */

SkinVertex::SkinVertex(ZFile* nParent) : ZResource(nParent)
{
}

void SkinVertex::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x04);
	unk_6 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x06);
	unk_7 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x07);
	unk_8 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x08);
	unk_9 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
}

std::string SkinVertex::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, %i, %i, %i, %i, %i, 0x%02X", unk_0, unk_2, unk_4, unk_6,
	                             unk_7, unk_8, unk_9);
}

std::string SkinVertex::GetSourceTypeName() const
{
	return "SkinVertex";
}

ZResourceType SkinVertex::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t SkinVertex::GetRawDataSize() const
{
	return 0x0A;
}

/* SkinTransformation */

SkinTransformation::SkinTransformation(ZFile* nParent) : ZResource(nParent)
{
}

void SkinTransformation::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x00);
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x02);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x04);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x06);
	unk_8 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
}

std::string SkinTransformation::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, %i, %i, %i, 0x%02X", unk_0, x, y, z, unk_8);
}

std::string SkinTransformation::GetSourceTypeName() const
{
	return "SkinTransformation";
}

ZResourceType SkinTransformation::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t SkinTransformation::GetRawDataSize() const
{
	return 0x0A;
}

/* SkinLimbModif */

SkinLimbModif::SkinLimbModif(ZFile* nParent) : ZResource(nParent)
{
}

void SkinLimbModif::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x04);
	unk_8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x08);
	unk_C = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x0C);

	if (unk_8 != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);

		unk_8_arr.reserve(unk_0);
		for (size_t i = 0; i < unk_0; i++)
		{
			SkinVertex unk8_data(parent);
			unk8_data.ExtractFromFile(unk_8_Offset);
			unk_8_arr.push_back(unk8_data);

			unk_8_Offset += unk8_data.GetRawDataSize();
		}
	}

	if (unk_C != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_C_Offset = Seg2Filespace(unk_C, parent->baseAddress);

		unk_C_arr.reserve(unk_2);
		for (size_t i = 0; i < unk_2; i++)
		{
			SkinTransformation unkC_data(parent);
			unkC_data.ExtractFromFile(unk_C_Offset);
			unk_C_arr.push_back(unkC_data);

			unk_C_Offset += unkC_data.GetRawDataSize();
		}
	}
}

void SkinLimbModif::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = prefix;
	if (name != "")
		varPrefix = name;

	if (unk_8 != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		const auto& res = unk_8_arr.at(0);
		std::string unk_8_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_8_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_8_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_8_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_8_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_8_Str, arrayItemCnt, entryStr);
		}
		else
			decl->declBody = entryStr;
	}

	if (unk_C != 0 && GETSEGNUM(unk_C) == parent->segment)
	{
		const auto& res = unk_C_arr.at(0);
		std::string unk_C_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_C_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_C_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_C_Offset = Seg2Filespace(unk_C, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_C_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_C_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_C_Str, arrayItemCnt, entryStr);
		}
		else
			decl->declBody = entryStr;
	}
}

std::string SkinLimbModif::GetBodySourceCode() const
{
	std::string unk_8_Str;
	std::string unk_C_Str;
	Globals::Instance->GetSegmentedPtrName(unk_8, parent, "SkinVertex", unk_8_Str);
	Globals::Instance->GetSegmentedPtrName(unk_C, parent, "SkinTransformation", unk_C_Str);

	std::string entryStr = StringHelper::Sprintf("\n\t\tARRAY_COUNTU(%s), ARRAY_COUNTU(%s),\n",
	                                             unk_8_Str.c_str(), unk_C_Str.c_str());
	entryStr +=
		StringHelper::Sprintf("\t\t%i, %s, %s\n\t", unk_4, unk_8_Str.c_str(), unk_C_Str.c_str());

	return entryStr;
}

std::string SkinLimbModif::GetSourceTypeName() const
{
	return "SkinLimbModif";
}

ZResourceType SkinLimbModif::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t SkinLimbModif::GetRawDataSize() const
{
	return 0x10;
}

/* SkinAnimatedLimbData */

SkinAnimatedLimbData::SkinAnimatedLimbData(ZFile* nParent) : ZResource(nParent)
{
}

void SkinAnimatedLimbData::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x04);
	unk_8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x08);

	if (unk_4 != 0 && GETSEGNUM(unk_4) == parent->segment)
	{
		uint32_t unk_4_Offset = Seg2Filespace(unk_4, parent->baseAddress);

		unk_4_arr.reserve(unk_2);
		for (size_t i = 0; i < unk_2; i++)
		{
			SkinLimbModif unk_4_data(parent);
			unk_4_data.ExtractFromFile(unk_4_Offset);
			unk_4_arr.push_back(unk_4_data);

			unk_4_Offset += unk_4_data.GetRawDataSize();
		}
	}
}

void SkinAnimatedLimbData::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = prefix;
	if (name != "")
		varPrefix = name;

	ZResource::DeclareReferences(varPrefix);

	if (unk_4 != SEGMENTED_NULL && GETSEGNUM(unk_4) == parent->segment)
	{
		const auto& res = unk_4_arr.at(0);
		std::string unk_4_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_4_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_4_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_4_Offset = Seg2Filespace(unk_4, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_4_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_4_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_4_Str, arrayItemCnt, entryStr);
		}
		else
			decl->declBody = entryStr;
	}

	if (unk_8 != SEGMENTED_NULL && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);

		int32_t dlistLength = ZDisplayList::GetDListLength(
			parent->GetRawData(), unk_8_Offset,
			Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
		ZDisplayList* unk_8_dlist = new ZDisplayList(parent);
		unk_8_dlist->ExtractFromBinary(unk_8_Offset, dlistLength);

		std::string dListStr =
			StringHelper::Sprintf("%sSkinLimbDL_%06X", varPrefix.c_str(), unk_8_Offset);
		unk_8_dlist->SetName(dListStr);
		unk_8_dlist->DeclareVar(varPrefix, "");
		unk_8_dlist->DeclareReferences(varPrefix);
		parent->AddResource(unk_8_dlist);
	}
}

std::string SkinAnimatedLimbData::GetBodySourceCode() const
{
	std::string unk_4_Str;
	std::string unk_8_Str;
	Globals::Instance->GetSegmentedPtrName(unk_4, parent, "SkinLimbModif", unk_4_Str);
	Globals::Instance->GetSegmentedPtrName(unk_8, parent, "Gfx", unk_8_Str);

	std::string entryStr = "\n";
	entryStr += StringHelper::Sprintf("\t%i, ARRAY_COUNTU(%s),\n", unk_0, unk_4_Str.c_str());
	entryStr += StringHelper::Sprintf("\t%s, %s\n", unk_4_Str.c_str(), unk_8_Str.c_str());

	return entryStr;
}

std::string SkinAnimatedLimbData::GetSourceTypeName() const
{
	return "SkinAnimatedLimbData";
}

ZResourceType SkinAnimatedLimbData::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t SkinAnimatedLimbData::GetRawDataSize() const
{
	return 0x0C;
}
