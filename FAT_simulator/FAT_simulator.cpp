

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <math.h>
#include <stdlib.h>
using namespace std;

//sizeOfDisk = 1024;
//sizeOfBlock = 32;

struct Block
{
	int sizeOfBlock;
	string content;
};

struct Directory 
{
	string fileName; 
	int size;  
	int startingBlock; 
	int lastBlock; 
};

struct File 
{
	string name;
	string content;
};

vector<Directory> vectorOfDirectories;

array<int, 32> fatTable = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; //tablica FAT

array<bool, 32> tableOfFreeSpace = {0}; 

array<Block,32> database; 

void lookForSpaceForOneBlock(Directory &directory)
{
	int counter = 0;
	for (auto i : tableOfFreeSpace) 
	{
		if (i == 0)
		{
			directory.startingBlock = counter;
			tableOfFreeSpace[counter] = 1;
			break;
		}
		counter++;
	}
	
}

string take_frist32_chars(File &file)
{
	string temp;
	for (int i = 0; i < 32; i++)
	{
		temp += file.content[i];
	}

	file.content.erase(0, 31);

	return temp;
}

string take_rest_chars(File &file)
{
	string temp;
	for (int i = 0; i < file.content.length(); i++)
	{
		temp += file.content[i];
	}

	return temp;
}


void saveToDatabase(File &file)
{
	Directory directory;

	if (file.content.length() == 0) { cout << "Plik txt jest pusty"; }
		
	if (file.content.size() <= 32)  
	{
		Block block;
		
		lookForSpaceForOneBlock(directory);

		directory.fileName = file.name;
		directory.size = file.content.size();
		directory.lastBlock = directory.startingBlock;

		block.sizeOfBlock = file.content.size();
		block.content = file.content;

		database[directory.startingBlock] = block; 

		vectorOfDirectories.push_back(directory);
		
	}
	else 
	{
		int size = file.content.length(); 
		int clusterNumber;
		int temp;

		directory.fileName = file.name; 
		directory.size = size; 

		float neededBlocks = file.content.length() / 32.0;
		neededBlocks = ceil(neededBlocks); 

		for (int i = 0; i < neededBlocks; i++) 
		{
			if (i == 0)
			{
				Block block;
				lookForSpaceForOneBlock(directory); 
				temp = directory.startingBlock; 

				block.sizeOfBlock = 32; 
				block.content = take_frist32_chars(file);
				
				database[temp] = block; 
			}

			if (i == neededBlocks -1 ) 
			{
				Block block;
				int counter = 0;
				
				for (auto i : tableOfFreeSpace)
				{
					if (i == 0)
					{
						fatTable[temp] = counter;
						tableOfFreeSpace[counter] = 1;
						directory.lastBlock = counter; 
						break;
					}
					counter++;
				}
				
				block.content = take_rest_chars(file);
				block.sizeOfBlock = file.content.length(); 
				file.content.erase(0, file.content.length());
				temp++;
				database[temp] = block;
				break; 
			}

			if(i != 0)
			{
				Block block;
				int counter = 0;
				for (auto i : tableOfFreeSpace) 
				{
					if (i == 0)
					{
						fatTable[temp] = counter; 
						tableOfFreeSpace[counter] = 1;
						temp = counter;
						break;
					}
					counter++;
				}
				block.sizeOfBlock = 32;
				block.content = take_frist32_chars(file);
			
				database[temp] = block;
			}

		}
		vectorOfDirectories.push_back(directory);
	
	}

}

void showInfo()
{
	cout << "Lista Directories: " << endl;
	for (auto i : vectorOfDirectories) 
	{ 
		if (i.size != 0) 
		{
			cout << "Nazwa Pliku: " << i.fileName <<", ";
			cout << "Rozmiar Pliku: " << i.size << ", ";
			cout << "Blok Startowy: " << i.startingBlock << ", ";
			cout << "Blok koncowy: " << i.lastBlock;
			cout << endl;
		}
	}

	cout << endl;

	cout << "Tablica wolnego miejsca: ";
	for (auto i : tableOfFreeSpace) { cout << "[" << i << "]" <<" "; }

	cout << endl;

	cout << "Tablica FAT:             ";
	for (auto i : fatTable) { cout << "[" << i << "]" << " "; }

}

void showDatabase()
{
	for (int i = 0; i < 32; i++) 
	{
		cout << i << ". " << database[i].content << endl;
	}
}

void deleteFile(string &nazwaPliku)
{
	int startingBlock;
	int endingBlock;
	int counter = 0;
	for (auto i : vectorOfDirectories) {
		if (nazwaPliku == i.fileName) 
		{
				startingBlock = i.startingBlock;
				endingBlock = i.lastBlock;
				vectorOfDirectories.erase(vectorOfDirectories.begin()+counter);
		}
		counter++;
	}

	int temp = startingBlock;
	if (temp == endingBlock) { 
		fatTable[temp] = -1;
		tableOfFreeSpace[temp] = 0;
		database[temp].content = "";
		database[temp].sizeOfBlock = 0;
			
	}
	else { 
		while (fatTable[temp] != -1) {
			int temp2 = fatTable[temp];
			fatTable[temp] = -1;
			tableOfFreeSpace[temp] = 0;
			database[temp].content = "";
			database[temp].sizeOfBlock = 0;

			temp = temp2;
		}
		//usuniecie ostatniego blocku
		tableOfFreeSpace[endingBlock] = 0;
		database[endingBlock].content = "";
		database[endingBlock].sizeOfBlock = 0;
	}
}

int main()
{
	int x;

	do {
		system("cls");
		cout << "[1.Dodaj Plik, 2.Pokaz directories, 3.Pokaz wybrany block, 4.Pokaz cala baze danych, 5.Usun Plik, 6.Wyjdz]";
		cout << endl;
		cout << "Polecenie nr: ";
		cin >> x;
		switch(x){
			case 1: { 
				File file;
				cout << "Nazwa pliku: ";
				cin >> file.name;
				cout << "Zawartosc: ";
				cin >> file.content;
				saveToDatabase(file);
				cout << endl;
				system("pause");

				break;
			}
			case 2: { 
				showInfo();
				cout << endl;
				system("pause");

				break;
			}
			case 3: { 
				int wybor;
				cout << "Wybierz blok do wyswietlenia: ";
				cin >> wybor;
				if (wybor < 0 || wybor > 31) {
					cout << "W bazie znajduja sie 32 bloki";
					break;
				}
				cout << database[wybor].content << endl;
				system("pause");
				break;
			}
			case 4: { 
				showDatabase();
				system("pause");
				break;
			}
			case 5: { 
				string plik;
				cout << "plik do usuniecia: ";
				cin >> plik;
				deleteFile(plik);
				break;
			}	
		}
	} while (x != 6);

	system("pause");

	return 0;
}
