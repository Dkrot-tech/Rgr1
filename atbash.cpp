#include "atbash.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <clocale>  
#include <locale> 
using namespace std;

//АЛФАВИТ
static string getAlphabet() {
    string rusUpper = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    string rusLower = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    string engUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string engLower = "abcdefghijklmnopqrstuvwxyz";
    string digits = "0123456789";
    string specials = "!@#$%^&*()_+-=[]{}|;:,.<>?/`~";
    return rusUpper + rusLower + engUpper + engLower + digits + specials;
}

//ОБРАБОТКА СИМВОЛА
char atbashChar(char c) {
    string alphabet = getAlphabet();
    size_t pos = alphabet.find(c);
    if (pos == string::npos) return c;
    
    int N = alphabet.size();
    int mirroredPos = N - 1 - pos;
    return alphabet[mirroredPos];
}

// ОБРАБОТКА ТЕКСТА 
string atbashProcess(const string& text) {
    string result = "";
    for (char c : text) {
        result += atbashChar(c);
    }
    return result;
}

//РАБОТА С ФАЙЛАМИ
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

//ГЛАВНАЯ ФУНКЦИЯ АТБАШ
void atbashMain() {

    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    system("clear"); // или "cls"
    
    cout << "" << endl;
    cout << "         ШИФР АТБАШ" << endl;

    
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
    
    // Обработка
    string result = atbashProcess(text);
    
    // Вывод результата
    printResult(result);
    
    // Сохранение
    char saveChoice;
    cout << "" << endl;
    cout << "Сохранить результат в файл? (y/n): ";
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
    
    
    
    
    waitForEnter();
}