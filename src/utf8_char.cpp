#include "utf8_char.hpp"
#include <cstring>
#include <utility>


/* ==========================================
 *                  utf8_char                
 * ==========================================
 */
utf8_char::utf8_char() :
	_data(),
	_csize(1)
{}

utf8_char::utf8_char(const char* str) :
	_data()
{
	_csize = -1;
	if (*str < 128) { _csize = 1; }
	else if (*str >= 0b11110000) { _csize = 4; }
	else if (*str >= 0b11100000) { _csize = 3; }
	else if (*str >= 0b11000000) { _csize = 2; }

	int index = 0;
	char* p = const_cast<char*>(_data);
	for (;*str && index < _csize; ++index) {
		*p++ = *str++;
	}
	if (index != _csize) {
		_csize = -1;
	}
	for (;index < 5 - _csize; ++index) {
		*p++ = 0;
	}
}

utf8_char::~utf8_char() {}

utf8_char::utf8_char(const utf8_char& rhs) :
	_data(),
	_csize(rhs._csize)
{
	char* p = const_cast<char*>(_data);
	char* pp = const_cast<char*>(rhs._data);
	for (int index = 0; index < 5; ++index) {
		*p++ = *pp++;
	}
}
utf8_char& utf8_char::operator=(const utf8_char& rhs) {
	_csize = rhs._csize;
	char* p = const_cast<char*>(_data);
	char* pp = const_cast<char*>(rhs._data);
	for (int index = 0; index < 5; ++index) {
		*p++ = *pp++;
	}
	return *this;
}

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
	else if (_csize == 3) { return _data[3] > rhs._data[3]; }
	else if (_csize == 2) { return _data[2] > rhs._data[2]; }
	else if (_csize == 1) { return _data[1] > rhs._data[1]; }
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
	else if (_csize == 3) { return _data[3] > rhs._ptr[3]; }
	else if (_csize == 2) { return _data[2] > rhs._ptr[2]; }
	else if (_csize == 1) { return _data[1] > rhs._ptr[1]; }
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
	else if (*ptr >= 0b11110000) { _csize = 4; }
	else if (*ptr >= 0b11100000) { _csize = 3; }
	else if (*ptr >= 0b11000000) { _csize = 2; }
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
	if (_csize != -1 || rhs.char_size()) {
		return false;
	}
	if (_csize == 4) { return _ptr[4] > rhs._ptr[4]; }
	else if (_csize == 3) { return _ptr[3] > rhs._ptr[3]; }
	else if (_csize == 2) { return _ptr[2] > rhs._ptr[2]; }
	else if (_csize == 1) { return _ptr[1] > rhs._ptr[1]; }
	return false;
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



utf8_char_view next(char** p) {
	utf8_char_view c(*p);
	if (c.char_size() > 0) {
		*p += c.char_size();
	}
	return c;
}
std::string operator+(const std::string& lhs, const utf8_char& rhs) {
	std::string str = lhs;
	for (int index = 0; index < rhs.char_size(); ++index) {
		str += rhs.data()[index];
	}
	return lhs;
}
std::string operator+(const utf8_char& lhs, const std::string& rhs) {
	std::string str;
	for (int index = 0; index < lhs.char_size(); ++index) {
		str += lhs.data()[index];
	}
	return str + rhs;
}
