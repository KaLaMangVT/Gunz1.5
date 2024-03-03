#include "stdafx.h"
#include "Achievements.h"
#include "MMatchDBMgr.h"

Achievements::Achievements()
{

}

Achievements::~Achievements()
{

}

Achievements* Achievements::GetInstance()
{
	static Achievements inst;
	return &inst;
}

void Achievements::Destroy()
{
	for (int i = 0; i < achievements.size(); ++i)
	{
		delete achievements.at(i);
	}
	achievements.clear();
}


bool Achievements::readXml(const char* fileName, MZFileSystem* fileSystem)
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	char* buffer;
	MZFile mzf;

	if (fileSystem)
	{
		if (!mzf.Open(fileName, fileSystem))
		{
			if (!mzf.Open(fileName))
			{
				return false;
			}
		}
	}
	else
	{
		if (!mzf.Open(fileName))
		{
			return false;
		}
	}

	buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());

	if (!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		delete[] buffer;
		return false;
	}
	delete[] buffer;
	mzf.Close();

	MXmlElement rootElement, chrElement, attrElement, subElement;
	char szTagName[256];

	rootElement = xmlIniData.GetDocumentElement();
	int Count = rootElement.GetChildNodeCount();
	for (int i = 0; i < Count; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (_stricmp(szTagName, "ACHIEVEMENT") == 0)
		{
			achievementNode* node = new achievementNode;
			if (!parseXml_Achievement(chrElement, node))
			{
				mlog("Error reading achievement xml\n");
				delete node;
				return false;
			}
			achievements.push_back(std::move(node));
		}
	}
	return true;
}

bool Achievements::parseXml_Achievement(MXmlElement& elem, achievementNode* node)
{
	int id, type, requirement;
	string title, desc;
	elem.GetAttribute(&id, "id", 0);
	elem.GetAttribute(&type, "type", 0);
	elem.GetAttribute(&title, "title", "");
	elem.GetAttribute(&desc, "desc", "");
	elem.GetAttribute(&requirement, "requirement", 0);

	node->id = id;
	node->type = type;
	strcpy_s(node->title, title.c_str());
	strcpy_s(node->desc, desc.c_str());
	node->requirement = requirement;

	return true;
}


bool Achievements::checkRequirements(MMatchObject* targetPlayer, MMATCH_GAMETYPE type, int currVal, achievementNode*& outValue)
{
	vector<achievementNode*> nodes = Find(type);
	if (nodes.size() == 0)
		return false;
	bool isMatch = false;
	for (auto itor : nodes)
	{
		if (itor->requirement == currVal)
		{
			outValue = itor;
			outValue->id = itor->id;
			outValue->type = itor->type;
			isMatch = true;
			break;
		}
	}

	if (!isMatch)
		return false;

	if (MGetMatchServer()->GetDBMgr()->FindAchievements(targetPlayer->GetCharInfo()->m_nCID, outValue->type, outValue->id))
		return false;

	return true;
}

//todo: make unique_ptrs??
vector<achievementNode*> Achievements::Find(MMATCH_GAMETYPE achievementType)
{
	vector<achievementNode*> nodes;
	for (auto& itor  = achievements.begin(); itor != achievements.end(); ++itor)
	{
		achievementNode* node = *itor;
		if (node->type == achievementType)
			nodes.push_back(node);
	}
	return nodes;
}

achievementNode* Achievements::GetAchievement(int const& id)
{
	for (auto itor : achievements)
	{
		if (itor->id == id)
			return itor;
	}
	return nullptr;
}