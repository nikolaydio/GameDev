#pragma once

#include <cstdint>
#include <memory>

//This class gives absolute no garancy that constructor/destructors will be called
//The element may be relocated at any given time. Use IDs instead.

typedef uint16_t ARRAY_ID;
typedef ARRAY_ID ARRAY_INDEX;
#define INVALID_ID UINT16_MAX

template<class T>
class SparseArray {
	struct Index {
		ARRAY_INDEX offset;
	};
	struct Element {
		ARRAY_ID id;
		T data;
	};
	Element* elements;
	Index* index;
	ARRAY_ID element_count;
	ARRAY_ID max_element_capacity;
public:
	SparseArray() : elements(0), index(0), element_count(0),
		max_element_capacity(0) { }

	//**can throw elements out**
	void SetCapacity(int new_capacity) {
		if(new_capacity == max_element_capacity) {
			return;
		}
		Element* new_elements = new Element[new_capacity];
		Index* new_indices = new Index[new_capacity];

		int entry_count = std::min<int>(new_capacity, element_count);

		if(entry_count != 0) {
			if(elements)
				memcpy(new_elements, elements, sizeof(Element) * entry_count);
			if(index)
				memcpy(new_indices, index, sizeof(Index) * entry_count);
		}
		for(int i = entry_count; i < new_capacity; ++i) {
			new_indices[i].offset = INVALID_ID;
		}

		delete elements;
		delete index;

		elements = new_elements;
		index = new_indices;

		max_element_capacity = new_capacity;
		element_count = entry_count;
	}

	inline int GetCapacity() {
		return max_element_capacity;
	}

	inline int GetElementCount() {
		return element_count;
	}
	inline bool has(ARRAY_ID id) {
		if(id >= max_element_capacity) {
			return false;
		}
		return index[id].offset != INVALID_ID;
	}
	inline T& lookup(ARRAY_ID id) {
		return elements[index[id].offset];
	}
	inline void get_by_index(ARRAY_INDEX index, ARRAY_ID *id, T** data) {
		*id = elements[index].id;
		*data = &elements[index].data;
	}

	inline ARRAY_INDEX AllocateID(ARRAY_ID id) {
		if(id >= max_element_capacity - 1) {
			return INVALID_ID;
		}


		if(has(id)) {
			return index[id].offset;
		}

		index[id].offset = element_count;
		element_count++;
	}
	inline void RemoveID(ARRAY_ID id) {
		if(!has(id)) {
			return;
		}
		ARRAY_INDEX idx = index[id].offset;
		
		if(idx != element_count)
			elements[idx] = elements[element_count];

		index[id].offset = INVALID_ID;
		element_count--;
	}

};