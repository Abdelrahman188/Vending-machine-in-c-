#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>  // Change to unordered_map
#include<map>
using namespace std;

enum coins { penny = 1, nickel = 5, dime = 10, quarter = 25, dollar = 100, twodollar = 200 };

class product {
private:
	string item;
	int price;
public:
	product() = default;
	product(string x, int y) : item(x), price(y) {}

	string getname() const { return item; }
	int get_price() const { return price; }

	void setname(const string& x) { item = x; }
	void set_price(const int& y) { price = y; }

	bool operator==(const product& other) const {
		return (item == other.getname() && price == other.get_price());
	}
};

// Hash specialization for product
namespace std {
	template <>
	struct hash<product> {
		size_t operator()(const product& p) const {
			return hash<std::string>()(p.getname()) ^ hash<int>()(p.get_price());
		}
	};
}

class coin {
private:
	int x;
public:
	coin(coins y) : x(y) {}

	void set_coin(int y) { x = y; }
	int get_coin() const { return x; }

	friend bool operator==(const coin& z, const coin& other) {
		return z.get_coin() == other.get_coin();
	}
};

// Hash specialization for coin
namespace std {
	template <>
	struct hash<coin> {
		size_t operator()(const coin& p) const {
			return hash<int>()(p.get_coin());
		}
	};
}

// Inventory class template
template<class T>
class inventory {
private:
	unordered_map<T, int> invent;
public:
	void add_item(const T& obj) {
		invent[obj]++;
	}

	void remove_item(const T& obj) {
		auto it = invent.find(obj);
		if (it != invent.end() && invent[obj] > 0)
			invent[obj]--;
	}

	void set_quantity(const T& obj, const int& x) {
		invent[obj] = x;
	}

	int get_quantity(const T& obj) {
		auto it = invent.find(obj);
		if (it != invent.end())
			return it->second;
		else
			return -1;  // Item not found
	}

	bool check_available(const T& obj) {
		auto it = invent.find(obj);
		if (it == invent.end() || it->second == 0)
			return false;
		return true;
	}

	void put(const T& y, const int& x) {
		invent[y] = x;
	}

	void clear() {
		invent.clear();
	}
};

template<class def ,class T>
class par {
private:
	def x;  // Item of generic type 'def'
	T y;  // Quantity of the item
public:
		// Constructor: public to allow object creation
	

	// Getter for item
	def get_item() { return x; }

	// Getter for quantity
	T get_quantity() { return y; }

	// Setter for item
	void set_item(def z) { x = z; }

	// Setter for quantity
	void set_quantity(T m) {y = m; }
};
// Custom exception class for insufficient funds
class InsufficientFundsException : public std::exception {
private:
	std::string message;
public:
	InsufficientFundsException(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class vending_machine {
private:
	// products inventory 
	inventory<product> database;
	//register
	inventory<coin> registr;
	//
	product selecteditem;
	int current_balance;
	vector<coin> accepted_coin;

public:
	vending_machine(vector<product> products, vector<int>product_quantity, vector<int>coins_quantity, vector<coin>coin) {
		accepted_coin = coin;
		current_balance = 0;
		for (int i = 0; i < products.size(); i++)
			database.put(products[i], product_quantity[i]);
		for (int i = 0; i < coins_quantity.size(); i++) {
			registr.put(coin[i], coins_quantity[i]);
			current_balance += (coin[i].get_coin() * coins_quantity[i]);
			cout << accepted_coin[i].get_coin() << endl;
		}
	}
	int select_item_and_get_price(const product& item) {
		bool condition = database.check_available(item);
		if (condition == true) {
			selecteditem = item;
			return item.get_price();
		}
		else
			return -1;
	}
	par<product, inventory<coin>> pay(vector<int>coins_quantity) {
		// declare the bucket will be returned
		// product 
		// inventory type which has accepted coin list with quantity of each coin

		par<product, inventory<coin>> bucket;

		//refund is the object will be returned has the coins and their amount

		inventory<coin> refnd;
		// intializing the refund by adding all the coins and zero quantity
		for (int i = 0; i < coins_quantity.size(); i++)
			refnd.put(accepted_coin[i], 0);
		//check sufficient money
		int money = 0;
		for (int i = 0; i < accepted_coin.size(); i++)
			money += accepted_coin[i].get_coin() * coins_quantity[i];
		if (money < selecteditem.get_price()) {
			//exception
			throw InsufficientFundsException("Insufficient funds. Please insert more coins.");
		}
		cout << money << endl;
		// Calculate refund/change amount
		int refund = money - selecteditem.get_price();
		if (refund == 0) {

			bucket.set_item(selecteditem);
			bucket.set_quantity(refnd);
			database.remove_item(selecteditem);
			return bucket; // No change needed
		}

		// Calculate change to return
		for (int i = accepted_coin.size() - 1; i >= 0 && refund > 0; i--) {
			cout << i << endl;
			int coin_value = accepted_coin[i].get_coin();
			int coins_needed = refund / coin_value;
			
			// Only give change if there are enough coins in the register
			if (registr.get_quantity(accepted_coin[i]) > 0) {
				int coins_to_give = min(coins_needed, registr.get_quantity(accepted_coin[i]));
				refund -= coins_to_give * coin_value;
				refnd.set_quantity(accepted_coin[i], coins_to_give);
				registr.set_quantity(accepted_coin[i], registr.get_quantity(accepted_coin[i]) - coins_to_give);
			}
		}
		cout << "xxxxxxxxxxxxxxxxxxxxxx";
		if (refund > 0) {

			throw InsufficientFundsException("Cannot provide exact change.");
		}
		database.remove_item(selecteditem);
		// Set selected item and refund amount
		bucket.set_item(selecteditem);
		bucket.set_quantity(refnd);
		return bucket;


	}

};

	// Test function to simulate the vending machine
	int main() {
		// Define available products
		vector<product> products = {
			product("CANDY", 10),
			product("SNACK", 50),
			product("NUTS", 20),
			product("COKE", 25)
		};

		// Initial product quantities
		vector<int> product_quantities = { 10, 10, 5, 7 };  // Quantities of each product

		// Define available coins
		vector<coin> accepted_coins = { coin(penny), coin(nickel), coin(dime), coin(quarter), coin(dollar), coin(twodollar) };
		cout << accepted_coins[0].get_coin();
		cout << accepted_coins[1].get_coin();
		cout << accepted_coins[2].get_coin();
		// Initial coin quantities in the vending machine register
		vector<int> coin_quantities = { 100, 100, 50, 40, 30,2 };

		// Create vending machine
		vending_machine vm(products, product_quantities, coin_quantities, accepted_coins);

		// Simulate selecting an item (COKE which costs 25)
		int pre = vm.select_item_and_get_price(products[3]);  // Selecting "COKE"
		cout << "Selected item: COKE, Price: " << pre << " cents" << endl;

		// Insert coins for payment
		vector<int> inserted_coins = { 0, 0, 0, 1, 0, 80 };  // Insert 1 quarter (25 cents)

	
			// Make payment
			
			try {
				auto result = vm.pay(inserted_coins);
				cout << "Payment successful! Enjoy your " << result.get_item().getname() << "." << endl;
				// Check if there's any change
				inventory<coin> refund = result.get_quantity();
				for (int i = 0; i < accepted_coins.size(); i++) {
					int qty = refund.get_quantity(accepted_coins[i]);
					cout << "Returned " << qty << " coins of " << accepted_coins[i].get_coin() << " cents." << endl;

				}
			}
			catch (InsufficientFundsException e) {
				cout << e.what();
		}
	
		return 0;
	}

