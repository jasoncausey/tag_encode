/**
 * @file test_tag_encoding.cpp
 *
 * Provides a simple test run to demonstrate how the
 * `tag_encode()` and `tag_decode()` functions work.
 * See the file "tag_encode.h" for definitions.
 * 
 *
 * @copyright (c) 2013 Jason L Causey,
 * Distributed under the MIT License (MIT):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include<iostream>
#include<string>
#include<limits>
#include<ctime>

#include "tag_encode.h"

int main(int argc, const char* argv[]){
	std::string s;
	long int    ds;
	bool        ok         = true;
	time_t      start_time = time(0);

	std::cout << "First 200 values, or until first mis-matched value, if any: \n";
	for(long int i = 0; ok && (time(0) < start_time + 90) && i < std::numeric_limits<long int>::max() / 1000; i++){
		s  = tag_encode(i);  // encode the counter
		ds = tag_decode(s);  // and then decode the encoded tag
		if(i < 200){		 // print the first 200
			std::cout << i << "\t" << s << "\t" << ds << std::endl;
			std::cout << (i == 199 ? "Testing wide range of values; this could take up to 90 seconds... Please be patient...\n" : "");
		}
		if(i != ds){         // always test for a mismatch
			for(int j = i - 5; j <= i; j++){
				std::cout << j << "\t" << tag_encode(j) << "\t" << tag_decode(tag_encode(j)) << std::endl;
			}
			ok = false;
		}		
	}
	if(ok){                  // if all went well, say so
		std::cout << "Encode/Decode test passed OK!\n";
	}
	
	std::cout << "\n";
	std::cout << "Testing last ten possible 'long int' values:\n";
	
	long int i = std::numeric_limits<long int>::max() - 10L;
	
	for(long int j = 0; j < 10; j++){
		try{
			s  = tag_encode(i+j);
			ds = tag_decode(s);
			std::cout << (i+j) << "\t" << s << "\t" << ds << std::endl;
		}catch(std::exception& e){
			std::cout << "Failed on " << (i+j) << "\twith error: " << e.what() << std::endl;
		}
	}

	std::cout << "\n";
	std::cout << "Testing conversion of case and mis-used '0' and '1' digits: " << std::endl;
	std::cout << "Converting 30A: " << tag_decode("30a") << "\t" << tag_decode("3oa") << std::endl;
	std::cout << "Converting 31A: " << tag_decode("31a") << "\t" << tag_decode("3la") << std::endl;
	
	return 0;
}
