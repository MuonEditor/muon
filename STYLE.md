# Code Style

## Naming Convention

- Files: `snake_case.h` `snake_case.cpp`
- Namespaces: `CamelCase`
- Classes: `CamelCase`
- Structs: `CamelCase_t`
- Members: `camelCase`
- Enum: `SNAKE_CASE`
- Macros: `SNAKE_CASE`

## Brace Style

```c++
int myFunction() {
    if (shortIf) { shortFunctionName(); }

    if (longIf) {
        longFunction();
        otherStuff();
        myLongFunction();
    }
}
```

Note: If it makes the code cleaner, remember to use the `?` keyword instead of a `if else` statement.

## Pointers

Please use `type* name` for pointers.

## Structure

Headers and their associated C++ files shall be in the same directory. All headers must use `#pragma once` instead of other include guards. Only include files in a header that are being used in that header. Include the rest in the associated C++ file.

# JSON Formatting

The ability to add new radio band allocation identifiers and color maps relies on JSON files. Proper formatting of these JSOn files is important for reference and readability. The following guides will show you how to properly format the JSON files for their respective uses.

**IMPORTANT: JSON File cannot contain comments, there are only in this example for clarity**

## Band Frequency Allocation 

Please follow this guide to properly format the JSON files for custom radio band allocation identifiers.

```json
{
    "name": "Short name (has to fit in the menu)",
    "country_name": "Name of country or area, if applicable (Use '--' otherwise)",
    "country_code": "Two letter country code, if applicable (Use '--' otherwise)",
    "author_name": "Name of the original/main creator of the JSON file",
    "author_url": "URL the author wishes to be associated with the file (personal website, GitHub, Twitter, etc)",
    "bands": [ 
        // Bands in this array must be sorted by their starting frequency
        {
            "name": "Name of the band",
            "type": "Type name ('amateur', 'broadcast', 'marine', 'military', or any type decalre in config.json)",
            "start": 148500, //In Hz, must be an integer
            "end": 283500 //In Hz, must be an integer
        },
        {
            "name": "Name of the band",
            "type": "Type name ('amateur', 'broadcast', 'marine', 'military', or any type decalre in config.json)",
            "start": 526500, //In Hz, must be an integer
            "end": 1606500 //In Hz, must be an integer
        }    
    ]
}
```