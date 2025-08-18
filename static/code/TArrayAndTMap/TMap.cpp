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
	// ��������0.7
	const float loadFactor = 0.7;

	size_t hash(const K& key) const {
		// ���ñ�׼��Ĺ�ϣ���� ����һ����ϣֵ
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
		// ������������0.7���ٹ�ϣ
		if (static_cast<float>(count) / table.size() > loadFactor) {
			rehash();
		}
		size_t index = hash(key);
		while (table[index].state == EntryState::Occupied) {
			if (table[index].key == key) {
				table[index].value = value;
				return;
			}
			// ��ֹ�������鷶Χ
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
			// ��ֹ����ɾ�����ļ�ֵ��Ȼȥ�Ա� ���տ��ܻ᷵��һ���Ѿ�ɾ���ļ�ֵ�� 
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
		��STL��map�У�ʹ�ò�����[] ���ʲ����ڵļ�ʱ�����Զ����һ����ֵ��Ԫ�أ������������TMap��ͬ���ǣ�������[] �����˲����ڵļ��ᴥ�����ԣ�
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
	cout << "��Ϊ2��ֵ��" << *map.find(2) << endl;
	cout << "��ֵ��������" << map.num() << endl;
	map.remove(2);
	cout << "ɾ�����ֵ��������" << map.num() << endl;
	if (!map.find(2)) {
		cout << "�����ڼ�Ϊ2�ļ�ֵ��" << endl;
	}
	map[2] = "orange";
	cout << map[2];
	return 0;
}

/*
	��Ϊ2��ֵ��banana
	��ֵ��������2
	ɾ�����ֵ��������1
	�����ڼ�Ϊ2�ļ�ֵ��
	orange
*/