#include <iostream>
#include <cstring>

int main(int argc, char * argv[]){
	if (argc == 1) return 0;

	int i = 1, j = 1;
	if (!strcmp(argv[1], "-n")) {i = 2; j =2; }

	for (int p = i; p<= argc-1; p++){
		std::cout << argv[p];
		if (p != argc-1) std::cout << " ";
	}
	
	if (j == 1) std::cout << "\n";
	return 0;
}
