#pragma once
#include "Inventory.h"


class HealPotion : public Item {
public:
	HealPotion() : Item(0, "Heal Potion", "Restores 25 HP") {};
};