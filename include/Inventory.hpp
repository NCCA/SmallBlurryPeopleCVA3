#ifndef __INVENTORY_HPP__
#define __INVENTORY_HPP__

/// @file Inventory.hpp
/// @brief The world inventory, globally shared across storehouses
///

/// @class Inventory
/// @brief The Inventory class for management of the global inventory as accessed by the character through storehouses
///
class Inventory
{
	public:
		///
		/// @brief ctor, sets up intial values of inventory items
		///
		Inventory();
		///
		/// @brief destructor
		///
		~Inventory() = default;
		///
		/// @brief getWoodInventory, get the amount of wood form the global inventory
		/// @return m_wood_inventory, amount of wood in the inventory
		///
		int getWoodInventory() {return m_wood_inventory;}
		///
		/// @brief getBerryInventory, get the amount of berries from the global inventory
		/// @return m_berry_inventory, amount of berries in the inventory
		///
		int getBerryInventory() {return m_berry_inventory;}
		///
		/// @brief getFishInventory, get the amount of fishes from the global inventory
		/// @return m_fish_inventory, amount of fishes in the inventory
		///
		int getFishInventory() {return m_fish_inventory;}
		///
		/// @brief addWood, add wood to the global inventory
		/// @param _amount, amount of wood being added to the inventory
		///
		void addWood(int _amount);
		///
		/// @brief addBerries, add berries to the global inventory
		/// @param [in] _amount, amount of berries being added to the inventory
		///
		void addBerries(int _amount);
		///
		/// @brief addFish, add fishes to the global inventory
		/// @param [in] _amount, amount of fishes being added to the inventory
		///
		void addFish(int _amount);
		///
		/// @brief takeWood, take wood from the global inventory
		/// @param [in] _amount, amount of wood wanting to be taken
		/// @return amount of wood available/the amount asked for
		///
		int takeWood(int _amount);
		///
		/// @brief takeBerries, take berries from the global inventory
		/// @param [in] _amount, amount of berries wanting to be taken
		/// @return amount of berries available/the amount asked for
		///
		int takeBerries(int _amount);
		///
		/// @brief takeFish, take fish from the global inventory
		/// @param [in] _amount, amount of fishes wanting to be taken
		/// @return amount of fishes available/ the amount asked for
		///
		int takeFish(int _amount);

	private:
		///
		/// @brief m_wood_inventory, amount of wood in the global inventory
		///
		int m_wood_inventory;
		///
		/// @brief m_berry_inventory, amount of berries in the global inventory
		///
		int m_berry_inventory;
		///
		/// @brief m_fish_inventory, amount of fishes in the global inventory
		///
		int m_fish_inventory;
};

#endif//__INVENTORY_HPP__
