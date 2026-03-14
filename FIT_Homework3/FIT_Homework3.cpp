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

void Print() // будет печатать по возрастанию
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

void InsertIter(Student* stud) // вставляет сортируя по фамилиям. фамилии уникальны
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







void MakeList(Student studs[]) 
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
                return;
            }

            string line;
            int count = 0;
            while (getline(file, line) && count < len)
            {
                stringstream ss(line);
                string token;

                Student* newStudent = new Student;

                // Читаем фамилию
                getline(ss, token, '|');
                newStudent->surname = token;

                // Читаем имя
                getline(ss, token, '|');
                newStudent->name = token;

                // Читаем оценки
                for (int i = 0; i < MARKS_NUMBER; i++)
                {
                    getline(ss, token, '|');
                    newStudent->marks[i] = stoi(token);
                }

               // InsertRec(newStudent); // вставляем с сортировкой
                count++;
            }

            file.close();
            cout << "Загружено " << count << " студентов из файла\n";
            break;



    }
}

void RemoveStudent(/*добавить указатель на голову списка*/ string surname)
{
    // TODO: написать удаление записи из списка
}

void SearchStudentIter(string targetSurname /*добавить указатель на головоу списка*/) // будет искать итерационно
{

}

void SearchIndexStudentRatingRec(string targetSurname /*добавить указатель на головоу списка*/ ) // будет искать рекурсивно
{
    
}

void ReadList(Student studs[])
{
    ifstream file("input.txt"); // читать отсюда
    if (!file.is_open())
    {
        cout << "Не удалось открыть файл!\n";
        return;
    }
}

void AddNewSurname(/*написать*/) // досадно пока что это всё
{
    // написать
}

void UpdateIndex(/*написать*/)
{
    // написать
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    system("chcp 65001");
        
}
