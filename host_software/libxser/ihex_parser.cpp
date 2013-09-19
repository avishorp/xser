
#include "ihex_parser.h"
#include <exception>
#include <vector>
#include <memory.h>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace ihex_parser_ns;

#define IHEX_ERROR(ln_num, desc) error_list.push_back(ihex_error(ln_num, desc))

// IHEX Record Types
typedef enum {
	IREC_DATA         = 0,  // Data record
    IREC_EOF          = 1,  // End-of-File record
    IREC_EXT_SEG      = 2,  // Extended Segment Address record
    IREC_START_SEG    = 3,  // Start Segment Address record
    IREC_EXT_LINEAR   = 4,  // Extended Linear Address record
    IREC_START_LINEAR = 5   // Start Linear Address record
	} record_type_t;

struct ihex_parser_ns::ihex_record
{
	record_type_t rec_type;
	address_t address;
	const byte* payload;
	const byte* line_data;
	int payload_size;
	int total_size;

	ihex_record(const byte* line_data_, int size)
	{
		line_data = line_data_;
		total_size = size;

		payload_size = line_data[0];
		address = (line_data[1] << 8) + line_data[2];
		rec_type = (record_type_t)line_data[3];
		payload = line_data + 4;
	}

	bool validate(error_list_t& error_list, int line_number) {
		// Check that the payload size is not greater than 32 bytes.
		// This is not strictly a format restriction but it's the
		// usual case
		if (payload_size > 32) {
			IHEX_ERROR(line_number, "Payload size must be 32 bytes or less");
			return false;
		}

		// Check that the payload-size is consistent with the
		// reported size
		if (payload_size != (total_size - 5)) {
			IHEX_ERROR(line_number, "Incorrect payload size");
			return false;
		}

		// Validate the checksum
		byte checksum = 0;
		for(int i = 0; i < total_size-1; i++)
			checksum += line_data[i];
		checksum = (~checksum) + 1;

		if (checksum != line_data[total_size-1]) {
			IHEX_ERROR(line_number, "Checksum error");
			return false;
		}

		// Everything is ok
		return true;

	}
};

ihex_parser::ihex_parser(unsigned long start_addr, unsigned long end_addr, byte default_value)
{
	// Argument check - make sure the end address is > start address
	if (!(end_addr > start_addr))
		throw invalid_argument("end_addr must be greater that start_addr");

	// Allocate a buffer for the data and fill
	// it with the default value
	unsigned long buf_size = end_addr - start_addr + 1;
	buf_data = new byte[buf_size];
	memset(buf_data, default_value, buf_size);

	// Additional initializations
	buf_start_addr = start_addr;
	buf_end_addr = end_addr;
	eof_record_flag = false;
	multiple_eof_record_error = false;
	data_past_eof_error = false;
	extended_address = 0;
}

ihex_parser::~ihex_parser()
{
	delete buf_data;
}

const byte* ihex_parser::get_buffer() const
{
	return (const byte*)buf_data;
}

int ihex_parser::get_error_count() const
{
	return error_list.size();
}

error_const_iter_t ihex_parser::get_error_iter() const
{
	return error_list.begin();
}

void ihex_parser::parse(std::istream& hex_stream)
{
	// Get the input data line-by-line and parse it
	string ln;
	current_line = 1;
	while(getline(hex_stream, ln)) {
		// Convert the string to uppercase
		transform(ln.begin(), ln.end(), ln.begin(), ::toupper);

		// Parse the line
		parse_line(ln);
		current_line++;
		cout << current_line << endl;
	}
}

void ihex_parser::parse_line(const string& line)
{
	
	// Make sure the line is not empty, skip it if so
	if (line.length() < 1)
		return;

	string j("x");
int k = (line.length() & 0x01);
char z = line[0];

	// Make sure the line is well formed
	if ((z == ':') && ((line.length() & 0x01) == 1))
	{
		// Parse the hexadecimal chars into a byte array
		int line_data_size = (line.length()-1)/2;
		byte* line_data = new byte[line_data_size];
		int data_ptr = 0;

		for(unsigned int i = 1; i < line.length(); i += 2) {
			string hex_pair(line, i, 2);
			line_data[data_ptr++] = stoi(hex_pair, 0, 16);
		}

		// Create an IHEX record structure
		ihex_record rec(line_data, line_data_size);

		if (rec.validate(error_list, current_line)) {
			process_record(rec);
		}
	}
	else {
		IHEX_ERROR(current_line, "Malformed line");
		return;
	}
}

void ihex_parser::process_record(const ihex_record& rec)
{
	unsigned long effective_address;

	switch(rec.rec_type) {
	case IREC_DATA:
		// Data record - copy the data into the buffer
		//////////////////////////////////////////////
		effective_address = (extended_address << 16) + rec.address;
		for(int i = 0; i < rec.payload_size; i++) {
			// Copy bytes that are in the buffer range only
			if ((effective_address >= buf_start_addr) && (effective_address <= buf_end_addr)) {
				buf_data[effective_address-buf_start_addr] = rec.payload[i];
				effective_address++;
			}
		}
		break;

	case IREC_EOF:
		// An EOF record must have no data
		if (rec.payload_size != 0) {
			IHEX_ERROR(current_line, "Illegal EOF record - must contain no data");
			return;
		}

		// Make sure it's the only one
		if (eof_record_flag) {
			if (!multiple_eof_record_error) {
				IHEX_ERROR(current_line, "More than one EOF encountered");
				multiple_eof_record_error = true;
			}
			return;
		}

		eof_record_flag = true;
		break;

	case IREC_EXT_LINEAR:
		// The payload size must be 2
		if (rec.payload_size != 2) {
			IHEX_ERROR(current_line, "Data size of an extended linear record must be 2");
			return;
		}

		// Set the extended address
		extended_address = (rec.payload[0] << 8) + rec.payload[1];
		break;

	default:
		IHEX_ERROR(current_line, "Unknown record type, ignoring");
	}
}





