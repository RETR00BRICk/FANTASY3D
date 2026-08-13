#pragma once

namespace StringFunctions {
	inline bool IsCharNumeric(char checking_char){
		return checking_char == '-' || ('0' <= checking_char && checking_char <= '9');
	}
	inline bool IsCharDigit(char checking_char){
		return '0' <= checking_char && checking_char <= '9';
	}
	inline bool CompareStrings(const char* string0, const char* string1, int string_max_size){
		int i = 0;
		while(string0[i] != '\0' && string1[i] != '\0' && i < string_max_size - 1){
			if(string0[i] != string1[i]) return false;
			i ++;
		}
		return string0[i] == string1[i];
	}
	inline void WriteTextToString_CH(char* string, const char* text, int string_max_size){
		int i = 0;
		while (text[i] != '\0' && i < string_max_size - 1) {
			string[i] = text[i];
			i++;
		}
		string[i] = '\0';
	}
	inline void AddTextToString_CH(char* string, const char* text, int string_max_size){
		int string_cursor = 0;
		while(string[string_cursor] != '\0' &&  string_cursor < string_max_size - 1){
			string_cursor++;
		}
		int text_cursor = 0;
		while(text[text_cursor] != '\0' && string_cursor < string_max_size - 1){
			string[string_cursor] = text[text_cursor]; 
			text_cursor ++;
			string_cursor ++;
		}
		string[string_cursor] = '\0';
	}
	//INT
	inline void WriteIntToString_CH(char* string, int integer, int string_max_size){
		bool negative = integer < 0;
		int cursor = 0;
		if(cursor < string_max_size - 1){
			if(negative){
				string[cursor] = '-';
				integer = -integer;
				cursor ++;
			}
		}else return; 
		
		if(integer == 0){
			if(cursor < string_max_size - 1){ 
				string[cursor] = '0';
				string[cursor+1] = '\0';
				return;
			}
		}else{
			int remaining_number = integer;
			char reversed_numbers[16];
			int counter = 0;
			while(remaining_number > 0){
				char digit = '0' + remaining_number%10;
				reversed_numbers[counter] = digit;
				remaining_number /= 10;	
				counter ++;
			}
			int reversed_nums_cursor = counter - 1;
			while(reversed_nums_cursor >= 0 && cursor < string_max_size - 1){
				string[cursor] = reversed_numbers[reversed_nums_cursor];
				cursor ++;
				reversed_nums_cursor --;
			}
			string[cursor] = '\0';
		}
	}
	inline void AddIntToString_CH(char* string, int integer, int string_max_size){
		int string_cursor = 0;
		while(string[string_cursor] != '\0' && string_cursor < string_max_size - 1){
			string_cursor++;
		}

		if(integer == 0){
			if(string_cursor < string_max_size - 1){ 
				string[string_cursor] = '0';
				string[string_cursor+1] = '\0';
				return;
			}
		}else{
			if(string_cursor < string_max_size - 1){
				if(integer < 0){
					string[string_cursor] = '-';
					integer = -integer;
					string_cursor++;
				}
			}else return; 
			
			int digits_count = 0;
			int remaining_number = integer;
			int reversed_numbers[16];
			
			while(remaining_number > 0){
				reversed_numbers[digits_count] = remaining_number%10;
				remaining_number /= 10;
				digits_count ++;
			}
			int reversed_nums_cursor = digits_count - 1;
			while(reversed_nums_cursor >= 0 && string_cursor < string_max_size - 1){
				char digit = '0' + reversed_numbers[reversed_nums_cursor];
				string[string_cursor] = digit;
				reversed_nums_cursor --;
				string_cursor ++;
			}
			string[string_cursor] = '\0';
		}
	}
	//CONVERSION FROM STRING
	inline int GetIntFromString(const char* string, int string_max_size){
		int string_cursor = 0;
		bool negative = false;
		int result = 0;
		if(string_cursor < string_max_size){
			negative = string[string_cursor] == '-';
			if(negative) string_cursor++;
		}else return 0;
		while(string_cursor < string_max_size && string[string_cursor] != '\0'){
			int symbol_code = string[string_cursor];
			if(IsCharDigit(symbol_code)){
				result = result*10 + (symbol_code - '0');
				string_cursor ++;
			}else return 0;
		}
		if(negative) result*=-1;
		return result;
	}
	//FOR FILE PARCER
	inline void TrySkipComment(int& cursor, const char* string, int string_max_size){
		if(string[cursor] == '#'){
			while(cursor < string_max_size && string[cursor] != '\n') cursor ++;	
		}
	}
	inline bool TryReadNumberFromString_CH(int& number, int& cursor, const char* string, int string_max_size){
		if(StringFunctions::IsCharNumeric(string[cursor])){
			int number_length = 0;
			int number_start = cursor;
			cursor += string[cursor] == '-';
			number_length += string[number_start] == '-';
			while(cursor < string_max_size && StringFunctions::IsCharDigit(string[cursor])){
				number_length ++;
				cursor ++;
			}
			number = StringFunctions::GetIntFromString(&string[number_start], number_length);
			return true;
		}else{
			return false;
		}
	}
	inline bool TryLoadIntArrayFromString_CH(int* num_array, int num_array_size, int& cursor, const char* string, int string_max_size){
         int read_numbers_count = 0;
         while(cursor < string_max_size){
            TrySkipComment(cursor, string, string_max_size);
            if(cursor == string_max_size) return false;
            int value;
            if(TryReadNumberFromString_CH(value, cursor, string, string_max_size)){
                num_array[read_numbers_count] = value;
                read_numbers_count++;
                if(read_numbers_count == num_array_size){
                    return true;
                }
            }else{
                cursor++;
            }
         }
         return false;
    } 
}
