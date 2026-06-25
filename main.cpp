#include <iostream>
#include <cstdlib>
#include "vigenere.h"
#include "atbash.h"
#include "xor_cipher.h"
#include <clocale>  
#include <locale>   
using namespace std;

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    while (true) {
        system("clear"); // или "cls" для Windows
        
        cout << "" << endl;
        cout << "   КРИПТОГРАФИЧЕСКИЙ ИНСТРУМЕНТ" << endl;
        cout << "" << endl;
        cout << "1. Шифр Виженера" << endl;
        cout << "2. Шифр Атбаш" << endl;
        cout << "3. Шифр XOR" << endl;
        cout << "0. Выход" << endl;
        cout << "" << endl;
        cout << "Выберите шифр: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1:
                vigenereMain();  // Вся логика Виженера
                break;
            case 2:
                atbashMain();    // Вся логика Атбаш
                break;
            case 3:
                xorMain();       // Вся логика XOR
                break;
            case 0:
                cout << "До свидания!" << endl;
                return 0;
            default:
                cout << "Неверный выбор!" << endl;
                cout << "Нажмите Enter для продолжения...";
                cin.get();
        }
    }
    
    return 0;
}