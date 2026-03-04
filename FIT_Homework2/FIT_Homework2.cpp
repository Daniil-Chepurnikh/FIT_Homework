#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const unsigned char MARKS_NUMBER = 11; // число предметов для расчёта рейтинга
const unsigned char STUDENTS_NUMBER = 11; // ограничение на число студентов

unsigned char removed = 0; // сколько было удалено

struct Student // описание студента
{
    unsigned number; // номер зачётки

    string name; // имя
    string surname; // фамилия
    string patronymic; // отчество

    bool isRemoved; // метка удаления

    unsigned char marks[MARKS_NUMBER]; // перечень оценок для расчёта рейтинга
};

struct IndexStudentSurnameName // поменять
{
    short originIndex; // индекс в исходнике
    string surname; // ключевое поле фамилии(сортировать начинаем по нему)
    string name; // надесюсь, поле имени не станет ключевым, но если вдруг совпадут фамилии продолжим сортировать по нему
};

struct IndexStudentRating // поменять
{
    short originIndex;
    double rating;
};

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

void MakeArray(Student studs[]) // ручное создание массива
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        studs[p].number = p + 1;
        studs[p].isRemoved = false;

        cout << "Введите фамилию студента\n";
        cin >> studs[p].surname;

        cout << "Введите имя студента\n";
        cin >> studs[p].name;

        cout << "Введите отчество студента\n";
        cin >> studs[p].patronymic;

        for (int q = 0; q < MARKS_NUMBER; q++) // число от 1 до 10 точно войдёт в чар и незачем усложнять
            studs[p].marks[q] = InputNumber("Введите оценку за предмет\n", "Вы не ввели целое число в допустимом диапазоне от 1 до 10\n", 1, 10);
    }
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

string ToString(Student stud)
{
    return to_string(stud.number) + " " + stud.surname + " " + stud.name + " " + stud.patronymic + " Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
}

void PrintArrayConsole(Student studs[])
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        cout << ToString(studs[p]);
    }
} // в отчёте

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

void PrintArrayFile(Student studs[]) // магия чистой воды. и сам и железным другом
{
    string fileName;
    switch (GetMenuAction("Новый файл", "Существующий файл", "Выберете в какой файл писать"))
    {
    case 1:
    {
        cout << "Как нужно назвать новый файл?\n"; // сам придумал так сделать. больше удобства
        cin >> fileName;
        fileName = fileName + ".txt";

        ofstream outFile(fileName); // говорим какой файл надо создать и открыть

        if (outFile.is_open()) // проверяем, открылся ли файл
        {
            for (int p = 0; p < STUDENTS_NUMBER; p++) // пишем в файл
            {
                outFile << ToString(studs[p]);
            }
            outFile.close(); // закрываем файл
        }
        else
            cout << "Ошибка при открытии файла!\n";
        break;
    }
    case 2:
    {
        ofstream file(fileName, ios::app); // 

        if (file.is_open())
        {
            for (int p = 0; p < STUDENTS_NUMBER; p++) // пишем в файл
            {
                file << ToString(studs[p]);
            }
            file.close(); // закрываем файл
        }
        else
            cout << "Ошибка при открытии файла!\n";
        break;
    }
    }
} // в отчёте

void ReadArray(Student studs[])
{
    ifstream file("input.txt"); // читать отсюда
    if (!file.is_open())
    {
        cout << "Не удалось открыть файл!\n";
        return;
    }

    string line;
    int studentIndex = 0;
    while (getline(file, line) && studentIndex < STUDENTS_NUMBER)
    {
        stringstream ss(line); // создаёт какой-то строковый поток, чтобы эту самую строку делить по разделителям. сложно
        string token; // читаемый объект

        studs[studentIndex].isRemoved = false;

        getline(ss, token, '|'); // номер
        studs[studentIndex].number = stoi(token); // видимо что-то вроде конверт или парс

        getline(ss, token, '|'); // фамилия
        studs[studentIndex].surname = token;

        getline(ss, token, '|'); // имя
        studs[studentIndex].name = token;

        getline(ss, token, '|'); // отчество
        studs[studentIndex].patronymic = token;

        for (int i = 0; i < MARKS_NUMBER; i++)  // сразу в массив. без дополнительных проверок
        {
            getline(ss, token, '|');
            studs[studentIndex].marks[i] = stoi(token);
        }

        studentIndex++;
    }
    file.close();
}

// TODO: переделать
void PrintIndexStudentSurnameName(/* Добавит новый индекс и изменить функцию*/ Student studs[]) // будет печатать по возрастанию
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        int newIndex = index[p].originIndex; // удаление из исходника записи не очень заботит индекс

        if (!studs[newIndex].isRemoved) // если не удалено выводим
            cout << ToString(studs[newIndex]);
    }
}

// TODO: переделать
void PrintIndexStudentRating(IndexStudentRating index[], Student studs[]) // будет печатать по убыванию
{
    for (int p = STUDENTS_NUMBER - 1; p >= 0; p--)
    {
        int newIndex = index[p].originIndex; // удаление из исходника записи не очень заботит индекс

        if (!studs[newIndex].isRemoved) // если не удалено выводим
            cout << ToString(studs[newIndex]);
    }
}

short SearchIndexStudentSurnameNameIter(IndexStudentSurnameName index[], string surname, string name) // будет искать итерационно
{
    // TODO: написать
}

short SearchIndexStudentRatingRec(IndexStudentRating index[], int left, int right, double rating) // будет искать рекурсивно
{
    // TODO: написать
}

void RemoveStudent(Student studs[], double rating) // будем удалять по рейтингу, будто бы отчислили. вроде бы логично
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        if (!studs[p].isRemoved && CalculateRating(studs[p]) == rating)
        {
            studs[p].isRemoved = true;
            removed++;
        }
    }
}

void UpdateIndex(Student studs[], IndexStudentSurnameName indexSurname[], IndexStudentRating indexRating[], int newSize)
{
    // TODO: переделать
    
    int activeIndex = 0; // активные студенты в начало
    for (int q = 0; q < STUDENTS_NUMBER && activeIndex < newSize; q++)
    {
        if (!studs[q].isRemoved) // не работаем с удалёнными
        {
            indexSurname[activeIndex].originIndex = q;
            indexSurname[activeIndex].surname = studs[q].surname;
            indexSurname[activeIndex].name = studs[q].name;

            indexRating[activeIndex].originIndex = q;
            indexRating[activeIndex].rating = CalculateRating(studs[q]);
            activeIndex++;
        }
    }
}

int PhysicalRemoveStudentsAndUpdateIndex(Student studs[], IndexStudentSurnameName indexSurname[], IndexStudentRating indexRating[]) // с большой помощью железного друга
{
    // TODO: переделать    
    
    int newSize = 0;

    for (int q = 0; q < STUDENTS_NUMBER; q++) // сдвигаем всех неудалённых в начало массива
    {
        if (!studs[q].isRemoved) // если не удалён
        {
            if (q != newSize) // чтобы не делать лишнего
                studs[newSize] = studs[q]; // на новое место

            newSize++;
        }
    }
    UpdateIndex(studs, indexSurname, indexRating, newSize);

    return newSize; // чтобы двигаться только по неудалённым студентам в других функциях
}

void ReinstateStudent(Student studs[], double rating) // восстановление
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        if (studs[p].isRemoved && CalculateRating(studs[p]) == rating)
        {
            studs[p].isRemoved = false;
            removed--;
        }
    }
}

void EditStudentAndUpdateIndex(Student studs[], IndexStudentSurnameName indexSurname[], IndexStudentRating indexRating[], int index)
{
    // TODO: переделать
    
    // страшнее чем удаление из-за изменения ключей. очень тяжело на уставшего человека

    if (index < 0 || index >= STUDENTS_NUMBER || studs[index].isRemoved) // не работать по ошибкам
    {
        cout << "Некорректный индекс или студент удалён\n";
        return;
    }

    cout << "Редактирование студента:\n";
    cout << ToString(studs[index]);

    // персональные данные
    cout << "Введите новое имя: ";
    cin >> studs[index].name;

    cout << "Введите новую фамилию: ";
    cin >> studs[index].surname;

    cout << "Введите новое отчество: ";
    cin >> studs[index].patronymic;

    // оценки
    for (int q = 0; q < MARKS_NUMBER; q++)
    {
        studs[index].marks[q] = InputNumber("Введите новую оценку: ", "Вы не ввели целое число в допустимом диапазоне от 1 до 10\n", 1, 10);
    }

    int activeCount = 0;
    for (int p = 0; p < STUDENTS_NUMBER; p++) // считает количество активных студентов
    {
        if (!studs[p].isRemoved)
            activeCount++;
    }

    UpdateIndex(studs, indexSurname, indexRating, activeCount);

    cout << "Студент отредактирован:\n";
    cout << ToString(studs[index]);
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8"); // была какая-то жесть с выводом и товарищ интернет сказал сделать так
    system("chcp 65001"); // чтобы писались нормальные русские слова

    // TODO: переделать

}
