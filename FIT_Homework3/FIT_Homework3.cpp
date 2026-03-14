#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const unsigned char MARKS_NUMBER = 11; // число предметов для расчёта рейтинга

struct Student // описание студента
{
    string name; // имя
    string surname; // фамилия

    unsigned char marks[MARKS_NUMBER]; // перечень оценок для расчёта рейтинга

    Student* next; // указатель на следующий элемент списка по фамилиям
    Student* prev; // указатель на предыдущий элемент списка по фамилиям
};

Student* head = nullptr;

unsigned short InputNumber(string message, string error, int min, int max) // получаем целые в нужном диапазоне
{
    bool isCorrect = false;
    unsigned short number;
    do
    {
        cout << message;
        cin >> number;

        if (cin.fail() || number > max || number < min) // какой-то аналог трайпарс видимо
        {
            cout << error << '\n';
            cin.clear(); // эта штука похоже говорит забудь прошлую ошибку мы всё учли будем пробовать снова
            cin.ignore(32767, '\n');   // очищает буфер от ошибочного числа которое было введено. 
            // написано, если его не очистить, то влетаем в бесконечный цикл
            // потому что продолжают читать неправильные данные
        }
        else
            isCorrect = true;

    } while (!isCorrect);
    return number;
}

double CalculateRating(Student stud) // вычисление рейтинга для сортировки в будущем
{
    double rating = 0.0;
    for (int p = 0; p < MARKS_NUMBER; p++)
    {
        rating += stud.marks[p];
    }
    return rating / MARKS_NUMBER;
}

unsigned short GetMenuAction(string option1, string option2, string message) // пришло с лаб по программированию. максимально упрощено
{
    unsigned short action;

    cout << message << '\n';
    cout << "1 " << option1 << '\n';
    cout << "2 " << option2 << '\n';
    cout << "Введите номер выбранного действия 1 или 2(никакой другой ввод обработан не будет): ";

    cin >> action;

    return action;
}

string ToString(Student stud)
{
    return stud.surname + " " + stud.name + " Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
}

void PrintIter() // будет печатать по возрастанию
{
    if (head == nullptr)
        cout << "Список пуст\n";
    else
    {
        Student* stud = head;
        while (stud != nullptr)
        {
            cout << ToString(*stud);
            stud = stud->next;
        }
    }
}

void InsertIter(Student* stud) // вставляет итерационно сортируя по фамилиям. фамилии уникальны
{
    if (head == nullptr) // если список пустой вставляем первым
    {
        head = stud; // в списке из одного элемента голова и хвост одинаковые
        head->next = nullptr; // нет элементов после
        head->prev = nullptr; // нет элементов до
        return;
    }

    if (stud->surname < head->surname) // если голова больше студента
    {
        stud->next = head; // не потеряли голову
        head->prev = stud; // студент стал предыдущим для старой голвы
        stud->prev = nullptr; // перед студентом никого
        head = stud; // студент теперь голова
        return;
    }
    
    Student* current = head; // начинаем искать место с головы
    while (current != nullptr && stud->surname > current->surname)
        current = current->next;

    if (current == nullptr)
    {
        Student* last = head;
        while (last->next != nullptr)
            last = last->next;

        last->next = stud;
        stud->prev = last;
        stud->next = nullptr;
        return;
    }   
    
    stud->prev = current;
    stud->next = current->next;

    if (current->next != nullptr)
        current->next->prev = stud;

    current->next = stud;
}

void InsertRecHelper(Student* cur, Student* stud)
{
    if (cur->next == nullptr) // дошли до конца списка
    {
        cur->next = stud;
        stud->prev = cur;
        stud->next = nullptr;
        return;
    }

    if (stud->surname < cur->next->surname) // место в середине
    {
        stud->next = cur->next;
        stud->prev = cur;
        cur->next->prev = stud;
        cur->next = stud;
        return;
    }

    InsertRecHelper(cur->next, stud);
}

void InsertRec(Student* stud) // вставляет рекурсивно сортируя по фамилиям. фамилии уникальны
{
    stud->next = nullptr;
    stud->prev = nullptr;

    if (head == nullptr)
    {
        head = stud;
        return;
    }

    if (stud->surname < head->surname)
    {
        stud->next = head;
        head->prev = stud;
        head = stud;
        return;
    }

    if (head->next == nullptr)
    {
        head->next = stud;
        stud->prev = head;
        return;
    }

    InsertRecHelper(head, stud); // Запускаем helper с головы
}

void MakeList()
{
    int len;
    len = InputNumber("Введите количество элементов в списке(от 1): ", "Вы не ввели целое число в разрешённом дипазоне", 1, INT16_MAX);

    switch (GetMenuAction("Создать вручную", "Прочитать из файла", "Выберете как ввести список:"))
    {
    case 1: // ручное создание

        for (int p = 1; p <= len; p++)
        {
            Student* newStudent = new Student; // создаём нового студента для вставки

            cout << "Введите фамилию: ";
            cin >> newStudent->surname; // его фамилия
            cout << "Введите имя: ";
            cin >> newStudent->name; // его имя

            for (int q = 0; q < MARKS_NUMBER; q++) // его оценки
                newStudent->marks[q] = InputNumber("Введите оценку: ", "Ошибка! Введите число от 1 до 10\n", 1, 10);

            InsertIter(newStudent); // вставляем в правильном порядке
        }
        break;

    case 2: // чтение из файла

        string filename;
        cout << "Введите имя файла: ";
        cin >> filename;

        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Не удалось открыть файл!\n";
            break;
        }

        string line;
        int count = 0;
        while (getline(file, line) && count < len)
        {
            stringstream ss(line); // что-то вроде буфера со значениями
            string token; // для запоминания значений

            Student* newStudent = new Student;

            getline(ss, token, '|'); // чтение фамилии
            newStudent->surname = token;

            getline(ss, token, '|'); // чтение имени
            newStudent->name = token;

            for (int i = 0; i < MARKS_NUMBER; i++) // чтение оценок
            {
                getline(ss, token, '|');
                newStudent->marks[i] = stoi(token);
            }

            InsertRec(newStudent); // вставляем с сортировкой
            count++;
        }

        file.close(); // закрыли откуда читали
        break;
    }
}

void SearchIter(string targetSurname) // будет искать итерационно
{
    if (head == nullptr) // вывели предупреждение и закончили работу
        cout << "Невозможно искать в пустом списке\n";
    else
    {
        Student* cur = head;
        while (cur != nullptr) // пока не дойдём до конца
        {
            if (cur->surname == targetSurname) // если нашли
            {
                cout << ToString(*cur); // выводим данные
                return; // и заканчиваем работу поиска
            }
            cur = cur->next; // сдвигаемся к концу
        }
        cout << "Студент не найден"; // если оказались на этой строке значит точно не нашли
    }
}

void SearchRec(Student* cur, string targetSurname) // будет искать рекурсивно
{
    if (cur == nullptr) // если не вышли до того как упёрлись в пустоту значит не нашли
    {
        cout << "Студент не найден";
        return;
    }

    if (cur->surname == targetSurname) // если совпало
    {
        cout << ToString(*cur); // печатаем данные
        return; // и заканчиваем
    }

    SearchRec(cur->next, targetSurname); // продолжаем идти вперёд и искать
}

void RemoveStudent() // удаяет студент по фамилии если возможно
{
    if (head == nullptr) // если список пустой то из него не удалить
    {
        cout << "Невозможно удалить в пустом списке";
        return; // сразу закончили работу
    }

    string surname; // получили фамилию для удаления
    cout << "Введите фамилию для удаления";
    cin >> surname;

    if (head->surname == surname) // удалили голову
    {
        if (head->next != nullptr) // если за головой что-то есть
        {
            Student* copy = head->next; // новая голова тот который был за старой
            delete head; // удалили старую

            head = copy; // сделали новыю голову, но у неё 
            head->prev = nullptr; // прев не налл что неверно поэтому сделали налл

            return; // закончили
        }
        else // если список только из головы
        {
            delete head; // удалили
            head = nullptr; // обнулили
            return; // закончили
        }
    }

    Student* cur = head->next;

    while (cur != nullptr) // пока не дошли до конца
    {
        if (cur->surname == surname) // если совпало
        {
            if (cur->next != nullptr) // если удаляемый элемент не последний надо подвигать связи
            {
                cur->prev->next = cur->next; // тот который был до теперь указателем на следующего должен смотреть на того кто был после
                cur->next->prev = cur->prev; // тот кто был после теперь указателем не предыдущего должен смотреть на того кто был до
            }
            delete cur; // удалили
            return;
        }

        cur = cur->next; // шагаем вперёд
    }

    cout << "В списке нет такой фамилии"; // если дошли до сюда и не вышли раньше значит фамилии не было
}

void AddStudent()
{
    Student* stud = new Student; // создали студента

    cout << "Введите фамилию студента:\n"; // узнали его фамилию
    cin >> stud->surname;

    cout << "Введите имя студента:\n"; // имя
    cin >> stud->name;

    for (int p = 0; p < MARKS_NUMBER; p++) // оценки
        stud->marks[p] = InputNumber("Введите оценку: ", "Ошибка! Введите число от 1 до 10\n", 1, 10);

    InsertIter(stud); // добавили в список
}

void PrintRec(Student* cur) // рекурсивная печать списка
{
    if (cur == nullptr) // базовый случай: дошли до конца или список пуст
    {
        if (head == nullptr) // если это первый вызов и список пуст
            cout << "Список пуст\n";
        return;
    }

    cout << ToString(*cur); // печатаем текущий
    PrintRec(cur->next);    // рекурсивно идём дальше
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    system("chcp 65001");

    while (GetMenuAction("Продолжить работу", "Завершить работу", "Выберите действие: ") != 2)
    {
        switch (GetMenuAction("Вручную", "Чтение из файла", "Как создать список: "))
        {
            case 1:
                MakeList();
            break;

            case 2:
                AddStudent();
            break;
        }

        switch (GetMenuAction("Итерационно", "Рекурсивно", "Как напечатать список: "))
        {
            case 1:
                PrintIter();
            break;

            case 2:
                PrintRec(head);
            break;
        }

        if (GetMenuAction("Да", "Нет", "Хотите найти студента?") == 1)
        {
            string surname;
            cout << "Введите фамилию для поиска: ";
            cin >> surname;

            cout << "Итеративный поиск:\n";
            SearchIter(surname);

            cout << "Рекурсивный поиск:\n";
            SearchRec(head, surname);
        }

        if (GetMenuAction("Да", "Нет", "Хотите удалить студента?") == 1)
            RemoveStudent();

        if (GetMenuAction("Да", "Нет", "Хотите добавить студента?") == 1)
            AddStudent();

        PrintIter();
    }

    while (head != nullptr) // поработали почистились
    {
        Student* temp = head;
        head = head->next;
        delete temp;
    }
}
