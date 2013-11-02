/**
 * @file tag_encode.h
 *
 * Defines the functions `tag_encode()` and `tag_decode()`, which are designed
 * to convert from integers (such as those used as database serial numbers)
 * to human- and web-friendly alphanumeric strings.  The encoding produces 
 * short strings (shorter than the string versions of the base-10 integers)
 * that are easy to email, write down, place in a URL, or any other common
 * operation on the web.  
 * 
 * For robustness against humans, the tags are not case-sensitive, although 
 * they will always be generated with lowercase characters.  The numeric 
 * digits '1' and '0' are also unused in the encoding because of the 
 * ambiguity with 'l' and 'O' -- although `tag_decode()` will convert these
 * digits into the corresponding alphabetical characters on-the-fly.
 * Additionally, the tag encoder will never produce more than two consecutive
 * alphabetical characters, thus avoiding any problems with certain "words"
 * being generated.  Strings of more than two consecutive digits are also
 * avoided, avoiding "big number" tags.
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
#ifndef TAG_ENCODE_H
#define TAG_ENCODE_H

#include<string>
#include<sstream>
#include<algorithm>
#include<exception>
#include<cctype>

const int N_ALPHACASE   = 26;
const int N_ALPHANUM    = 34;
const int N_DIGITS      = 8;
const int BASE_SELECT[] = {N_ALPHANUM, N_ALPHACASE, N_DIGITS};

/**
 * @brief  encode a non-negative integer into alphanumeric "tag" string
 * 
 * Encode a non-negative integer serial number into a "shortened URL"-like
 * hash token.  The encoding is web-safe, using only the characters
 * a-z2-9.  The digits '0' and '1' are NOT used, since they are easily
 * visually confused with the characters 'O' and 'l'.  Tags are not case-
 * sensitive, but are always generated with lowercase characters.
 * Additionally, this scheme always guarentees that there is are never more
 * than two consecutive alphabetical characters, thereby avoiding the need
 * to blacklist any "impolite" words.  Also, there are never more than two
 * consecutive numbers, avoiding "big number" appearance.
 * Limits:  Minimum serial number: 0
 *          Maximum is the system-dependant upper limit for type 'long int'
 *
 * @remark  These tags are shorter than the input numbers 
 *          (MAX_INT  of          2147483646 encodes as ba9n82dq        -- 8 characters!)
 *          (Max long of 9223372036854775806 encodes as 6eh5g28yq5mi7bq -- 15 characters!)
 * 
 * @param  serial non-negative integer serial number to convert to alphanumeric "tag"
 * @return        alphanumeric "tag" string that is both web- and human-friendly
 */
std::string tag_encode(long int serial){
    if(serial < 0){
        throw std::out_of_range("Serial number must be non-negative.");
    }
    std::ostringstream tag;
    char   digit_in_ascii;
    int    digit, digit_base;
    int    position = 0;
    do{
        digit_base = BASE_SELECT[position++ % 3];
        digit      = serial % digit_base;
        serial    /= digit_base;
        digit_in_ascii = (digit_base != N_ALPHACASE) ? '2' + digit : 'a' + digit;   // Base digit is '2' because both '0' and '1' are 
        if(digit_base != N_ALPHACASE && digit_in_ascii > '9'){                      // ambiguous in comparison to 'O' and 'l'.
            digit_in_ascii = 'a' + (digit - N_DIGITS);
        }
        tag << digit_in_ascii;
    }while(serial > 0);
    std::string tag_str = tag.str();                                                // order is big-endian now...
    std::reverse(tag_str.begin(), tag_str.end());                                   // so convert to little-endian
    return tag_str;
}

/**
 * @brief  decode an alphanumeric "tag" string into its corresponding integer
 * 
 * Decode a "tag" produced by the `tag_encode` function into its corresponding
 * non-negative integer.  For robustness, this function will accept upper- and
 * lower-case versions of the tag string, and converts often-mistaken characters
 * '0' and '1' into 'O' and 'l' on-the-fly.
 *
 * @remark  The tag encoding is web-safe, using only the characters
 *          a-z2-9.  The digits '0' and '1' are NOT used, since they are easily
 *          visually confused with the characters 'O' and 'l'.  Tags are not case-
 *          sensitive, but are always generated with lowercase characters.
 *          Additionally, this scheme always guarentees that there is are never more
 *          than two consecutive alphabetical characters, thereby avoiding the need
 *          to blacklist any "impolite" words.  Also, there are never more than two
 *          consecutive numbers, avoiding "big number" appearance.
 *          These tags are shorter than the serial numbers produced:
 *          (MAX_INT  of          2147483646 encodes as ba9n82dq        -- 8 characters!)
 *          (Max long of 9223372036854775806 encodes as 6eh5g28yq5mi7bq -- 15 characters!)
 *
 * @throw  std::invalid_argument    thrown if the tag is blank or if re-encoding the
 *                                  tag does not produce the same tag
 * 
 * @param  tag "tag" string as produced by the `tag_encode` function
 * @return     non-negative integer serial number corresponding to the input tag
 */
long int tag_decode(std::string tag){
    if(tag.size() < 1){
        throw std::invalid_argument("Tag cannot be blank.");
    }
    int      digit, digit_base;
    char     digit_in_ascii;
    long int serial   = 0;
    int      position = 0;
    int      tag_size = tag.size();
    long int mult     = 1;
    bool     check    = true;
                                                                                    // "user-proof" pre-process:
    for(size_t pos = tag.find("0"); pos != std::string::npos; pos = tag.find("0", pos+1)){
        tag.replace(pos, 1, "O");                                                   // replace all 0's with o's
    }
    for(size_t pos = tag.find("1"); pos != std::string::npos; pos = tag.find("1", pos+1)){
        tag.replace(pos, 1, "l");                                                   // and all 1's with l's 
    }

    do{
        digit_base     = BASE_SELECT[position++ % 3];
        digit_in_ascii = tag[tag_size - position] = tolower(tag[tag_size - position]);
        if(digit_base == N_ALPHACASE || digit_in_ascii > '9'){
            digit = digit_in_ascii - 'a' + ( (digit_base != N_ALPHACASE) ? N_DIGITS : 0 );
        }
        else{
            digit = digit_in_ascii - '2';
        }
        serial += digit * mult;
        mult   *= digit_base;
    }while(position < tag_size);

    try{                                                                            // if the string is re-encoded
        if(tag_encode(serial) != tag){                                              // but doesn't match the original,
            check = false;                                                          // that is a problem...
        }
    }catch(std::out_of_range) { check = false; }                                    // if we generate a negative somehow, that's bad too
    if(!check){                                                                     // Any kind of mismatch creates an exception
        throw std::invalid_argument(
            std::string("Invalid input tag: \"") + tag + "\""
        );      
    }
                                                                                    
    return serial;                                                                  // return only if all is well
}

#endif
