#pragma once
/// ����û��ʵ��manager�࣬����ÿһ��set���洢�����ָ�룡�� T->*
/// ÿ�����϶�Ӧ����һ��ID�������������ģ�

#include <set>
#include <string>
#include <algorithm>

using namespace std;
template<typename T>
class Set
{
private:
	set<T> data;
	int set_id;
	static int id;
private:
	Set(int _set_id) : set_id(_set_id) 
	{
		data = set<T>();
	}
public:
	~Set() {}
	set<T>* Traverse();
	bool IsNull();
	bool Insert(T);
	T Contain(T);
	int Difference(Set<T>*);
	int Union(Set<T>*);
	static Set<T>* Create();
	void Print();
	string ToString();
	int Count();
	int ElementCount();
	bool Equal(Set<T>*);
	int GetID() { return set_id; }
};
template<typename T> int Set<T>::id = 0;
/// <summary>
/// ֱ�ӷ��ؼ��ϲ��ã������б�Ҫô��û�б�Ҫ��
/// </summary>
/// <typeparam name="T"></typeparam>
/// <returns></returns>
template<typename T>
set<T>* Set<T>::Traverse()
{
	return &(this->data);
}

template<typename T>
bool Set<T>::IsNull()
{
	return this->data.size() == 0;
}

template<typename T>
bool Set<T>::Insert(T element)
{
	if (this->Contain(element))
	{
		return false;
	}
	pair<set<T>::iterator, bool> ret = data.insert(element);
	if (ret.second)
	{
		return true;
	}
	return false;
}

template<typename T>
T Set<T>::Contain(T element)
{
	//typename set<T>::iterator ret = data.find(element);
	//if (ret != data.end())
	//{
	//	return true;
	//}
	//return false;
	for (auto pr : data)
	{
		if (element->Equal(pr))
		{
			return pr;
		}
	}
	return nullptr;
}

template<typename T>
int Set<T>::Difference(Set<T>* other)
{
	int s = 0;
	for (auto p : *other->Traverse())
	{
		if (Contain(p))
		{
			data.erase(p);
			s += 1;
		}
	}
	return s;
}

template<typename T>
int Set<T>::Union(Set<T>* other)
{
	int s = 0;
	for (T element: *other->Traverse())
	{
		s += Insert(element);
	}
	return s;
}

template<typename T>
Set<T>* Set<T>::Create()
{
	return new Set<T>((Set<T>::id)++);
}

template<typename T>
void Set<T>::Print()
{
	cout << "{" << endl;
	for (T ele : data)
	{
		cout << "\t" << (ele->ToString()) << "," << endl;
	}
	cout << "}" << endl;
}

template<typename T>
string Set<T>::ToString()
{
	string a = "[";
	for (auto pr : data)
	{
		a += (pr->ToString() + ", ");
	}
	a += "]";
	return a;
}

template<typename T>
int Set<T>::Count()
{
	return data.size();
}

template<typename T>
int Set<T>::ElementCount()
{
	int s = 0;
	for (auto p : data)
	{
		s += p->ElementCount();
	}
	return s;
}

template<typename T>
bool Set<T>::Equal(Set<T>* st)
{
	if (st->ElementCount() != this->ElementCount())
	{
		return false;
	}
	for (auto pr : *(st->Traverse()))
	{
		if (!this->Contain(pr))
		{	
			return false;
		}
	}
	return true;
}

// ��Ӽ���Ƿ��ڼ����е��жϣ� �ж�Ԫ���Ƿ��ڼ����У� ���Ǳ������ϣ� �ж��Ƿ������ͬԪ�أ� ����Ԫ�ص�Equal()������