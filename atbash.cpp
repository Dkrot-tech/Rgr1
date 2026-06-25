#include "atbash.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <clocale>  
#include <locale> 
#include <codecvt>
using namespace std;

// КОНВЕРТАЦИЯ UTF-8 -> UTF-16
static wstring stringToWstring(const string& str) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    try {
        return converter.from_bytes(str);
    } catch (...) {
        return L"";
    }
}

// КОНВЕРТАЦИЯ UTF-16 -> UTF-8
static string wstringToString(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    try {
        return converter.to_bytes(wstr);
    } catch (...) {
        return "";
    }
}

// ПРОВЕРКА НА КИРИЛЛИЦУ
static bool isCyrillic(wchar_t c) {
    return (c >= 0x0410 && c <= 0x044F) || c == 0x0401 || c == 0x0451;
}

// ПРОВЕРКА НА ЛАТИНИЦУ
static bool isLatin(wchar_t c) {
    return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z');
}

// ПРОВЕРКА, ЯВЛЯЕТСЯ ЛИ СИМВОЛ БУКВОЙ
static bool isAlpha(wchar_t c) {
    return isLatin(c) || isCyrillic(c);
}

// ПРОВЕРКА НА ЦИФРУ
static bool isDigit(wchar_t c) {
    return (c >= L'0' && c <= L'9');
}

// ПРОВЕРКА, ЯВЛЯЕТСЯ ЛИ СИМВОЛ БУКВОЙ ИЛИ ЦИФРОЙ
static bool isAlnum(wchar_t c) {
    return isAlpha(c) || isDigit(c);
}

// ПЕРЕВОД В ВЕРХНИЙ РЕГИСТР
static wchar_t toUpper(wchar_t c) {
    if (isLatin(c)) {
        if (c >= L'a' && c <= L'z') {
            return c - (L'a' - L'A');
        }
        return c;
    }

    if (isCyrillic(c)) {
        if (c >= 0x0430 && c <= 0x044F) {
            return c - 0x20;
        }
        if (c == 0x0451) return 0x0401;
        return c;
    }
    return c;
}

// ПЕРЕВОД В НИЖНИЙ РЕГИСТР
static wchar_t toLower(wchar_t c) {
    if (isLatin(c)) {
        if (c >= L'A' && c <= L'Z') {
            return c + (L'a' - L'A');
        }
        return c;
    }

    if (isCyrillic(c)) {
        if (c >= 0x0410 && c <= 0x044F) {
            return c + 0x20;
        }
        if (c == 0x0401) return 0x0451;
        return c;
    }
    return c;
}

// ОБРАТНЫЙ СИМВОЛ ДЛЯ ЛАТИНИЦЫ (A<->Z, B<->Y, ...)
static wchar_t atbashLatin(wchar_t c) {
    if (c >= L'A' && c <= L'Z') {
        return static_cast<wchar_t>(L'Z' - (c - L'A'));
    } else if (c >= L'a' && c <= L'z') {
        return static_cast<wchar_t>(L'z' - (c - L'a'));
    }
    return c;
}

// ОБРАТНЫЙ СИМВОЛ ДЛЯ КИРИЛЛИЦЫ (А<->Я, Б<->Ю, ...)
static wchar_t atbashCyrillic(wchar_t c) {
    // Заглавные буквы А-Я (без Ё)
    if (c >= 0x0410 && c <= 0x042F) {
        return static_cast<wchar_t>(0x042F - (c - 0x0410));
    }
    // Строчные буквы а-я (без ё)
    else if (c >= 0x0430 && c <= 0x044F) {
        return static_cast<wchar_t>(0x044F - (c - 0x0430));
    }
    // Ё и ё
    else if (c == 0x0401) {
        return 0x0401; // Ё -> Ё (зеркально сам себе)
    }
    else if (c == 0x0451) {
        return 0x0451; // ё -> ё (зеркально сам себе)
    }
    return c;
}

// ОБРАТНЫЙ СИМВОЛ ДЛЯ ЦИФР (0<->9, 1<->8, ...)
static wchar_t atbashDigit(wchar_t c) {
    if (c >= L'0' && c <= L'9') {
        return static_cast<wchar_t>(L'9' - (c - L'0'));
    }
    return c;
}

// ОБРАБОТКА СИМВОЛА АТБАШ
static wchar_t atbashChar(wchar_t c) {
    if (isLatin(c)) {
        return atbashLatin(c);
    } else if (isCyrillic(c)) {
        return atbashCyrillic(c);
    } else if (isDigit(c)) {
        return atbashDigit(c);
    }
    return c; // Символ не обрабатываем
}

// ОБРАБОТКА ТЕКСТА АТБАШ
static bool atbashProcess(const string& input, string& output) {
    if (input.empty()) {
        return false;
    }

    wstring wInput = stringToWstring(input);
    if (wInput.empty()) {
        return false;
    }
    
    wstring wOutput;
    wOutput.reserve(wInput.length());

    for (wchar_t c : wInput) {
        wOutput.push_back(atbashChar(c));
    }

    output = wstringToString(wOutput);
    return true;
}

// РАБОТА С ФАЙЛАМИ (ТЕКСТОВЫЙ РЕЖИМ)
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

// ГЛАВНАЯ ФУНКЦИЯ АТБАШ
void atbashMain() {
    setlocale(LC_ALL, "");
    system("clear"); // или "cls"
    
    cout << "" << endl;
    cout << "         ШИФР АТБАШ" << endl;
    cout << "   )" << endl;
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
    
    // Обработка
    string result;
    bool success = atbashProcess(text, result);
    
    if (!success) {
        cerr << "Ошибка при обработке текста!" << endl;
        waitForEnter();
        return;
    }
    
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
