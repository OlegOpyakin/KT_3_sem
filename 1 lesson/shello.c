#include <stdio.h>
#include <unistd.h>
int main() {
   write(1, "Hello, World!", 20);
   return 0;
}
