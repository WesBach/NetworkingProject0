#include "Buffer.h"
#include <string>
#include <iostream>
#include <conio.h>

//TO DO: Client side connection
int main() {


	//Taking in user's input and quiting when detecting the 'q' (infinite loop)
	while (true)
	{
		if (_kbhit()) 
		{
			char ch = _getch();
			std::cout << ch;
			if (ch == 'q')
			{
				std::cout << "QUIT" << std::endl;
				break;
			}
		}
	}
}