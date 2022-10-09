#pragma once
#include <vector>

#include "element.h"
#include <shlobj.h> // 可替换为 windows.h
#include <shellapi.h>



using namespace std;
class Node
{
private:
	vector<Node*> children;
	Node* parent = nullptr;
	void* data;
	static int counter;
	int id;
	Node(int _id):id(_id) {}
	Node(int _id, void* _data) :id(_id), data(_data) {}
	~Node() { parent = nullptr; children.clear(); data = nullptr; }
public:
	static Node* Create() { return new Node(counter++); }
	static Node* Create(void* _data) { return new Node(counter++, _data); }

	vector<Node*> Children() 
	{
		return children;
	}
	Node* Parent()
	{
		return parent;
	}
	void SetParent(Node* par)
	{
		parent = par;
	}
	void AddChild(Node* child)
	{
		children.push_back(child);
	}
	template<typename T> friend T GetData(Node*);
	friend bool CheckCircle(Node*);
	string ToString() { return to_string(id); }
};
int Node::counter = 0;
// 友元函数用于user_data指针类型转换
template<typename T> T GetData(Node* node)
{
	return reinterpret_cast<T>(node->data);
}

bool _CheckCircle(Node* node, set<Node*>* st)
{
	if (!st->insert(node).second) 
	{ 
		return true; 
	}
	bool check = false;
	for (auto pr : node->Children())
	{
		check = check || _CheckCircle(pr, st);
	}
	return check;
}

bool CheckCircle(Node* node) 
{
	// 这里仅仅需要判断节点id大小即可
	set<Node*> flag;
	return _CheckCircle(node, &flag);
}

string _TreeView(Node* node)
{
	string children = "[";
	for (auto pr : node->Children())
	{
		children += _TreeView(pr) + ", ";
	}
	children += "]";
	if (node->Children().size() > 0)
	{
		return "{name:\'" + GetData<Product*>(node)->GetLeft()->ToString()  + "\',children:" + children + "}";
	}
	else
	{
		return "{name:\'" + GetData<Symble*>(node)->ToString()  +  "\'}";
	}
}
void TreeView(Node* node)
{
	string prefix = R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <title>ECharts</title>
    <!-- 引入刚刚下载的 ECharts 文件 -->
    <script src="echarts.min.js"></script>
</head>
<body>
<!-- 为 ECharts 准备一个定义了宽高的 DOM -->
<div id="main" style="width: 2560px;height:1440px;"></div>
<script type="text/javascript">
var chartDom = document.getElementById('main');
var myChart = echarts.init(chartDom);
var option;
// 生成数据)";
	string tail = R"(// 结束生成数据
myChart.showLoading();
myChart.hideLoading();
  data.children.forEach(function (datum, index) {
    index % 2 === 0 && (datum.collapsed = true);
  });
  myChart.setOption(
    (option = {
      tooltip: {
        trigger: 'item',
        triggerOn: 'mousemove'
      },
      series: [
        {
          type: 'tree',
          data: [data],
          left: '2%',
          right: '2%',
          top: '8%',
          bottom: '20%',
          symbol: 'emptyCircle',
          orient: 'vertical',
          expandAndCollapse: true,
          label: {
            position: 'top',
            rotate: 0,
            verticalAlign: 'middle',
            align: 'right',
            fontSize: 9
          },
          leaves: {
            label: {
              position: 'bottom',
              rotate: 0,
              verticalAlign: 'middle',
              align: 'left'
            }
          },
          animationDurationUpdate: 750
        }
      ]
    })
  );
option && myChart.setOption(option);
</script>
</body>
</html>)";
	ofstream writer("index.html", ofstream::app);
	writer << prefix << endl << "var data=" << _TreeView(node) << tail;
	writer.close();
	//WinExec("D://Program Files//Test//Test.exe", SW_SHOWMAXIMIZED);
	HINSTANCE hRslt = ShellExecute(NULL, L"open", L"index.html", NULL, NULL, SW_SHOWNORMAL);

}


void Travarse(Node* node)
{
	//if (node->Children().size() == 0)return;
	for (auto pr : node->Children())
	{
		Travarse(pr);
	}
	if (node->Children().size() >= 1)
	{
		cout << GetData<Product*>(node)->ToString() << endl;
	}
	else
	{
		cout << GetData<Symble*>(node)->ToString() << endl;

	}
	cout << node->ToString() << endl;
}