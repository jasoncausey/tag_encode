tag_encode
==========

Functions for reversible encoding of integer serial numbers to web- and human-friendly tag strings.

This library was designed as a quick-and-dirty proof of concept for allowing an integer capable of keying a database row (or similar) to be converted to an alphanumeric "tag" reminiscent of the "URL shorteners" that have become widespread.  The idea is that alternating sequences of letters and numbers are somehow easier for most people to enter--probably because the mind separates digits and characters, so they don't "run together" the way long sequences of either tend to do.  Tags of this kind are also web and URL safe, so they are appropriate for hyperlinks or embedding in a website.

The alphabet used for the encoding is the set of lowercase characters [a...z] and the digits [2...9].  The `0` and `1` are avoid on purpose due to the visual similarity with `O` and `l`.  Additionally, the decoding algorithm is case-insensitive, so if the user types the tag in by hand and uses uppercase letters, everything still works.  

In addition to the user-friendly alphabet, the algorithm is designed so that sequences of more than two letters or more than two digits are strictly avoided.  This keeps the visual separation by alternation high, and avoid things like undesirable "words" from being generated. 

Encoding, not Hashing
---------------------

This is an *encoding* scheme, not a *hashing* scheme.  The process produces a 1:1 conversion from integer values to "tags", and vice-versa.  Because the tags are produced using an alphabet larger than the base-10 digits, tags will always consist of the same or fewer characters than their corresponding integers.  For example, the value `2147483646` encodes to `ba9n82dq`.  
