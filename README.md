# maddy

forked from [maddy](https://github.com/progsource/maddy)

# Changes:

* cmake min version 3.10
* use huntergate for dependencies (google test)
* C++ 17 upgrade
* changes to cmake in order to see the all files in qtcreator
* convert shared_ptr
  * to unique_ptr when polymorphic behaviour needed (for block parsers)
  * to stack object (for line parsers) 
* type aliases (defined in blockparser.h)
