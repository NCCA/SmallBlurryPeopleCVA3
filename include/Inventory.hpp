#ifndef __INVENTORY_HPP__
#define __INVENTORY_HPP__

class Inventory
{
	public:
		Inventory();
		~Inventory() = default;

		int getWoodInventory() {return m_wood_inventory;}
		int getBerryInventory() {return m_berry_inventory;}
		int getFishInventory() {return m_fish_inventory;}

		void addWood(int _amount);
		void addBerries(int _amount);
		void addFish(int _amount);

		int takeWood(int _amount);
		int takeBerries(int _amount);
		int takeFish(int _amount);

	private:
		int m_wood_inventory;
		int m_berry_inventory;
		int m_fish_inventory;

};

#endif//__INVENTORY_HPP__
