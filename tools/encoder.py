#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys

HEADER = """#ifndef SHELLCODE_H
#define SHELLCODE_H

const char *SHELLCODE[] = {{{}}};

#endif // SHELLCODE_H
"""

# fmt: off
# Look up table for converting bytes to English words
LOOKUP_TABLE = [
    "the", "of", "and", "to", "in", "for", "is", "on", "that", "by", "this",
    "with", "you", "it", "not", "or", "be", "are", "from", "at", "as",
    "your", "all", "have", "new", "more", "an", "was", "we", "will", "home",
    "can", "us", "about", "if", "page", "my", "has", "search", "free",
    "but", "our", "one", "other", "do", "no", "information", "time", "they",
    "site", "he", "up", "may", "what", "which", "their", "news", "out",
    "use", "any", "there", "see", "only", "so", "his", "when", "contact",
    "here", "business", "who", "web", "also", "now", "help", "get", "pm",
    "view", "online", "first", "am", "been", "would", "how", "were", "me",
    "services", "some", "these", "click", "its", "like", "service", "than",
    "find", "price", "date", "back", "top", "people", "had", "list", "name",
    "just", "over", "state", "year", "day", "into", "email", "two",
    "health", "world", "re", "next", "used", "go", "work", "last", "most",
    "products", "music", "buy", "data", "make", "them", "should", "product",
    "system", "post", "her", "city", "add", "policy", "number", "such",
    "please", "available", "copyright", "support", "message", "after",
    "best", "software", "then", "jan", "good", "video", "well", "where",
    "info", "rights", "public", "books", "high", "school", "through",
    "each", "links", "she", "review", "years", "order", "very", "privacy",
    "book", "items", "company", "read", "group", "sex", "need", "many",
    "user", "said", "de", "does", "set", "under", "general", "research",
    "university", "january", "mail", "full", "map", "reviews", "program",
    "life", "know", "games", "way", "days", "management", "part", "could",
    "great", "united", "hotel", "real", "item", "international", "center",
    "ebay", "must", "store", "travel", "comments", "made", "development",
    "report", "off", "member", "details", "line", "terms", "before",
    "hotels", "did", "send", "right", "type", "because", "local", "those",
    "using", "results", "office", "education", "national", "car", "design",
    "take", "posted", "internet", "address", "community", "within",
    "states", "area", "want", "phone", "dvd", "shipping", "reserved",
    "subject", "between", "forum", "family", "long", "based", "code",
    "show", "even", "black", "check", "special"
]
# fmt: on

# Make sure the lookup table has 256 words
assert len(LOOKUP_TABLE) == 256
assert len(sys.argv) == 3, "Usage: python encoder.py <payload.bin> <shellcode.h>"

# Read the payload bytes from the file
with open(sys.argv[1], "rb") as payload_file:
    payload = payload_file.read()

# Convert the payload bytes to English words and print them
words = []
for index, byte in enumerate(payload):
    words.append(f'"{LOOKUP_TABLE[byte]}"')

with open(sys.argv[2], "w", encoding="utf-8") as payload_file:
    payload_file.write(HEADER.format(", ".join(words)))
