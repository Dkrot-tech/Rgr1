#include "vigenere.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <clocale>  
#include <locale> 
#include <codecvt>
#include <vector>
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

// ВАЛИДАЦИЯ КЛЮЧА
static bool isKeyValid(const wstring& key) {
    if (key.empty()) return false;
    for (wchar_t c : key) {
        if (!isAlpha(c)) {
            return false;
        }
    }
    return true;
}

// ПРЕОБРАЗОВАНИЕ КЛЮЧА В ЧИСЛОВЫЕ СМЕЩЕНИЯ
static vector<int> getKeyOffsets(const wstring& key) {
    vector<int> offsets;
    for (wchar_t c : key) {
        wchar_t upper = toUpper(c);
        if (isLatin(upper)) {
            offsets.push_back(upper - L'A');
        } else if (isCyrillic(upper)) {
            offsets.push_back(upper - 0x0410);
        } else {
            offsets.push_back(0);
        }
    }
    return offsets;
}

// СДВИГ СИМВОЛА
static wchar_t shiftChar(wchar_t c, int offset, bool encrypt) {
    if (!isAlpha(c)) {
        return c;
    }

    wchar_t base;
    int alphabetSize;
    wchar_t upper = toUpper(c);

    if (isLatin(upper)) {
        base = L'A';
        alphabetSize = 26;
    } else if (isCyrillic(upper)) {
        base = 0x0410;
        alphabetSize = 32;
    } else {
        return c;
    }

    int shift = encrypt ? offset : (alphabetSize - offset);
    wchar_t result = static_cast<wchar_t>((upper - base + shift) % alphabetSize + base);
    
    // Восстанавливаем регистр
    if (isLatin(c)) {
        if (c >= L'a' && c <= L'z') {
            result = result - L'A' + L'a';
        }
    } else if (isCyrillic(c)) {
        if (c >= 0x0430 && c <= 0x044F) {
            result = result - 0x0410 + 0x0430;
        }
        if (c == 0x0451) {
            if (result == 0x0401) return 0x0451;
        }
    }
    
    return result;
}

// ШИФРОВАНИЕ
static bool encrypt(const string& plaintext, const wstring& key, string& ciphertext) {
    if (plaintext.empty() || key.empty()) {
        return false;
    }

    wstring wPlaintext = stringToWstring(plaintext);
    if (wPlaintext.empty()) {
        return false;
    }
    
    vector<int> offsets = getKeyOffsets(key);
    wstring wCiphertext;
    wCiphertext.reserve(wPlaintext.length());

    size_t keyIndex = 0;
    for (wchar_t c : wPlaintext) {
        if (isAlpha(c)) {
            wchar_t processed = shiftChar(c, offsets[keyIndex % offsets.size()], true);
            wCiphertext.push_back(processed);
            keyIndex++;
        } else {
            wCiphertext.push_back(c);
        }
    }

    ciphertext = wstringToString(wCiphertext);
    return true;
}

// ДЕШИФРОВАНИЕ
static bool decrypt(const string& ciphertext, const wstring& key, string& plaintext) {
    if (ciphertext.empty() || key.empty()) {
        return false;
    }

    wstring wCiphertext = stringToWstring(ciphertext);
    if (wCiphertext.empty()) {
        return false;
    }
    
    vector<int> offsets = getKeyOffsets(key);
    wstring wPlaintext;
    wPlaintext.reserve(wCiphertext.length());

    size_t keyIndex = 0;
    for (wchar_t c : wCiphertext) {
        if (isAlpha(c)) {
            wchar_t processed = shiftChar(c, offsets[keyIndex % offsets.size()], false);
            wPlaintext.push_back(processed);
            keyIndex++;
        } else {
            wPlaintext.push_back(c);
        }
    }

    plaintext = wstringToString(wPlaintext);
    return true;
}

// ГЕНЕРАЦИЯ СЛУЧАЙНОГО КЛЮЧА
static string generateKey(int length) {
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string key = "";
    int N = alphabet.size();
    
    srand(time(0) + rand());
    for (int i = 0; i < length; i++) {
        key += alphabet[rand() % N];
    }
    return key;
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

// ГЛАВНАЯ ФУНКЦИЯ ВИЖЕНЕРА
void vigenereMain() {
    setlocale(LC_ALL, "");
    system("clear"); // или "cls"
    
    cout << "" << endl;
    cout << "         ШИФР ВИЖЕНЕРА " << endl;
    cout << "  " << endl;
    cout << "" << endl;
    
    wstring key;
    string keyUtf8;
    
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
    
    cout << "1. Ввести ключ вручную" << endl;
    cout << "2. Сгенерировать случайный ключ" << endl;
    cout << "Выберите способ: ";
    cin >> keyChoice;
    cin.ignore();
    
    if (keyChoice == 1) {
        cout << "Введите ключ (только буквы, русские или английские): ";
        getline(cin, keyUtf8);
        
        key = stringToWstring(keyUtf8);
        if (key.empty() || !isKeyValid(key)) {
            cerr << "Ошибка: неверный ключ! Используйте только буквы." << endl;
            waitForEnter();
            return;
        }
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
        
        keyUtf8 = generateKey(keyLength);
        key = stringToWstring(keyUtf8);
        cout << "Сгенерированный ключ: " << keyUtf8 << endl;
    } else {
        cerr << "Неверный выбор!" << endl;
        waitForEnter();
        return;
    }
    
    if (key.empty()) {
        cerr << "Ошибка: ключ не установлен!" << endl;
        waitForEnter();
        return;
    }
    
    // Обработка
    string result;
    bool success;
    
    if (mode == 1) {
        success = encrypt(text, key, result);
    } else {
        success = decrypt(text, key, result);
    }
    
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
    
    if (mode == 1 && keyChoice == 2) {
        cout << "" << endl;
        cout << "Сохраните ключ для расшифровки!" << endl;
        cout << "Ключ: " << keyUtf8 << endl;
    }
    
    waitForEnter();
}
