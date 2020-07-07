#ifndef _List_h_
#define _List_h_

class PCB;

class List
{
public:
	List();
	void add(PCB* data);
	void remove(unsigned id);
	PCB* find(unsigned id);
	PCB* get();
	int numOfElem();
	PCB* elem(int i);
	//void print();
//private:
	PCB* list[1000];
	int size;
};
#endif
