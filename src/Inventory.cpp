#include "Inventory.hpp"

Inventory::Inventory():
	m_wood_inventory(0),
	m_berry_inventory(0),
	m_fish_inventory(0)
{
}

void Inventory::addWood(int _amount)
{
	m_wood_inventory += _amount;
}

void Inventory::addBerries(int _amount)
{
	m_berry_inventory += _amount;
}

void Inventory::addFish(int _amount)
{
	m_fish_inventory += _amount;
}

int Inventory::takeWood(int _amount)
{
	int taken = 0;
	if(_amount > 0)
	{
		if (_amount > m_wood_inventory)
		{
			taken = m_wood_inventory;
			m_wood_inventory = 0;
		}
		else
		{
			taken = _amount;
			m_wood_inventory -= _amount;
		}
		return taken;
	}
	else
		return -1;
}

int Inventory::takeBerries(int _amount)
{
	int taken = 0;
	if(_amount > 0)
	{
		if (_amount > m_berry_inventory)
		{
			taken = m_berry_inventory;
			m_berry_inventory = 0;
		}
		else
		{
			taken = _amount;
			m_berry_inventory -= _amount;
		}
		return taken;
	}
	else
		return -1;
}

int Inventory::takeFish(int _amount)
{
	int taken = 0;
	if(_amount > 0)
	{
		if (_amount > m_fish_inventory)
		{
			taken = m_fish_inventory;
			m_fish_inventory = 0;
		}
		else
		{
			taken = _amount;
			m_fish_inventory -= _amount;
		}
		return taken;
	}
	else
		return -1;
}
