#include "pch.h"
#include <iostream>
#include <string>
#include <random> //для rand(),srand()
#include <ctime> //для time()

using namespace std;

int getRandomNumber(int min, int max) {
	//	функция возвращает случайное число между min и max
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

class Creature {
	//	класс существо используется для описания и игрока, и монстров
protected:
	string m_name;	// имя
	char m_symb;	// символ
	int m_hp;		// здоровье
	int m_damage;	// наносимый урон
	int m_gold;		// золото
public:
	//	конструктор, инициализирует все параметры существа
	Creature(string name, char symb, int hp, int damage, int gold) :
		m_name{ name }, m_symb{ symb }, m_hp{ hp }, m_damage{ damage }, m_gold{ gold } {}

	const string& getName() { return m_name; }
	char getSymbol() { return m_symb; }
	int getHealth() { return m_hp; }
	int getDamage() { return m_damage; }
	int getGold() { return m_gold; }

	void reduceHealth(int hit) { m_hp -= hit; }	// уменьшение здоровья
	bool isDead() { return m_hp <= 0; }	// проверка жив или мертв
	void addGold(int moreGold) { m_gold += moreGold; }	// добавление золота
};

class Player : public Creature {
private:
	int m_level = 1;	// уровень игрока = урон
	int m_mp;		// уровень маны
public:
	// конструктор игрока с его именем, 10hp, 1 урон, 0 золота и 1 уровень
	Player(string name) : 
		Creature(name, '@', 10, 1, 0), m_level{ 1 }, m_mp{ 0 } {}

	int getLevel() { return m_level; }
	int getMp() { return m_mp; }

	void levelUp() { // поднятие уровня
		
		//	увеличение статусов игрока
		m_level++; m_damage++;
		if (m_level >= 5) m_mp++;
	}
	bool hasWon() { return m_level >= 20; }	// игрок победил
	void healthPotion(int health) { m_hp += health; }	// игрок нашел зелье здоровья
	void reduceMp(int points) { m_mp -= points; }	// уменьшение маны
	void displayInformation() {
		cout << "Player " << m_name << " has " << m_hp << " health";
		if (m_level >= 5)
			cout << " and " << m_mp << " mana\n";
		else
			cout << endl;
	}
};

class Monster :public Creature {
public:
	enum Type {	// типы монстров
		DRAGON,
		KNIGHT,
		SKELETON,
		ORC,
		ZOMBIE,
		RAT,
		SLIME,
		MAX_TYPES
	};
	struct MonsterData {	// структура для описания информации о монстрах
		string name;
		char symb;
		int hp;
		int damage;
		int gold;
	};
	static MonsterData monsterData[MAX_TYPES];

	//	конструктор монстра, передается тип монстра, на основе типа все данные берутся из monsterData
	Monster(Type type) : Creature(monsterData[type].name, monsterData[type].symb,
		monsterData[type].hp, monsterData[type].damage, monsterData[type].gold) {}

	static Monster getRandomMonster(bool start) {

		//	возвращает случайного монстра
		//	если игрок в начале игры, вовзращает более слабых монстров
		if (start)
			return (Monster(static_cast<Type>(getRandomNumber(MAX_TYPES / 2, MAX_TYPES - 1))));
		else
			return (Monster(static_cast<Type>(getRandomNumber(0, MAX_TYPES - 3))));
	}
};

Monster::MonsterData Monster::monsterData[Monster::MAX_TYPES]
{
	//	информация по монстрам
	{ "dragon", 'D', 25, 5, 100 },
	{ "knight", 'k' , 15, 3, 50 },
	{ "skeleton", 's', 10, 3, 35},
	{ "orc", 'o', 4, 2, 25 },
	{ "zombie", 'z', 2, 2, 15 },
	{ "rat", 'r', 2, 1, 10},
	{ "slime", 's', 1, 1, 5 }
};

bool attackMonster(Monster &monster, Player &player, bool magic) {
	
	//	игрок атакует монстра
	if (magic) {
		monster.reduceHealth(ceil(1.5*(double)player.getDamage()));
		player.reduceMp(3);
		cout << "You used magic on the " << monster.getName() << ". Hit for " << ceil(1.5*player.getDamage()) << " damage.\n";
	}
	else {
		monster.reduceHealth(player.getDamage());
		cout << "You hit the " << monster.getName() << " for " << player.getDamage() << " damage.\n";
	}

	//	если монстр побежден, игрок поднимается в уровне и получает золото с мностра
	if (monster.isDead()) {
		cout << "You killed the " << monster.getName() << endl;
		player.levelUp();
		player.addGold(monster.getGold());
		cout << "You are now level " << player.getLevel() << ".\n";
		//	игрок открывает магию на 5 уровне
		if (player.getLevel() == 5) {
			cout << "\tYou feel something strange...something powerfull within you awakening\n";
			cout << "\tYou now can use magic spells!\n";
			cout << "\tSpells are more powerfull than usual hits, but cost 3 mana (you will gain 1 each level up)\n";
			cout << "\tHave fun!\n";
		}
		cout << "You found " << monster.getGold() << " gold.\n";
		return true; // монстр убит
	}

	return false; // монстр жив
}

bool attackPlayer(Monster &monster, Player &player) {
	
	//	монстр атакует игрока
	player.reduceHealth(monster.getDamage());
	cout << "The " << monster.getName() << " hit you for " << monster.getDamage() << " damage.\n";

	//	если игрок умирает, выводится сообщение о смерти
	if (player.isDead()) {
		cout << "You died at level " << player.getLevel() << " and with " << player.getGold()
			<< " gold.\nToo bad you can't take it with you!\n";
		return true; // игрок убит
	}
	
	//	если игрок жив, выводится оставшееся hp
	player.displayInformation();
	return false; // игрок жив
}

bool fightMonster(Monster &monster, Player &player) {
	//	драка с монстром, возвращает true при смерти игрока, false при смерти монстра
	player.displayInformation();
	cout << "You have encountered a " << monster.getName() << " (" <<
		monster.getSymbol() << ").\n";
	while (true) {
		char answer;
		//	бежать или драться с монстром?
		if (player.getLevel() >= 5) {
			while (true)
			{
				cout << "(R)un, (H)it or use (M)agic : ";
				cin >> answer;
				if ((cin.fail()) || ((answer != 'r') && (answer != 'R') && (answer != 'h') && (answer != 'H')
					&& (answer != 'm') && (answer != 'M')))
				{
					cin.clear();
					cin.ignore(32767, '\n');
					cout << "You entered something wrong, please answer again.\n";
				}
				else if ((answer == 'm') || (answer == 'M')) {
					if (player.getMp() < 3)
						cout << "You don't have enough mana to use magic now.\n";
				}
				else break;
			}
		}
		else {
			while (true)
			{
				cout << "(R)un or (F)ight : ";
				cin >> answer;
				if ((cin.fail()) || ((answer != 'r') && (answer != 'R') && (answer != 'f') && (answer != 'F')))
				{
					cin.clear();
					cin.ignore(32767, '\n');
					cout << "You entered something wrong, please answer again.\n";
				}
				else break;
			}
		}
		//	игрок решил бежать, шанс убежать 50%
		if (answer == 'r' || answer == 'R') {
			bool chance = getRandomNumber(0, 1);
			if (chance) {
				cout << "You successfully fled.\n";
				return false; // игрок сбежал
			}
			else {
				//	при неудачном побеге монстр атакует
				cout << "You couldn't run away.\n";
				if (attackPlayer(monster, player)) return true; // игрок умер
			}
		}
		else {

			//	игрок решил драться
			bool magic(false);
			//	игрок выбрал атаку магией
			if ((answer == 'm') || (answer == 'M')) { magic = true; }

			//	1 атакует игрок, потом монстр
			if (attackMonster(monster, player, magic)) return false; // монстр умер
			if (attackPlayer(monster, player)) return true; // игрок умер
		}
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0))); // устанавливаем значение системных часов в качестве стартового числа
	rand(); // сбрасываем первый результат

	//	запрос имени игрока
	cout << "Enter your name: ";
	string playerName;
	getline(std::cin, playerName);
	Player player(playerName);

	//	приветствие игрока
	cout << "Welcome, " << player.getName() << ".... I don't really know where you are...\n";
	cout << "Seems like this is a dungeon. Look there is somethimg written on the door!\n";
	cout << "\t<< yOu DO noT haVE a cHOicE >>\n";
	cout << "\t<< YoU eiThER FigHT or Run >>\n";
	cout << "\t<< nOthInG ELse >>\n";
	cout << "\t<< NoTHing >>\n";
	cout << "Hmmmm..... Seems strange.\n";
	cout << "Well, let's go through the door.\n";


	//	цикл игры
	while (!player.hasWon()) {

		//	шанс на зелье здоровья 10% (20% если у игрока 3hp или меньше), лечит от 5 до 10 hp
		int luck = getRandomNumber(1, 10);
		if ((luck == 7 && player.getHealth() <= 15) || (luck < 3 && player.getHealth() <= 3)) {
			cout << "Wow, you're lucky. You found a health potion.\n";
			int health = getRandomNumber(5, 10);
			player.healthPotion(health);
			cout << "You recovered " << health << " health. And now have " << player.getHealth() << " hp.\n";
		}

		//	определяется уровень игрока, если он меньше или равен 5, то при генерации монстра ему не выпадут более сложные монстры
		bool start;
		if (player.getLevel() <= 5)
			start = true;
		else
			start = false;
		Monster m = Monster::getRandomMonster(start);
		//	драка с монстром, возвращает true при смерти игрока
		if (fightMonster(m, player)) break;

	}

	//	сообщение о победе игрока
	if (player.hasWon())
		cout << "Congratulations, you reached level 20, so you get nothing! You lose! Good day, sir!\n";

	return 0;
}
