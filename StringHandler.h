#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <exception>
#include <cctype>


class StringHandler {

public:
	static std::vector<std::string> spilt(std::string &source, char delimiter) {
	
		std::vector<std::string> output;
		std::stringstream ss(source);
		std::string temp;

		while (std::getline(ss, temp, delimiter)) {
		
			output.push_back(temp);
		}

		return output;
	
	}

	static std::string replace(std::string source, const std::string &find, const std::string &replace) {
	
		size_t pos = 0;
		while ((pos = source.find(find, pos)) != std::string::npos)
		{
			source.replace(pos, find.length(), replace);
			pos += replace.length();
		}
		return source;
	}

	static bool IsNumeric(std::string source) {
	
		
		if (atoi(source.c_str()) != 0 || std::str.compare("0") == 0 )){
			
			return true;
		}

		else {
			
			return false;
		}
	
	
	}

};

