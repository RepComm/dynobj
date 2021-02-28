# dynobj

A dynamic object library written in C

## Dependencies
- stdlib.h [malloc]
- [jwerle/murmurhash](https://github.com/jwerle/murmurhash.c) [soft dependency]

---

Eventually, all dependencies will be soft dependencies (ones that can be swapped at will)

---

HashedKeyStore now is completely free from hash implementation, and only requires
passing a callback that takes `char *` and returns `int`

Implementation that uses murmurhash is in dynobj.c

## What
Heap allocated objects/properties for C

Objects are fancy linked-lists of key-value pairs

Key value pairs here are actually fixed size (hash, void-pointer) structs

`object->get` is more expensive than `object->getByHash`

as `get` has to reverse lookup the hash from the key

## Performance
I have not run tests yet, but this should be pretty darn fast
Nothing is as fast as static and fixed data structures, though

## Caching and optimisations
A frequent requirement is to have the count of properties of an object<br/>
This value is cached and marked dirty when the object->set method is called

Object keys are stored as hashes (32bit atm) to save space<br/>
and to make object memory more predictable/safe

If two objects both have a key `"some really really really really long string"` there will only be one copy of that string stored in the HashedKeyTable globally.<br/>

This can save a lot of space with lots of similar objects.

---
A future optimisation will be sorting HashedKeyStore linked list for binary search during reverse lookup

## Why
I'm new to C, and like a good challenge

Dynamic objects are great for rapid prototyping

## State
Functional, needs testing before production ready

## API

Standalone functions
- DynObj_create
- DynObj_set
- DynObj_get
- DynObj_getByHash

Object instance methods
- obj->set (same as DynObj_set)
- obj->get (same as DynObj_get)
- obj->getByHash (same as DynObj_getByHash)

### Note:

C doesn't have a concept of `this`,<br/>
so object methods must be passed a reference to itself
in the first parameter.<br/>

I know this can feel weird for non C programmers, but its not a real hinderence as you need a reference to the object in question anyways.
