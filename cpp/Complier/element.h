#pragma once


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <assert.h>
#include <iomanip>
#include <set>

using namespace std;

class Symble
{
public:
	Symble(string _str, bool _terminal):str(_str), terminal(_terminal){}
	~Symble() {}
	bool IsTerminal() { return terminal; }
	void SetTerminal(bool _terminal) { terminal = _terminal; }
	bool operator==(Symble& smb) { return smb.ToString() == str; }
	bool operator!=(Symble& smb) { return !operator==(smb); }
	bool IsNull() { return str == "e_"; }
	bool IsStart() { return str == "Start_"; }
	bool IsEnd() { return str == "$"; }
	string ToString() { return str; }
	int ElementCount() { return 1; }
	bool Equal(Symble* smb) { return smb->ToString() == this->ToString() ? true : false; }

private:
	string str;
	bool terminal;
};

class Product
{
public:
	Product(Symble* left_, vector<Symble*> right_) :left_id(left_), right_id(right_) {}
	Product() {}
	~Product() {}
	string Hash()
	{
		string res = to_string((int)left_id);
		for (auto i : right_id)
		{
			res += to_string((int)i);
		}
		return res;
	}

	void SetLeft(Symble* smb)
	{
		left_id = smb;
	}
	void Insert(Symble* smb)
	{
		right_id.push_back(smb);
	}
	Symble* GetLeft() { return left_id; }
	vector<Symble*> GetRight() { return right_id; }
	vector<Symble*> Nonterminal()
	{
		vector<Symble*> res;
		for (int i = 0; i < right_id.size(); i++)
		{
			if (!right_id[i]->IsTerminal())
			{
				res.push_back(right_id[i]);
			}
		}
		return res;
	}

	int LastNonterminal()
	{
		int last_pos = 0;
		for (int i = 0; i < right_id.size(); i++)
		{
			if (!right_id[i]->IsTerminal())
			{
				last_pos = i;
			}
		}
		return last_pos;
	}
	vector<pair<Symble*, int>> SubSequence()
	{
		vector<pair<Symble*, int>> ret;
		for (int i = 0; i < right_id.size(); i++)
		{
			if (right_id[i]->IsTerminal())
				continue;
			ret.push_back(pair<Symble*, int>(right_id[i], i));
		}
		return ret;
	}

	string ToString()
	{
		string ret = left_id->ToString() + " -> ";
		for (Symble* pr : right_id)
		{
			ret += pr->ToString() + (pr == right_id.back()? "" :" ");
		}
		return ret;
	}
	int ElementCount() { return 1; }
	bool Equal(Product* pdc) { return pdc->ToString() == this->ToString() ? true : false; }
private:
	Symble* left_id;
	vector<Symble*> right_id;
};


#include "manager.h"

/// 所有的集合对象都需要实现
/// 1. static Create()
/// 2. GetID() && private:id
/// <summary>
/// 项：产生式 + dot + 向前看符号, 可以删掉这个类， 将GetDotRight对象换个地方实现。这里将所有的项都表示成三个int就好了tuple
/// </summary>
class Item
{
public:
	~Item() {};
public:
	Symble* GetDotRight()
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		vector <Symble*> right = product_id->GetRight();
		if (dot_pos >= right.size())
			return smb_ins->Find("e_");
		return right[dot_pos];
	}
	vector<Symble*> GetPosRight(int pos)
	{
		vector<Symble*> ret;
		vector<Symble*> right = product_id->GetRight();
		if (pos < 0 || pos > right.size()) return ret;
		for (auto pr : right)
		{
			if (pos > 0)
			{
				pos--;
				continue;
			}
			else
			{
				ret.push_back(pr);
			}
		}
		return ret;
	}

	static Item* Create(Product* _product_id, int _dot_pos, Symble* _look_forward_id)
	{
		return new Item(_product_id, _dot_pos, _look_forward_id);
	}

	string ToString() { return "(" + product_id->ToString() + ", " + to_string(dot_pos) + ", " + look_forward_id->ToString()  +  ")"; }
	
	int ElementCount() { return 1; }

	bool Equal(Item* item) { return this->ToString() == item->ToString() ? true : false; }

	int GetDotPos() { return dot_pos; }

	Symble* GetLookForward() { return look_forward_id; }

	Item* DotMoveClone(int offset) { return new Item(product_id, dot_pos + offset, look_forward_id); }

	Symble* GetLeft() { return product_id->GetLeft(); }

	Product* GetProduct() { return product_id; }

private:
	Item(Product* _product_id, int _dot_pos, Symble* _look_forward_id) :
		product_id(_product_id), dot_pos(_dot_pos), look_forward_id(_look_forward_id) {};
private:
	Product* product_id;
	int dot_pos;
	Symble* look_forward_id;
};