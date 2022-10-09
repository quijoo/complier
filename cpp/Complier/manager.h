#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>
#include <iomanip>

#include "element.h"
#include "tools.h"

using namespace std;

class SymbleManager
{
public:
	SymbleManager(const SymbleManager&) = delete;
	SymbleManager& operator=(const SymbleManager&) = delete;

public:
	static SymbleManager& getInstance()
	{
		static SymbleManager instance;
		return instance;
	}

private:
	SymbleManager() 
	{
		Insert(new Symble("e_", true));
		Insert(new Symble("$", true));
	}
	~SymbleManager() {}

private:
	map<string, Symble*> symble_map;

public:
	void Print()
	{
		cout << "------------------" << "输入符号" << "------------------" << endl;
		cout << setiosflags(ios::left) << setw(14) << "Name" << resetiosflags(ios::left) // 用完之后清除
			<< setiosflags(ios::right) << setw(9) << "ID" << setw(12) << "Terminal"
			<< resetiosflags(ios::right) << endl;
		for (auto pr : symble_map)
		{
			Symble* smb = pr.second;
			cout << setiosflags(ios::left) << setw(14) << smb->ToString() << resetiosflags(ios::left)
				<< setiosflags(ios::right) << setw(9) << (int)smb << setw(10) << smb->IsTerminal()
				<< resetiosflags(ios::right) << endl;
		}
		cout << "------------------" << "----" << "------------------" << endl;
	}

	Symble* Insert(Symble* smb)
	{
		pair<map<string, Symble*>::iterator, bool> succ = symble_map.insert(pair<string, Symble*>(smb->ToString(), smb));
		if (succ.second)
		{
			return smb;
		}
		return nullptr;
	}

	Symble* Find(const string& str)
	{
		map<string, Symble*> ::iterator it = symble_map.find(str);
		if (it != symble_map.end())
		{
			return it->second;
		}
		return nullptr;
	}

	bool Exit(Symble* smb)
	{
		Symble* ret = Find(smb->ToString());
		if (!ret)
			return false;
		return true;
	}

	vector<Symble*> GetAllSymble()
	{
		vector<Symble*> ret;
		for (auto pr : symble_map)
		{
			ret.push_back(pr.second);
		}
		return ret;
	}
};

class ProductionManager
{
public:
	ProductionManager(const ProductionManager&) = delete;
	ProductionManager& operator=(const ProductionManager&) = delete;
	static ProductionManager& getInstance()
	{
		static ProductionManager instance;
		return instance;
	}

private:
	ProductionManager() {}
	~ProductionManager() {}

private:
	map<string, Product*> product_map;
	map<Symble*, vector<Product*>*> smb_pdc_map;

public:
	Product* Insert(Product* pdc)
	{
		pair<map<string, Product*>::iterator, bool> succ = product_map.insert(pair<string, Product*>(pdc->ToString(), pdc));
		if (succ.second)
		{
			// 添加映射 map<int, vector<int>*>
			smb_pdc_map.insert(pair<Symble*, vector<Product*>*>(pdc->GetLeft(), new vector<Product*>()));
			vector<Product*>* pdc_list = smb_pdc_map.find(pdc->GetLeft())->second;
			pdc_list->push_back(pdc);
			return pdc;
		}
		return nullptr;
	}

	Product* Find(string str)
	{
		map<string, Product*> ::iterator it = product_map.find(str);
		if (it != product_map.end())
		{
			return it->second;
		}
		return nullptr;
	}

	vector<Product*>* GetProductByLeft(Symble* smb)
	{
		if (smb->IsTerminal())
			return new vector<Product*>();
		return smb_pdc_map.find(smb)->second;
	}

	vector<Product*> GetAllProduct()
	{
		vector<Product*> ret = vector<Product*>();
		for (auto pr : product_map)
		{
			ret.push_back(pr.second);
		}
		return ret;
	}
	void Print()
	{
		cout << "------------------" << "输入文法" << "------------------" << endl;
		cout << setiosflags(ios::left) << setw(20) << "ID" << resetiosflags(ios::left) // 用完之后清除
			<< setiosflags(ios::left) << setw(50) << "Expression" << setw(20) << "Hash Code"
			<< resetiosflags(ios::left) << endl;
		for (auto pr : product_map)
		{
			Product* pdc = pr.second;
			cout << setiosflags(ios::left) << setw(20) << pdc << resetiosflags(ios::left)
				<< setiosflags(ios::left) << setw(50) << pdc->ToString() << setw(20) << pdc->Hash()
				<< resetiosflags(ios::left) << endl;
		}
		cout << "------------------" << "----" << "------------------" << endl;
	}
};

void InitManager(string filename)
{
	ifstream inFile(filename, ios::in | ios::binary);
	if (!inFile) {
		cout << "error" << endl;
	}
	string line;
	Symble* smb;
	SymbleManager* smbMgr = &SymbleManager::getInstance();
	ProductionManager* pdcMgr = &ProductionManager::getInstance();
	Symble* left_id, *right_id;
	while (getline(inFile, line))
	{
		//1. 处理行末的换行符号\r
		line = strip(line, "\n");
		line = strip(line, "\r");
		//line = strip(line, "\n\r");

		if (line.size() == 0) continue;
		Product* product = new Product();
		//2. 拆分左部右部
		vector<string> symble_vector = split(line, " -> ");
		assert(symble_vector.size() == 2);
		
		//3.1 插入左部符号
		smb = smbMgr->Find(symble_vector[0]);
		if (!smb)
		{
			left_id = smbMgr->Insert(new Symble(symble_vector[0], false));
			assert(left_id);
			product->SetLeft(left_id);
		}
		else
		{
			smb->SetTerminal(false);
			product->SetLeft(smb);
		}
		
		//3.2 插入右部符号
		vector<string> right = split(symble_vector[1], " ");
		for (int i = 0; i < right.size(); i++)
		{
			Symble* new_smb = smbMgr->Find(right[i]);
			if (!new_smb)
			{
				new_smb = new Symble(right[i], true);
				smbMgr->Insert(new_smb);
				product->Insert(new_smb);
			}
			else
			{
				product->Insert(new_smb);
			}
		}
		pdcMgr->Insert(product);
	}
	inFile.close();
}