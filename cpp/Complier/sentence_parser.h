#pragma once
#include "element.h"
#include "element_set.h"
#include <stack>
#include <map>
#include <time.h>

typedef Set<Symble*>     SymbleSet;
typedef Set<Item*>       ItemSet;
typedef Set<Set<Item*>*> ItemCluser;

#define ACTION_ACC 0
#define ACTION_MERGE 1
#define ACTION_MOVE 2

#define FINISH      11
#define NEXTSYMBLE  12
#define CONTINUE    13

class Action
{
public:
	int type;
	int next_state = -1;
	Product* pdc = nullptr;
public:
	Action(int type, Product* pdc, int state) : type(type), pdc(pdc), next_state(state){};
	~Action() {};
	string ToString() { return "[" + to_string(type) + ", " + to_string(next_state) + ", " + (!pdc ? "null" : pdc->ToString()) + "]"; }
	bool Equal(Action* other) { return type == other->type && next_state == other->next_state && pdc == other->pdc; }
};

class AnalyseTable
{
private:
	map<Symble*, SymbleSet*> first_map;
	map<Symble*, SymbleSet*> fellow_map;

	map<int, map<Symble*, int>*> GOTO;
	map<int, map<Symble*, Action*>*> ACTION;

	ItemCluser* C;

	int start_state;
private:
	stack<int>* stk;

public:
	AnalyseTable()
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		for (Symble* smb : smb_ins->GetAllSymble())
		{
			if (!smb->IsTerminal())
			{
				fellow_map.insert(pair<Symble*, SymbleSet*>(smb, SymbleSet::Create()));
			}
			first_map.insert(pair<Symble*, SymbleSet*>(smb, SymbleSet::Create()));
		}
	}
	~AnalyseTable() {}

	SymbleSet* FindFirst(Symble* smb)
	{
		map<Symble*, SymbleSet*> ::iterator it = first_map.find(smb);
		if (it != first_map.end())
		{
			return it->second;
		}
		return nullptr;
	}

	SymbleSet* FindFellow(Symble* smb)
	{
		map<Symble*, SymbleSet*> ::iterator it = fellow_map.find(smb);
		if (it != fellow_map.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void Print()
	{
		cout << "First = {" << endl;

		for (auto p : first_map)
		{
			cout << setiosflags(ios::left) << setw(10) << p.first->ToString() << resetiosflags(ios::left)
				<< setiosflags(ios::left) << setw(8) << "->" << setw(50) << p.second->ToString()
				<< resetiosflags(ios::left) << endl;
		}
		cout << "}" << endl << endl;

		cout << "Fellow = {" << endl;

		for (auto p : fellow_map)
		{
			cout << setiosflags(ios::left) << setw(10) << p.first->ToString() << resetiosflags(ios::left)
				<< setiosflags(ios::left) << setw(8) << "->" << setw(50) << p.second->ToString()
				<< resetiosflags(ios::left) << endl;
		}
		cout << "}" << endl;

		//cout << "Cluser = {" <<  C->ToString() << "}" << endl;

		cout << "GOTO = {" << endl;

		for (auto p : GOTO)
		{
			cout << "\tState " << p.first << endl;
			for (auto pp : *p.second)
			{
				cout << "\t\tcase: " << pp.first->ToString() << " to " << pp.second << endl;
			}
		}
		cout << "}" << endl;

		cout << "ACTION = {" << endl;
		for (auto p : ACTION)
		{
			cout << "\tState " << p.first << endl;
			for (auto pp : *p.second)
			{
				cout << "\t\tcase: " << pp.first->ToString() << " to " << pp.second->ToString() << endl;
			}
		}
		cout << "}" << endl;

	}
	int FirstCount()
	{
		int s = 0;
		for (auto p : first_map)
		{
			s += p.second->ElementCount();
		}
		return s;
	}

	int FellowCount()
	{
		int s = 0;
		for (auto p : fellow_map)
		{
			s += p.second->ElementCount();
		}
		return s;
	}
	/// <summary>
	/// 更改First集合函数
	/// </summary>
	/// <param name="smb"></param>
	/// <param name="insert"></param>
	void InsertFirst(Symble* smb, Symble* insert)
	{
		first_map.find(smb)->second->Insert(insert);
	}

	void InsertFellow(Symble* smb, Symble* insert)
	{
		fellow_map.find(smb)->second->Insert(insert);
	}
	void InsertTable(int i, Symble* smb, Action* act)
	{
		map<int, map<Symble*, Action*>*> ::iterator it = ACTION.find(i);
		if (it == ACTION.end())
		{
			ACTION[i] = new map<Symble*, Action*>();
		}
		auto ret = (ACTION[i])->insert(pair<Symble*, Action*>(smb, act));
		if (!ret.second && !(ret.first->second->Equal(act)))
		{
			cout << "fatal error: the input grammar is not belong to LR(1). when "  << i << " " << ret.first->second->ToString() << " " << act->ToString() << endl;
		}
	}
	void InsertTable(int i, Symble* smb, int j)
	{
		map<int, map<Symble*, int>*> ::iterator it = GOTO.find(i);
		if (it == GOTO.end())
		{
			GOTO[i] = new map<Symble*, int>();
		}
		(GOTO[i])->insert(pair<Symble*, int>(smb, j));
	}
	int FindState(int i, Symble* smb)
	{
		map<int, map<Symble*, int>*> ::iterator it = GOTO.find(i);
		if (it != GOTO.end())
		{
			 map<Symble*, int>::iterator it_1 = it->second->find(smb);
			 if (it_1 != it->second->end())
			 {
				 return it_1->second;
			 }
		}
		return -1;
	}

	Action* FindAction(int i, Symble* smb)
	{
		map<int, map<Symble*, Action*>*> ::iterator it = ACTION.find(i);
		if (it != ACTION.end())
		{
			map<Symble*, Action*>::iterator it_1 = it->second->find(smb);
			if (it_1 != it->second->end())
			{
				return it_1->second;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// 计算First集合的核心函数
	/// </summary>
	/// <param name="pdc"></param>
	void UpdateFirst(Product* pdc)
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		ProductionManager* pdc_ins = &ProductionManager::getInstance();
		Symble* smb_null = smb_ins->Find("e_");

		Symble* left = pdc->GetLeft();
		vector<Symble*> right = pdc->GetRight();

		assert(right.size() >= 1);
		first_map[left]->Union(first_map[right[0]]);
		for (int i = 0; i < right.size(); i++)
		{
			if (!first_map[right[i]]->Contain(smb_null))
			{
				break;
			}
			if (i == right.size() - 1)
			{
				first_map[left]->Insert(smb_null);
				break;
			}
			first_map[left]->Union(first_map[right[i + 1]]);
		}
	}
	/// <summary>
	/// 计算基础符号的First集合
	/// </summary>
	void BuildFirst()
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		ProductionManager* pdc_ins = &ProductionManager::getInstance();
		Symble* smb_null = smb_ins->Find("e_");

		int prev_size = -1;
		while (prev_size != FirstCount())
		{
			// record map size.
			prev_size = FirstCount();
			for (Symble* smb : smb_ins->GetAllSymble())
			{
				if (smb->IsTerminal()) InsertFirst(smb, smb);
				else
				{
					for (Product* pdc : *pdc_ins->GetProductByLeft(smb))
					{
						UpdateFirst(pdc);
					}
				}
				if (pdc_ins->Find(smb->ToString() + " -> e_"))
				{
					InsertFirst(smb, smb_null);
				}

			}
		}
	}

	/// <summary>
	/// 计算序列的First集合
	/// </summary>
	/// <param name="smb_list"></param>
	/// <param name="start"></param>
	/// <returns></returns>
	SymbleSet* SeqFirst(vector<Symble*> smb_list, int start)
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		ProductionManager* pdc_ins = &ProductionManager::getInstance();

		Symble* smb_null = smb_ins->Find("e_");
		SymbleSet* smb_set_null = SymbleSet::Create();
		smb_set_null->Insert(smb_null);

		// 特判当输入序列为空的时候返回仅包含e_的集合
		if (smb_list.size() <= start)
		{
			return smb_set_null;
		}

		SymbleSet* smb_set = SymbleSet::Create();

		smb_set->Union(first_map[smb_list[start]]);
		smb_set->Difference(smb_set_null);

		for (int i = start; i < smb_list.size(); i++)
		{
			if (!first_map[smb_list[i]]->Contain(smb_null))
				break;
			if (i == smb_list.size() - 1)
			{
				smb_set->Insert(smb_null);
				break;
			}
			smb_set->Union(first_map[smb_list[i + 1]]);
			smb_set->Difference(smb_set_null);
		}
		/*for (Symble* smb : smb_list)
		{
			cout << smb->ToString() << "  ";
		}
		cout << smb_set->ToString() << endl;*/
		return smb_set;
	}

	/// <summary>
	///	计算Fellow集合
	/// </summary>
	void BuildFellow()
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		ProductionManager* pdc_ins = &ProductionManager::getInstance();
		SymbleSet* smb_set_null = SymbleSet::Create();
		Symble* smb_null = smb_ins->Find("e_");
		smb_set_null->Insert(smb_null);

		smb_ins->Insert(new Symble("$", true));

		int prev_size = -1;
		fellow_map[smb_ins->Find("Start")]->Insert(smb_ins->Find("$"));

		while (prev_size != FellowCount())
		{
			// record map size.
			prev_size = FellowCount();
			for (auto pdc : pdc_ins->GetAllProduct())
			{
				for (auto pr : pdc->SubSequence())
				{
					SymbleSet* firstB = SeqFirst(pdc->GetRight(), pr.second + 1);
					fellow_map[pr.first]->Union(firstB);
					fellow_map[pr.first]->Difference(smb_set_null);
					// 由于SeqFirst特判了终结符后为空的情况， 这里就不处理B后边没有符号的情况了
					if (firstB->Contain(smb_null))
					{
						fellow_map[pr.first]->Union(fellow_map[pdc->GetLeft()]);
					}

				}
			}

		}
	}

	/// <summary>
	/// 用于计算闭包
	/// </summary>
	/// <param name="I"></param>
	/// <returns></returns>
	ItemSet* Closure(ItemSet* I)
	{
		SymbleManager* smb_ins = &SymbleManager::getInstance();
		ProductionManager* pdc_ins = &ProductionManager::getInstance();
		vector<Item*> wait_add;
		vector<Item*> next_process;
		for (auto item : *I->Traverse())
		{
			next_process.push_back(item);
		}
		while (true)
		{
			wait_add.clear();
			for (auto item : next_process)
			{
				// 记当前项为 A->alpha . B beta, a
				// 获取 B
				Symble* B = item->GetDotRight();
				if (B->IsNull() || B->IsTerminal()) continue;
				vector<Symble*> beta = item->GetPosRight(item->GetDotPos() + 1);
				beta.push_back(item->GetLookForward());
				SymbleSet* sset = SeqFirst(beta, 0);
				for (auto B_pdc : *(pdc_ins->GetProductByLeft(B)))
				{
					// 先计算FIRST && FELLOW
					for (Symble* b : *sset->Traverse())
					{
						if (!b->IsTerminal()) continue;
						Item* it = Item::Create(B_pdc, 0, b);
						if (!I->Contain(it))
							wait_add.push_back(it);
					}
				}
			}

			if (wait_add.size() == 0)
				break;

			next_process.clear();
			for (Item* pr : wait_add)
			{
				if (I->Insert(pr))
				{
					next_process.push_back(pr);
				}
			}
		}
		return I;

	}

	/// <summary>
	/// Function Goto， 用于计算Goto表
	/// </summary>
	/// <param name="I"></param>
	/// <param name="X"></param>
	/// <returns></returns>
	ItemSet* Goto(ItemSet* I, Symble* X)
	{
		ItemSet* J = ItemSet::Create();
		for (auto item : *I->Traverse())
		{
			Symble* smb = item->GetDotRight();
			if (smb->IsNull() || !smb->Equal(X)) 
				continue;
			J->Insert(item->DotMoveClone(1));
		}
		return Closure(J);
	}

	void items(ProductionManager* pdc_ins, SymbleManager* smb_ind)
	{
		C = ItemCluser::Create();
		ItemSet* start_itemset = ItemSet::Create();
		start_state = start_itemset->GetID();
		Item* start_item = Item::Create(pdc_ins->Find("Start_ -> Start"), 0, smb_ind->Find("$"));
		start_itemset->Insert(start_item);

		C->Insert(Closure(start_itemset));

		vector<ItemSet*> wait_add;
		vector<pair<int, pair<ItemSet*, Symble*>>> wait_add_goto;
		vector<ItemSet*> next_process;
		for (auto pr : *C->Traverse())
		{
			next_process.push_back(pr);
		}

		while (wait_add.size()!=0 || next_process.size()!=0)
		{
			time_t t = clock();
			wait_add.clear();
			wait_add_goto.clear();
			for (auto I : next_process)
			{
				for (auto X : smb_ind->GetAllSymble())
				{
					ItemSet* tmp = Goto(I, X);
					if (tmp->Count() == 0)
						continue;
					if (!C->Contain(tmp))
					{
						wait_add.push_back(tmp);
					}
					wait_add_goto.push_back(pair<int, pair<ItemSet*, Symble*>>(I->GetID(), pair<ItemSet*, Symble*>(tmp, X)));
				}
			}
			next_process.clear();
			for (auto iset : wait_add)
			{
				if (C->Insert(iset))
				{
					// 保证相同id
					next_process.push_back(C->Contain(iset));
				}
			}
			for (auto pr : wait_add_goto)
			{
				int i = pr.first;
				ItemSet* J = pr.second.first;
				Symble* X = pr.second.second;
				int a = J->GetID();
				J = C->Contain(J);
	
				InsertTable(i, X, J->GetID());
			}

			cout << "time::" << (clock() - t)/1000 << "add_size:" << wait_add.size() << "next for loop:" << C->Traverse()->size() << endl;
		}
		
	}
	void BuildAction()
	{
		ProductionManager* pdc_ins = &ProductionManager::getInstance();
		SymbleManager* smb_ins = &SymbleManager::getInstance();

		for (ItemSet* I : *C->Traverse())
		{
			for (Item* item : *I->Traverse())
			{
				Symble* a = item->GetDotRight();
				if (a->IsTerminal() && !a->IsNull())
				{
					int j = FindState(I->GetID(), a);
					if (j != -1)
					{
						InsertTable(I->GetID(), a, new Action(ACTION_MOVE, nullptr, j));
					}
				}
				if (a->IsNull() && item->GetLeft()->ToString() != "Start_")
				{
					InsertTable(I->GetID(), item->GetLookForward(), new Action(ACTION_MERGE, item->GetProduct(), -1));
				}
				if (item->Equal(Item::Create(pdc_ins->Find("Start_ -> Start"), 1, smb_ins->Find("$"))))
				{
					InsertTable(I->GetID(), smb_ins->Find("$"), new Action(ACTION_ACC, nullptr, -1));
				}
			}
		}
	}
	void Build(int Debug)
	{
		cout << "loading grammar..." << endl;
		SymbleManager* instance = &SymbleManager::getInstance();
		ProductionManager* instance_ = &ProductionManager::getInstance();
		cout << "building first set..." << endl;
		BuildFirst();
		cout << "building fellow set..." << endl;
		BuildFellow();
		cout << "building item cluser..." << endl;
		items(instance_, instance);
		cout << "computing action..." << endl;

		BuildAction();
		if (Debug == 1)
		{
			instance_->Print();
			instance->Print();
			Print();
		}
	}
	int GetStart()
	{
		return start_state;
	}
	stack<int>* GetStack() { return stk; }
};