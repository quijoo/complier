#include <iostream>
#include <time.h>
#include <string>

#include "codereader.h"
#include "parser.h"
#include "element.h"
#include "manager.h"
#include "element_set.h"
#include "sentence_parser.h"
#include "treenode.h"


#include <vector>
#include <stack>
#define DEBUG_SETTING 1

typedef Set<Symble*>     SymbleSet;
typedef Set<Item*>       ItemSet;
typedef Set<Set<Item*>*> ItemCluser;

void test();

int main()
{
    setlocale(LC_ALL, "zh-CN");
    InitManager("grammar.txt");

	SymbleManager* instance = &SymbleManager::getInstance();
	ProductionManager* instance_ = &ProductionManager::getInstance();
    AnalyseTable* G = new AnalyseTable();
    G->Build(DEBUG_SETTING);

	string text = "if ( id > NUM ) if ( id < id ) { id = id + NUM ; } for ( id = NUM ; id < NUM ; id ++ ) { if ( id > NUM ) for ( id = NUM ; id < NUM ; id ++ ) if ( id == NUM ) id = id + NUM ; } $";
	//string text = "i n t   H e l l o _ 1 ; $";
	for (auto pr : split(" i n t  ", " "))
	{
		cout << "$" + pr + "$" << endl;
	}
	vector<string> test_text = split(text, " ");
	vector<Symble*> in;
	for (auto s : test_text)
	{
		in.push_back(instance->Find(s));
	}

	int index = 0, state = 0;
	stack<int> stk;
	stack<Node*> node_stack;

	stk.push(G->GetStart());
	node_stack.push(Node::Create(reinterpret_cast<void*>(instance_->Find("Start_ -> Start"))));
    while (1)
    {   
		if (index >= in.size())
		{
			cout << "Unexcept end of code." << endl;
			break;
		}
		state = stk.top();
        Action* act = G->FindAction(state, in[index]);
		if (!act)
		{
			cout << "wrong expression." << "{ " + to_string(index) + ", " + in[index]->ToString() + " }" << endl;
			break;
		}
		else if (act->type == ACTION_MOVE)
		{
			stk.push(act->next_state);
			node_stack.push(Node::Create(reinterpret_cast<void*>(in[index])));
			index++;
		}
		else if (act->type == ACTION_MERGE)
		{

			size_t s = act->pdc->GetRight().size();
			Node* node = Node::Create(reinterpret_cast<void*>(act->pdc));
			while (s)
			{
				stk.pop();
				node->AddChild(node_stack.top());
				node_stack.pop();
				s--;
			}
			stk.push(G->FindState(stk.top(), act->pdc->GetLeft()));
			node_stack.push(node);
			// 输出产生式。
			cout << "[compling] " << act->pdc->ToString() << endl;
		}
		else if (act->type == ACTION_ACC)
		{
			break;
		}
    }
	if (!CheckCircle(node_stack.top()))
	{
		TreeView(node_stack.top());
		//Travarse(node_stack.top());
	}
	else
	{
		cout << "[compling] error when check circle." << endl;
	}
	/*System::String^ target = "index.html";
	System::Diagnostics::Process::Start(target);*/
    return 0;
}