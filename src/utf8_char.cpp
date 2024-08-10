#include "utf8_char.hpp"
#include <cstring>


/* ==========================================
 *                  utf8_char                
 * ==========================================
 */
utf8_char::utf8_char() :
	_data({ 0, 0, 0, 0, 0}),
	_csize(1)
{}

utf8_char::utf8_char(const char* str) :
	_data({ 0, 0, 0, 0, 0})
{
	_csize = -1;
	if (*str < 128) { _csize = 1; }
	if (*str >= 0b11110000) { _csize = 4; }
	if (*str >= 0b11100000) { _csize = 3; }
	if (*str >= 0b11000000) { _csize = 2; }

	int index = 0;
	char* p = const_cast<char*>(_data);
	while (*str && index < _csize) {
		p[index++] = *str++;
	}
	if (index != _csize) {
		_csize = -1;
	}
	for (;index < 5 - _csize; ++index) {
		*p++ = 0;
	}
}

utf8_char::~utf8_char() {}

int utf8_char::char_size() const { return _csize; }
const char* utf8_char::data() const { return _data; }

bool utf8_char::operator==(const utf8_char& rhs) const {
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	return _csize == rhs.char_size() &&
			_data[0] == rhs._data[0] &&
			_data[1] == rhs._data[1] &&
			_data[2] == rhs._data[2] &&
			_data[3] == rhs._data[3] &&
			_data[4] == rhs._data[4];
}
bool utf8_char::operator!=(const utf8_char& rhs) const {
	return !(*this == rhs);
}
bool utf8_char::operator>(const utf8_char& rhs) const {
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	if (_csize == 4) { return _data[4] > rhs._data[4]; }
	if (_csize == 3) { return _data[3] > rhs._data[3]; }
	if (_csize == 2) { return _data[2] > rhs._data[2]; }
	if (_csize == 1) { return _data[1] > rhs._data[1]; }
	return false;
}
bool utf8_char::operator<(const utf8_char& rhs) const {
	return !(*this > rhs) && !(*this == rhs);
}
bool utf8_char::operator>=(const utf8_char& rhs) const {
	return (*this > rhs) || (*this == rhs);
}
bool utf8_char::operator<=(const utf8_char& rhs) const {
	return (*this < rhs) || (*this == rhs);
}

bool utf8_char::operator==(const utf8_char_view rhs) const {
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	return _csize == rhs.char_size() &&
			_data[0] == rhs._ptr[0] &&
			_data[1] == rhs._ptr[1] &&
			_data[2] == rhs._ptr[2] &&
			_data[3] == rhs._ptr[3] &&
			_data[4] == rhs._ptr[4];
}
bool utf8_char::operator!=(const utf8_char_view rhs) const {
	return !(*this == rhs);
}
bool utf8_char::operator>(const utf8_char_view rhs) const {
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	if (_csize == 4) { return _data[4] > rhs._ptr[4]; }
	if (_csize == 3) { return _data[3] > rhs._ptr[3]; }
	if (_csize == 2) { return _data[2] > rhs._ptr[2]; }
	if (_csize == 1) { return _data[1] > rhs._ptr[1]; }
	return false;
}
bool utf8_char::operator<(const utf8_char_view rhs) const {
	return !(*this > rhs) && !(*this == rhs);
}
bool utf8_char::operator>=(const utf8_char_view rhs) const {
	return (*this > rhs) || (*this == rhs);
}
bool utf8_char::operator<=(const utf8_char_view rhs) const {
	return (*this < rhs) || (*this == rhs);
}

/* ==========================================
 *               utf8_char_view              
 * ==========================================
 */

utf8_char_view::utf8_char_view() :
	_ptr(nullptr),
	_csize(-1)
{}
utf8_char_view::utf8_char_view(const char* ptr) :
	_ptr(ptr)
{
	_csize = -1;
	if (*ptr < 128) { _csize = 1; }
	if (*ptr >= 0b11110000) { _csize = 4; }
	if (*ptr >= 0b11100000) { _csize = 3; }
	if (*ptr >= 0b11000000) { _csize = 2; }
}
utf8_char_view::~utf8_char_view() {}

utf8_char_view::operator utf8_char() const {
	return utf8_char(_ptr);
}

int utf8_char_view::char_size() const {
	return _csize;
}
const char* utf8_char_view::data() const {
	return _ptr;
}

bool utf8_char_view::operator==(const utf8_char_view rhs) const {
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	return _csize == rhs.char_size() &&
		_ptr[0] == rhs._ptr[0] &&
		_ptr[1] == rhs._ptr[1] &&
		_ptr[2] == rhs._ptr[2] &&
		_ptr[3] == rhs._ptr[3] &&
		_ptr[4] == rhs._ptr[4];
}
bool utf8_char_view::operator!=(const utf8_char_view rhs) const {
	return !(*this == rhs);
}
bool utf8_char_view::operator>(const utf8_char_view rhs) const {
	;
}
bool utf8_char_view::operator<(const utf8_char_view rhs) const {
	return !(*this > rhs) && !(*this == rhs);
}
bool utf8_char_view::operator>=(const utf8_char_view rhs) const {
	return (*this > rhs) || *this == rhs;
}
bool utf8_char_view::operator<=(const utf8_char_view rhs) const {
	return (*this < rhs) || *this == rhs;
}
