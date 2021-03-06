/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#ifndef immutable_hash_map_h
#define immutable_hash_map_h

// a simple hash map optimized for simple, immutable data

#include <inttypes.h>
#include <math.h>
#include <vector>

template<typename key_type, typename value_type>
class immutable_hash_map
{
public:
    std::vector<key_type> _keys;
    std::vector<value_type> _values;
    int _size;
    int _mask;

    immutable_hash_map(key_type* keys, value_type* values, int size)
	: _size(size)
    {
	if (size <= 0) {
            _keys.resize(1,"");
	    _values.resize(1);
	    _mask = 0;
	    return;
	}

	// build table, size = (nearest power of two >= size*2)
	int table_size = 1<<int(ceil(log(size * 2) * (1/M_LN2)));
	_mask = table_size-1;
	
	_keys.resize(table_size);
	_values.resize(table_size);

	key_type* kp = keys, *end = kp + size;
	value_type* vp = values;
	while (kp != end) {
	    int pos = find(*kp);
	    _keys[pos] = *kp++;
	    _values[pos] = *vp++;
	}
    }

    const value_type& operator[] (const key_type& key) const
    {
        return _values[find(key)];
    }

    int size() const { return _size; }

private:
    int find(const key_type& key) const
    {
	uint32_t hash = intptr_t(key) ^ (intptr_t(key)>>32);
	// hash function from Thomas Wang (wang@cup.hp.com)
	hash = ~hash + (hash << 15);
	hash = hash ^ (hash >> 12);
	hash = hash + (hash << 2);
	hash = hash ^ (hash >> 4);
	hash = hash * 2057;
	hash = hash ^ (hash >> 16);
	while (1) {
	    int pos = hash & _mask;
	    const key_type& k = _keys[pos];
	    if (k == key || !k) return pos; // found key or blank
	    hash++; // collision, keep looking
	}
    }
};

#endif
