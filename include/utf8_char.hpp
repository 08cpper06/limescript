#pragma once


class utf8_char_view;

class utf8_char {
public:
	utf8_char();
	utf8_char(const char* str);
	~utf8_char();

	utf8_char(const utf8_char& rhs);
	utf8_char& operator=(const utf8_char& rhs);
	utf8_char(utf8_char&& rhs) = delete;
	utf8_char& operator=(utf8_char&& rhs) = delete;

	int char_size() const;
	const char* data() const;

	bool operator==(const utf8_char& rhs) const;
	bool operator!=(const utf8_char& rhs) const;
	bool operator>(const utf8_char& rhs) const;
	bool operator<(const utf8_char& rhs) const;
	bool operator>=(const utf8_char& rhs) const;
	bool operator<=(const utf8_char& rhs) const;

	bool operator==(const utf8_char_view rhs) const;
	bool operator!=(const utf8_char_view rhs) const;
	bool operator>(const utf8_char_view rhs) const;
	bool operator<(const utf8_char_view rhs) const;
	bool operator>=(const utf8_char_view rhs) const;
	bool operator<=(const utf8_char_view rhs) const;

private:
	friend class utf8_char_view;
	const char _data[5];
	int _csize;
};

class utf8_char_view {
public:
	utf8_char_view();
	utf8_char_view(const char* ptr);
	~utf8_char_view();

	utf8_char_view(const utf8_char_view&) = default;
	utf8_char_view& operator=(const utf8_char_view&) = default;
	utf8_char_view(utf8_char_view&&) = default;
	utf8_char_view& operator=(utf8_char_view&&) = default;

	operator utf8_char() const;

	int char_size() const;
	const char* data() const;

	bool operator==(const utf8_char_view rhs) const;
	bool operator!=(const utf8_char_view rhs) const;
	bool operator>(const utf8_char_view rhs) const;
	bool operator<(const utf8_char_view rhs) const;
	bool operator>=(const utf8_char_view rhs) const;
	bool operator<=(const utf8_char_view rhs) const;

private:
	friend class utf8_char;
	const char* _ptr;
	int _csize;
};

utf8_char_view next(char** p);