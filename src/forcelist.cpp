#include "forcelist.hpp"
//FUCK THE STL CONTAINER
ForceList::ForceList() {
	head = nullptr;
	tail = nullptr;
	size = 0;
}
ForceList::~ForceList() {
	ActiveForce* af = head;
	while (af != nullptr) {
		ActiveForce *temp = af->next;
		delete af;
		af = temp;
	}
}
//inserts at the back unless the list is empty (head nullptrptr)
//in which case it inserts at the front.
//
void ForceList::insert_back(ActiveForce newF) {
	ActiveForce *temp = new ActiveForce;
	temp->force = newF.force;
	temp->dt = newF.dt;
	temp->next = nullptr;
	if (head == nullptr)
	{
		head = temp;
		tail = temp;
		temp = nullptr;
	}
	else
	{
		tail->next = temp;
		tail = temp;
	}
	size++;	
}



void ForceList::delete_head()
{	
	ActiveForce *temp = new ActiveForce;
	temp = head;
	if (size == 1) {
		delete temp;
		head = nullptr;
		tail = nullptr;
	}
	else {
		head = head->next;
		delete temp;
	}
	temp = nullptr;
	size--;
}

void ForceList::delete_tail() {
	ActiveForce *current = new ActiveForce;
	ActiveForce *previous = new ActiveForce;
	current = head;
	while (current->next != nullptr)
	{
		previous = current;
		current = current->next;
	}
	tail = previous;
	previous->next = nullptr;
	delete current;
	size--;
	if (size == 0) {
		head = nullptr;
		tail = nullptr;
	}
}

void ForceList::delete_pos(int pos) {
	if (pos == size - 1) {
		delete_tail();
		return;
	}
	ActiveForce *current = new ActiveForce;
	ActiveForce *previous = new ActiveForce;
	current = head;
	for (int i = 0; i < pos; i++)
	{
		previous = current;
		current = current->next;
	}
	previous->next = current->next;
	delete current;
	size--;
}

