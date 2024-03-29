#ifndef K3KEY_H
#define K3KEY_H
#include <stdio.h>
#include <vector>

class K3Key {
private:
     int size_key;
     std::vector<bool> bector;
     bool out_of_range(int) const;
     void info(float, const char*) const;

public:
     K3Key(int size) : bector(size, false) { this->size_key = size; } /*!< public constructor */
     bool* is(int); /*!< get a reference to a key */
     bool status(int) const; /*!< get the key value */

     void hide(void); /*!< desactivate all keys */
     void show(int, bool); /*!< activate a key */
     void flip(int); /*!< flip the key */
};

#endif
