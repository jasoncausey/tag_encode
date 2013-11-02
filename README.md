tag_encode
==========

Functions for reversible encoding of integer serial numbers to web- and human-friendly tag strings.

This library was designed as a quick-and-dirty proof of concept for allowing an integer capable of keying a database row (or similar) to be converted to an alphanumeric "tag" reminiscent of the "URL shorteners" that have become widespread.  The idea is that alternating sequences of letters and numbers are somehow easier for most people to enter--probably because the mind separates digits and characters, so they don't "run together" the way long sequences of either tend to do.  Tags of this kind are also web and URL safe, so they are appropriate for hyperlinks or embedding in a website.

The alphabet used for the encoding is the set of lowercase characters [a...z] and the digits [2...9].  The `0` and `1` are avoided on purpose due to the visual similarity with `O` and `l`.  Additionally, the decoding algorithm is case-insensitive, so if the user types the tag in by hand and uses uppercase letters, everything still works.  

In addition to the user-friendly alphabet, the algorithm is designed so that sequences of more than two letters or more than two digits are strictly avoided.  This keeps the visual separation by alternation high, and avoid things like undesirable "words" from being generated. 

Encoding, not Hashing
---------------------

This is an *encoding* scheme, not a *hashing* scheme.  The process produces a 1:1 conversion from integer values to "tags", and vice-versa.  Because the tags are produced using an alphabet larger than the base-10 digits, tags will always consist of the same or fewer characters than their corresponding integers.  For example, the value `2147483646` encodes to `ba9n82dq`.  


Function Reference
------------------

```cpp
int tag_decode ( std::string tag )        
```
decode an alphanumeric "tag" string into its corresponding integer

Decode a "tag" produced by the `tag_encode()` function into its corresponding non-negative integer. For robustness, this function will accept upper- and lower-case versions of the tag string, and converts often-mistaken characters `'0'` and `'1'` into `'O'` and `'l'` on-the-fly.

**Parameters**

`tag  `  "tag" string as produced by the `tag_encode()` function

**Returns**

non-negative integer serial number corresponding to the input tag 


```cpp
std::string tag_encode ( int serial )     
```
encode a non-negative integer into alphanumeric "tag" string

Encode a non-negative integer serial number into a "shortened URL"-like hash token. The encoding is web-safe, using only the characters a-z2-9. The digits `'0'` and `'1'` are NOT used, since they are easily visually confused with the characters `'O'` and `'l'`. Tags are not case- sensitive, but are always generated with lowercase characters. Additionally, this scheme always guarentees that there is are never more than two consecutive alphabetical characters, thereby avoiding the need to blacklist any "impolite" words. Also, there are never more than two consecutive numbers, avoiding "big number" appearance.

**Parameters**

`serial  `  non-negative integer serial number to convert to alphanumeric "tag"

**Returns**

alphanumeric "tag" string that is both web- and human-friendly 
