#include "vigenere.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <clocale>  
#include <locale> 
using namespace std;

// АЛФАВИТ 
static string getAlphabet() {
    string rusUpper = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    string rusLower = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    string engUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string engLower = "abcdefghijklmnopqrstuvwxyz";
    string digits = "0123456789";
    string specials = "!@#$%^&*()_+-=[]{}|;:,.<>?/`~";
    return rusUpper + rusLower + engUpper + engLower + digits + specials;
}

//ГЕНЕРАЦИЯ КЛЮЧА
static string generateKey(int length) {
    string alphabet = getAlphabet();
    string key = "";
    int N = alphabet.size();
    
    srand(time(0) + rand()); // Добавляем rand() для разнообразия
    for (int i = 0; i < length; i++) {
        key += alphabet[rand() % N];
    }
    return key;
}

//ОБРАБОТКА СИМВОЛА
static char vigenereChar(char c, char keyChar, bool encrypt) {
    string alphabet = getAlphabet();
    size_t pos = alphabet.find(c);
    if (pos == string::npos) return c;
    
    size_t keyPos = alphabet.find(keyChar);
    if (keyPos == string::npos) keyPos = 0;
    
    int N = alphabet.size();
    int newPos;
    
    if (encrypt) {
        newPos = (pos + keyPos) % N;
    } else {
        newPos = (pos - keyPos + N) % N;
    }
    
    return alphabet[newPos];
}

// ОБРАБОТКА ТЕКСТА
static string vigenereProcess(const string& text, const string& key, bool encrypt) {
    if (key.empty()) {
        cerr << "Ошибка: ключ не может быть пустым!" << endl;
        return "";
    }
    
    string result = "";
    for (int i = 0; i < text.length(); i++) {
        char keyChar = key[i % key.length()];
        result += vigenereChar(text[i], keyChar, encrypt);
    }
    return result;
}

// РАБОТА С ФАЙЛАМИ
static string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return "";
    }
    
    string content;
    string line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    file.close();
    
    if (!content.empty() && content.back() == '\n') {
        content.pop_back();
    }
    
    return content;
}

static bool writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось создать файл " << filename << endl;
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
static void printResult(const string& result) {
    cout << "\nРЕЗУЛЬТАТ" << endl;
    cout << "" << endl;
    
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

// 
void vigenereMain() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    system("clear"); // или "cls"
    
    cout << "" << endl;
    cout << "         ШИФР ВИЖЕНЕРА" << endl;
    cout << "" << endl;
    
    // Выбор источника
    int source;
    cout << "1. Ввести текст вручную" << endl;
    cout << "2. Прочитать из файла" << endl;
    cout << "Выберите источник: ";
    cin >> source;
    cin.ignore();
    
    string text;
    
    if (source == 1) {
        cout << "Введите текст: ";
        getline(cin, text);
    } else if (source == 2) {
        string filename;
        cout << "Введите имя файла: ";
        getline(cin, filename);
        text = readFile(filename);
        if (text.empty()) {
            cerr << "Ошибка: файл пуст или не найден!" << endl;
            waitForEnter();
            return;
        }
        cout << "Прочитано " << text.length() << " символов." << endl;
    } else {
        cerr << "Неверный выбор!" << endl;
        waitForEnter();
        return;
    }
    
    if (text.empty()) {
        cerr << "Ошибка: текст пуст!" << endl;
        waitForEnter();
        return;
    }
    
    // Выбор режима
    int mode;
    cout << "1. Шифрование" << endl;
    cout << "2. Дешифрование" << endl;
    cout << "Выберите режим: ";
    cin >> mode;
    cin.ignore();
    
    if (mode != 1 && mode != 2) {
        cerr << "Неверный выбор!" << endl;
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
        cout << "Введите длину ключа: ";
        cin >> keyLength;
        cin.ignore();
        
        if (keyLength <= 0) {
            cerr << "Ошибка: длина ключа должна быть положительной!" << endl;
            waitForEnter();
            return;
        }
        
        key = generateKey(keyLength);
        cout << "Сгенерированный ключ: " << key << endl;
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
    string result = vigenereProcess(text, key, mode == 1);
    if (result.empty()) {
        waitForEnter();
        return;
    }
    
    // Вывод результата
    printResult(result);
    
    // Сохранение
    char saveChoice;
    cout << "" << endl;
    cout << "Сохранить результат в файл? (yes/no): ";
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
    
    if (mode == 1 && keyChoice == 2) {
        cout << "" << endl;
        cout << "Сохраните ключ для расшифровки!" << endl;
        cout << "Ключ: " << key << endl;
    }
    
    waitForEnter();
}