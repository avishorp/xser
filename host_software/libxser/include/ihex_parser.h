#ifndef __IHEX_PARSER_H__
#define __IHEX_PARSER_H__

#include "export.h"
#include <istream>
#include <list>
#include <string>

namespace ihex_parser_ns {

class EXPORT ihex_error {
public:
	ihex_error() : description() { line_number = -1; }
	ihex_error(const ihex_error& e) : description(e.description) {
		line_number = e.line_number;
	}
	ihex_error(int ln, const char* desc) : description(desc) {
		line_number = ln;
	}
	const std::string& get_description() { return description; }
	int get_line_number() { return line_number; }

private:
	int line_number;
	std::string description;
};

typedef EXPORT ihex_error error_t;
typedef EXPORT std::list<error_t> error_list_t;
typedef EXPORT error_list_t::const_iterator error_const_iter_t;
typedef EXPORT unsigned __int8 byte;
typedef EXPORT unsigned long address_t;

// Internal data type
struct ihex_record;

class EXPORT ihex_parser
{
public:
	ihex_parser(unsigned long start_addr, unsigned long end_addr, byte default_value = 0xff);
	~ihex_parser();
	void parse(std::istream& hex_stream);

	const byte* get_buffer() const;

	int get_error_count() const;
	error_const_iter_t get_error_iter() const;

private:
	void parse_line(const std::string& line);
	void process_record(const ihex_record& line_data);
	bool hex_to_dec(const char* hex, byte& dec);

	unsigned long buf_start_addr;
	unsigned long buf_end_addr;
	byte* buf_data;
	error_list_t error_list;
	bool eof_record_flag;
	bool multiple_eof_record_error;
	bool data_past_eof_error;
	unsigned long extended_address;
	int current_line;
};

};



#endif