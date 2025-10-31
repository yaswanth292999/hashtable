A low-level implementation of a generic hash table in C using open addressing with linear probing, supporting dynamic resizing, custom hash functions, and key/value cleanup callbacks.
Designed to illustrate how modern hash maps (like those in Python, Go, or Java) manage collisions and memory at a system level.

Features:
- Generic key/value storage using void *
- User-defined hash and equals functions (e.g., for strings, integers, structs)
- Custom destructors for automatic memory cleanup
- Automatic resizing based on configurable load factors
- Open addressing with linear probing for fast collision resolution
