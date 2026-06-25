#include "xor_cipher.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <clocale>  
#include <locale>   
using namespace std;

//ГЕНЕРАЦИЯ КЛЮЧА
static string generateKey(int length) {
    string key = "";
    
    srand(time(0) + rand());
    for (int i = 0; i < length; i++) {
        // ИСПРАВЛЕНО: теперь от 1 до 255, без нулевого байта!
        key += char((rand() % 255) + 1);
    }
    return key;
}

//XOR ОБРАБОТКА
string xorProcess(const string& data, const string& key) {
    if (key.empty()) {
        cerr << "Ошибка: ключ не может быть пустым!" << endl;
        return "";
    }
    
    string result = "";
    for (int i = 0; i < data.length(); i++) {
        char keyChar = key[i % key.length()];
        result += data[i] ^ keyChar;
    }
    return result;
}

//РАБОТА С ФАЙЛАМИ
static string readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return "";
    }
    
    string content;
    char ch;
    while (file.get(ch)) {
        content += ch;
    }
    file.close();
    return content;
}

static bool writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось создать файл " << filename << endl;
        return false;
    }
    
    file.write(content.c_str(), content.size());
    file.close();
    return true;
}

//ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
static void printResult(const string& result) {
    cout << "\n--- РЕЗУЛЬТАТ ---" << endl;
    cout << "" << endl;
    
    // Проверяем, можно ли вывести как текст
    bool isText = true;
    for (char c : result) {
        if (c != '\n' && c != '\r' && c != '\t' && (c < 32 || c > 126)) {
            if (c < 0) continue; // Русские буквы
            isText = false;
            break;
        }
    }
    
    if (!isText) {
        cout << "[Бинарные данные, размер: " << result.length() << " байт]" << endl;
        return;
    }
    
    if (result.length() <= 500) {
        cout << result << endl;
    } else {
        cout << result.substr(0, 500) << "..." << endl;
        cout << "(Показано первых 500 символов из " << result.length() << ")" << endl;
    }
}

static void waitForEnter() {
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

//ГЛАВНАЯ ФУНКЦИЯ XOR
void xorMain() {

    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    system("clear"); // или "cls"
    
    cout << "" << endl;
    cout << "ШИФР XOR" << endl;
    
    
    // Выбор источника
    int source;
    cout << "1. Ввести текст вручную" << endl;
    cout << "2. Прочитать из файла" << endl;
    cout << "Выберите источник: ";
    cin >> source;
    cin.ignore();
    
    string data;
    
    if (source == 1) {
        cout << "Введите текст: ";
        getline(cin, data);
    } else if (source == 2) {
        string filename;
        cout << "Введите имя файла: ";
        getline(cin, filename);
        data = readFile(filename);
        if (data.empty()) {
            cerr << "Ошибка: файл пуст или не найден!" << endl;
            waitForEnter();
            return;
        }
        cout << "Прочитано " << data.length() << " байт." << endl;
    } else {
        cerr << "Неверный выбор!" << endl;
        waitForEnter();
        return;
    }
    
    if (data.empty()) {
        cerr << "Ошибка: данные пусты!" << endl;
        waitForEnter();
        return;
    }
    
    // Работа с ключом
    int keyChoice;
    string key;
    
    cout << "1. Ввести ключ вручную" << endl;
    cout << "2. Сгенерировать случайный ключ" << endl;
    cout << "Выберите способ: ";
    cin >> keyChoice;
    cin.ignore();
    
    if (keyChoice == 1) {
        cout << "Введите ключ: ";
        getline(cin, key);
    } else if (keyChoice == 2) {
        int keyLength;
        cout << "Введите длину ключа (рекомендуется 16-32): ";
        cin >> keyLength;
        cin.ignore();
        
        if (keyLength <= 0) {
            cerr << "Ошибка: длина ключа должна быть положительной!" << endl;
            waitForEnter();
            return;
        }
        
        key = generateKey(keyLength);
        cout << "Сгенерированный ключ (HEX): ";
        for (unsigned char c : key) {
            printf("%02X ", c);
        }
        cout << endl;
    } else {
        cerr << "Неверный выбор!" << endl;
        waitForEnter();
        return;
    }
    
    if (key.empty()) {
        cerr << "Ошибка: ключ не может быть пустым!" << endl;
        waitForEnter();
        return;
    }
    
    // Обработка
    string result = xorProcess(data, key);
    if (result.empty()) {
        waitForEnter();
        return;
    }
    
    // Вывод результата
    printResult(result);
    
    // Сохранение
    char saveChoice;
    cout << "" << endl;
    cout << "Сохранить результат в файл? (YES/NO): ";
    cin >> saveChoice;
    cin.ignore();
    
    if (saveChoice == 'y' || saveChoice == 'Y') {
        string outputFile;
        cout << "Введите имя файла: ";
        getline(cin, outputFile);
        
        if (writeFile(outputFile, result)) {
            cout << "Сохранено в: " << outputFile << endl;
        }
    }
    
    cout << "" << endl;
    cout << "ВАЖНО: XOR симметричен!" << endl;
    cout << "Для расшифровки примените тот же ключ повторно." << endl;
    
    if (keyChoice == 2) {
        cout << "Ключ (HEX): ";
        for (unsigned char c : key) {
            printf("%02X ", c);
        }
        cout << endl;
    }
    
    waitForEnter();
}