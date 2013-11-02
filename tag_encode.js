/**
 * @file tag_encode.js
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

N_ALPHACASE = 26;
N_ALPHANUM  = 34;
N_DIGITS    = 8;

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
 *
 * @remark  These tags are shorter than the input numbers 
 *          (MAX_INT of 2147483646  encodes as ba9n82dq -- only 8 characters!)
 * 
 * @param  serial non-negative integer serial number to convert to alphanumeric "tag"
 * @return        alphanumeric "tag" string that is both web- and human-friendly
 */
function tag_encode(serial){
    if(serial < 0){
        throw "Serial number must be non-negative.";
    }
    var    tag      = "";
    var    position = 1;
    var    digit
    var    digit_base;
    var    digit_in_ascii;
    var    two_code  = "2".charCodeAt(0);
    var    nine_code = "9".charCodeAt(0);
    var    a_code    = "a".charCodeAt(0);

    do{
        digit_base = (position++ % 3) ? ((position % 3 == 0) ? N_ALPHACASE : N_ALPHANUM) : N_DIGITS;
        digit      = serial % digit_base;
        serial     = Math.floor(serial / digit_base);
        digit_in_ascii = (digit_base != N_ALPHACASE) ? two_code + digit : a_code + digit;   // Base digit is '2' because both '0' and '1' are 
        if(digit_base != N_ALPHACASE && digit_in_ascii > nine_code){                        // ambiguous in comparison to 'O' and 'l'.
            digit_in_ascii = a_code + (digit - N_DIGITS);
        }
        tag += String.fromCharCode(digit_in_ascii);
    }while(serial > 0);
                                                                                            // Must reverse since the generated string is big-endian and
    tag = tag.split("").reverse().join("");                                                 // we want little-endian.  naive reverse is ok since we are 
    return tag                                                                              // really just ASCII    
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
 *          (MAX_INT of 2147483646 encodes as ba9n82dq -- only 8 characters!)
 *          
 * @throw  "Tag cannot be blank."   thrown if the tag is blank 
 * @throw  "Invalid input tag."     thrown if re-encoding the tag does not 
 *                                  produce the same tag
 * 
 * @param  tag_str  "tag" string as produced by the `tag_encode` function
 * @return          non-negative integer serial number corresponding to the input tag
 */
function tag_decode(tag_str){
    if(tag_str.length < 1){
        throw "Tag cannot be blank.";
    }
    var    digit;
    var    digit_base;
    var    digit_in_ascii;
    var    serial   = 0;
    var    position = 1;
    var    tag_size = tag_str.length;
    var    mult     = 1;
    var    two_code  = "2".charCodeAt(0);
    var    nine_code = "9".charCodeAt(0);
    var    a_code    = "a".charCodeAt(0);
                                                                                    // "user-proof" pre-process:
    tag_str = tag_str.replace(/0/g, "O");                                           // replace all 0's with o's
    tag_str = tag_str.replace(/1/g, "l");                                           // and all 1's with l's 
    tag_str = tag_str.toLowerCase();                                                // and lowercase the string

    do{
        digit_in_ascii = tag_str.charCodeAt(tag_size - position);
        digit_base     = (position++ % 3) ? ((position % 3 == 0) ? N_ALPHACASE : N_ALPHANUM) : N_DIGITS;
        if(digit_base == N_ALPHACASE || digit_in_ascii > nine_code){
            digit = digit_in_ascii - a_code + ( (digit_base != N_ALPHACASE) ? N_DIGITS : 0 );
        }
        else{
            digit = digit_in_ascii - two_code;
        }
        serial += digit * mult;
        mult   *= digit_base;
    }while(position <= tag_size);
                                                                                    // if the string is re-encoded
    if(tag_encode(serial) != tag_str){                                              // but doesn't match the original,
        throw "Invalid input tag.";                                                 // that is a problem...
    }                                                                                
                                                                                    
    return serial;
}