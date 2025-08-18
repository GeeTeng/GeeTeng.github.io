#include<iostream>
#include<vector>
#include<string.h>
using namespace std;

template<typename K, typename V>
class TMap {
private:
	enum class EntryState {
		Empty,
		Occupied,
		Deleted
	};

	struct Entry {
		K key;
		V value;
		EntryState state = EntryState::Empty;
	};

	vector<Entry> table;
	size_t count = 0;
	// 负载因子0.7
	const float loadFactor = 0.7;

	size_t hash(const K& key) const {
		// 调用标准库的哈希函数 返回一个哈希值
		return std::hash<K>{}(key) % table.size();
	}

	void rehash() {
		vector<Entry> oldTable = table;
		table.resize(table.size() * 2);
		for (Entry& entry : table) {
			entry.state = EntryState::Empty;
		}
		count = 0;
		for (const Entry& entry : oldTable) {
			if (entry.state == EntryState::Occupied) {
				add(entry.key, entry.value);
			}
		}
	}

public:
	TMap(size_t initSize = 16) {
		table.resize(initSize);
	}

	void add(const K& key, const V& value) {
		// 超过负载因子0.7就再哈希
		if (static_cast<float>(count) / table.size() > loadFactor) {
			rehash();
		}
		size_t index = hash(key);
		while (table[index].state == EntryState::Occupied) {
			if (table[index].key == key) {
				table[index].value = value;
				return;
			}
			// 防止超出数组范围
			index = (index + 1) % table.size();
		}
		table[index].key = key;
		table[index].value = value;
		table[index].state = EntryState::Occupied;
		++count;
	}

	V* find(const K& key) {
		size_t index = hash(key);
		size_t startIndex = index;
		while (table[index].state != EntryState::Empty) {
			// 防止遇到删除过的键值仍然去对比 最终可能会返回一个已经删除的键值了 
			if (table[index].state == EntryState::Occupied && table[index].key == key) {
				return &table[index].value;
			}
			index = (index + 1) % table.size();
			if (startIndex == index) break;
		}
		return nullptr;
	}

	void remove(const K& key) {
		size_t index = hash(key);
		size_t startIndex = index;
		while (table[index].state != EntryState::Empty) {
			if (table[index].state == EntryState::Occupied && table[index].key == key) {
				table[index].state = EntryState::Deleted;
				--count;
				return;
			}
			index = (index + 1) % table.size();
			if (startIndex == index) break;
		}
	}

	/*
		在STL的map中，使用操作符[] 访问不存在的键时，会自动添加一个键值对元素，而在虚幻引擎TMap不同的是，操作符[] 访问了不存在的键会触发断言；
	*/
	V& operator[](const K& key) {
		V* findKey = find(key);
		if (findKey) {
			return *findKey;
		}
		else {
			add(key, V{});
			return *find(key);
		}
	}

	size_t num() const {
		return count;
	}
};

int main() {
	TMap<int, string> map;
	map.add(1, "apple");
	map.add(2, "banana");
	cout << "键为2的值是" << *map.find(2) << endl;
	cout << "键值对数量：" << map.num() << endl;
	map.remove(2);
	cout << "删除后键值对数量：" << map.num() << endl;
	if (!map.find(2)) {
		cout << "不存在键为2的键值对" << endl;
	}
	map[2] = "orange";
	cout << map[2];
	return 0;
}

/*
	键为2的值是banana
	键值对数量：2
	删除后键值对数量：1
	不存在键为2的键值对
	orange
*/