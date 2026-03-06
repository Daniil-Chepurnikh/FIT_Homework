#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const unsigned char MARKS_NUMBER = 11; // число предметов для расчёта рейтинга
const unsigned char STUDENTS_NUMBER = 11; // ограничение на число студентов

unsigned char removed = 0; // сколько было удалено

string FILENAME = "output.txt"; // исправление по сравнению с заданием 1. там было утеряно имя и файл 
// на запись никогда не открывался

struct Student // описание студента
{
    unsigned number; // номер зачётки

    string name; // имя
    string surname; // фамилия
    string patronymic; // отчество

    bool isRemoved; // метка удаления

    unsigned char marks[MARKS_NUMBER]; // перечень оценок для расчёта рейтинга
};

struct IndexStudentSurname // бинарное дерево по фамилии. фамилии будут уникальные
{
    short originIndex; // индекс в исходнике
    string surname; // ключевое поле фамилии сортировать начинаем по нему

    IndexStudentSurname* left; // левый потомок вершины
    IndexStudentSurname* right; // правый потомок вершины

};

struct IndexStudentRating // бинарное дерево по рейтингу. рейтинг будет уникальный
{
    short originIndex; // индекс в исходнике
    double rating; // рейтинг

    IndexStudentRating* left; // левый потомок вершины
    IndexStudentRating* right; // правый потомок вершины
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
    if (!stud.isRemoved)
        return to_string(stud.number) + " " + stud.surname + " " + stud.name + " " + stud.patronymic + " Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
    else
        return "Студент удалён";
}

void PrintArrayConsole(Student studs[])
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        cout << ToString(studs[p]);
    }
}

void PrintArrayFile(Student studs[]) // магия чистой воды. и сам и железным другом
{
    string fileName = FILENAME; // в случае записи в существующий файл мы теперь знаем куда писать
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

            FILENAME = fileName; // запомнили куда писали
        }
        else
            cout << "Ошибка при открытии файла!\n";
        break;
    }
    case 2:
    {
        ofstream file(fileName, ios::app);

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

void PrintIndexStudentSurnameNameRec(IndexStudentSurname* root, Student studs[]) // будет печатать по возрастанию
{ // массив тоже конечно за нами таскается, но, что есть то есть

    if (root == nullptr) // базовый случай точно понимаю. если идти некуда, то мы и не пойдём
        return;

    PrintIndexStudentSurnameNameRec(root->left, studs); // сначала левое поддерево, в коем фамилии меньше
    cout << ToString(studs[root->originIndex]); // потом корень
    PrintIndexStudentSurnameNameRec(root->right, studs); // потом правое поддерево, в коем фамилии больше
}

void PrintIndexStudentRating(IndexStudentRating* root, Student studs[]) // будет печатать по убыванию
{
    if (root == nullptr) // базовый случай точно понимаю. если идти некуда, то мы и не пойдём
        return;

    PrintIndexStudentRating(root->right, studs); // потом правое поддерево, в коем рейтинг больше
    cout << ToString(studs[root->originIndex]); // потом корень
    PrintIndexStudentRating(root->left, studs); // сначала левое поддерево, в коем рейтинг больше
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


short SearchIndexStudentSurnameNameIter(IndexStudentSurname* root, string surname) // будет искать итерационно
{
    while (root != )
}

void SearchIndexStudentRatingRec(IndexStudentRating* root, double targetRating, Student studs[]) // будет искать рекурсивно
{
    if (root == nullptr) // дошли до листьев и не вышли раньше значит не нашли
    {
        cout << "Такого студента не найдено\n";
        return;
    }
    
    if (targetRating > root->rating) // значение больше идём вправо
        SearchIndexStudentRatingRec(root->right, targetRating, studs);
    else if (targetRating < root->rating) // значение меньше идём влево
        SearchIndexStudentRatingRec(root->left, targetRating, studs);
    else // нашли
    {
        cout << ToString(studs[root->originIndex]);
        return;
    }
}

void UpdateIndex(Student studs[], IndexStudentSurname indexSurname[], IndexStudentRating indexRating[], int newSize)
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

int PhysicalRemoveStudentsAndUpdateIndex(Student studs[], IndexStudentSurname indexSurname[], IndexStudentRating indexRating[]) // с большой помощью железного друга
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

void EditStudentAndUpdateIndex(Student studs[], IndexStudentSurname indexSurname[], IndexStudentRating indexRating[], int index)
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
