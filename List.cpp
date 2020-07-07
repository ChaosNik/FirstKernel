#include "List.h"
#include "PCB.h"

/*Obicna staticka lista sa linearnim pretrazivanjem*/

List::List()
{
	size=0;
}
void List::add(PCB* data)
{
	list[size]=data;
	++size;
}
void List::remove(unsigned id)
{
	int k=0;
	for(int i=0;i<size-1;++i)
	{
		if(list[i]->id==id)k=1;
		if(k)list[i]=list[i+1];
	}
	--size;
}
PCB* List::find(unsigned id)
{
	PCB* temp=0;
	for(int i=0;i<size;++i)
		if(list[i]->id==id)temp=list[i];
	return temp;
}
PCB* List::get()
{
	PCB* temp=list[0];
	for(int i=0;i<size-1;++i)
		list[i]=list[i+1];
	--size;
	return temp;
}
int List::numOfElem()
{
	return size;
}
PCB* List::elem(int i)
{
	return list[i];
}