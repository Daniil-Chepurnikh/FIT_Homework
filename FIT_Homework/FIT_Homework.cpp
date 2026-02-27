// Тяжело

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const unsigned char MARKS_NUMBER = 11; // число предметов для расчёта рейтинга
const unsigned char STUDENTS_NUMBER = 11; // ограничение на число студентов

struct Student // описание студента
{
    unsigned number; // номер зачётки

    string name; // имя
    string surname; // фамилия
    string patronymic; // отчество

    bool isRemoved;

    unsigned char marks[MARKS_NUMBER]; // перечень оценок для расчёта рейтинга
};

struct IndexStudentSurnameName // отчество не добавлено, потому что я не верю, что в моей любимой вышке у кого-то добуквенно совпадут фамилия и имя
{
    short originIndex; // индекс в исходнике
    string surname; // ключевое поле фамилии(сортировать начинаем по нему)
    string name; // надесюсь, поле имени не станет ключевым, но если вдруг совпадут фамилии продолжим сортировать по нему
};

struct IndexStudentRating
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

        cout << "Введите имя студента\n";
        cin >> studs[p].name;

        cout << "Введите фамилию студента\n";
        cin >> studs[p].surname;

        cout << "Введите отчество студента\n";
        cin >> studs[p].patronymic;

        for (int q = 0; q < MARKS_NUMBER; q++)
        {
            studs[p].marks[q] = InputNumber("Введите оценку за предмет\n", "Вы не ввели целое число в допустимом диапазоне от 1 до 10\n", 1, 10);
        }
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
    return to_string(stud.number) + " " + stud.surname + " " + stud.name + " " + stud.patronymic + " " + "Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
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

void Swap(IndexStudentSurnameName array[], int indexFirst, int indexSecond)
{
    IndexStudentSurnameName clone = array[indexSecond];
    array[indexSecond] = array[indexFirst];
    array[indexFirst] = clone;
}

int Partition(IndexStudentSurnameName array[], int left, int right) // я надеюсь оно сработает
{
    IndexStudentSurnameName pivot = array[left];
    int low = left + 1;
    int high = right;

    while (high >= low)
    {
        while (high >= low && // условие осталось
            (array[low].surname < pivot.surname || // если влетели в неравенство фамилий всё сразу круто
                (array[low].surname == pivot.surname && // если фамилии совпали :( то 
                    array[low].name < pivot.name))) // проверяем имена
        {
            low++;
        }

        while (high >= low && // см выше но только для больших элементов соответственно
            (array[high].surname >= pivot.surname ||
                (array[high].surname == pivot.surname &&
                    array[high].name > pivot.name)))
        {
            high--;
        }

        if (high >= low)
            Swap(array, low, high);
    }

    Swap(array, left, high);
    return high;
}

void HoareSort(IndexStudentSurnameName array[], int left, int right) // будет сортировать по фамилии и имени
{
    if (left < right)
    {
        int pivotIndex = Partition(array, left, right);
        HoareSort(array, left, pivotIndex - 1);
        HoareSort(array, pivotIndex + 1, right);
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

void MakeIndexStudentSurnameName(IndexStudentSurnameName index[], Student studs[])
{
    for (int p = 0; p < STUDENTS_NUMBER; p++) // перезаписали то, что нужно
    {
        index[p].surname = studs[p].surname;
        index[p].name = studs[p].name;
        index[p].originIndex = p;
    }
}

void MakeIndexIndexStudentRating(IndexStudentRating index[], Student studs[])
{
    for (int p = 0; p < STUDENTS_NUMBER; p++) // перезаписали то, что нужно
    {
        index[p].rating = CalculateRating(studs[p]);
        index[p].originIndex = p;
    }
}

void PrintIndexStudentSurnameName(IndexStudentSurnameName index[], Student studs[]) // будет печатать по возрастанию
{
    for (int p = 0; p < STUDENTS_NUMBER; p++)
    {
        int newIndex = index[p].originIndex; // удаление из исходника записи не очень заботит индекс
        
        if (!studs[newIndex].isRemoved) // если не удалено выводим
            cout << ToString(studs[newIndex]);
    }
}

void PrintIndexStudentRating(IndexStudentRating index[], Student studs[]) // будет печатать по убыванию
{
    for (int p = STUDENTS_NUMBER - 1; p >= 0 ; p--)
    {
        int newIndex = index[p].originIndex; // удаление из исходника записи не очень заботит индекс

        if (!studs[newIndex].isRemoved) // если не удалено выводим
            cout << ToString(studs[newIndex]);
    }
}

short BinarySearchIndexStudentSurnameNameIter(IndexStudentSurnameName index[], string surname, string name) // будет искать итерационно
{
    int left = 0;
    int right = STUDENTS_NUMBER - 1;
    bool isFound = false; // на случай если не найдём
    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (index[mid].surname < surname)
            left = mid + 1;
        else if (index[mid].surname > surname)
            right = mid - 1;
        else // фамилии совпали
        {
            // действительно мид может влететь во что-то похоже на индекс вне границ массива
            while (mid > 0 && mid < STUDENTS_NUMBER && index[mid].surname == surname) // значит как говорила Л.Н. на паре идём в разные стороны пока фамилия совпадает и ищем имя
            {
                if (index[mid].name < name)
                    mid--; // на шаг влево
                else if (index[mid].name > name)
                    mid++; // на шаг вправо
                else
                    return index[mid].originIndex; // нашли
            }
        }
    }
    if (!isFound) // либо вышли по return внутри цикла либо не нашли
        return -1; // точно не может быть такого индекса
}

short BinarySearchIndexStudentRatingRec(IndexStudentRating index[], int left, int right, double rating) // будет искать рекурсивно
{
    if (left > right) // не нашли, вылет за границы
        return -1;

    int mid = left + (right - left) / 2;

    if (index[mid].rating < rating)
        return BinarySearchIndexStudentRatingRec(index, mid + 1, right, rating); // сдвинулись вправо
    else if (index[mid].rating > rating)
        return BinarySearchIndexStudentRatingRec(index, left, mid - 1, rating); // сдвинулись влево

    return index[mid].originIndex; // нашли
}


//======================================== В ОТЧЁТЕ УЖЕ ЕСТЬ ==============================================================


int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8"); // была какая-то жесть с выводом и товарищ интернет сказал сделать так
    system("chcp 65001"); // чтобы писались нормальные русские слова

    Student students[STUDENTS_NUMBER]; // исходник
    
    switch (GetMenuAction("Создать массив самостоятельно", "Прочитать массив из файла", "Выберете каким способом заполнить массив"))
    {
        case 1:
            MakeArray(students);
            break;
        case 2: 
            ReadArray(students);
        break;
    }

    switch (GetMenuAction("Напечатать массив в консоль", "Напечатать массив в файл", "Выберете каким способом напечатать массив"))
    {
        case 1:
            PrintArrayConsole(students);
        break;
        case 2:
            PrintArrayFile(students);
        break;
    }

    IndexStudentSurnameName indexStudentSurnameName[STUDENTS_NUMBER];
    MakeIndexStudentSurnameName(indexStudentSurnameName, students);

    HoareSort(indexStudentSurnameName, 0, STUDENTS_NUMBER - 1);
    PrintIndexStudentSurnameName(indexStudentSurnameName, students); 
    
    IndexStudentRating indexStudentRating[STUDENTS_NUMBER];
    MakeIndexIndexStudentRating(indexStudentRating, students);

    string surname;   
    cout << "Введите фамилию для поиска\n";
    cin >> surname;

    string name;
    cout << "Введите имя для поиска\n";
    cin >> name;

    int indexBinarySearch = BinarySearchIndexStudentSurnameNameIter(indexStudentSurnameName, surname, name);
    
    if (indexBinarySearch == -1 || students[indexBinarySearch].isRemoved)
        cout << "Элемент не найден";
    else
        cout << ToString(students[indexBinarySearch]);
    

    

}
