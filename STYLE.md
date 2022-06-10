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