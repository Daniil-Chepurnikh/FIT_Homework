#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const unsigned short marksNumber = 11; // число предметов для расчёта рейтинга
const unsigned short len = 11;

struct Student // описание студента
{
    unsigned number; // номер зачётки

    string name; // имя
    string surname; // фамилия
    string patronymic; // отчество

    unsigned char marks[marksNumber]; // перечень оценок для расчёта рейтинга
};

struct IndexStudentSurnameName // отчество не добавлено, потому что я не верю, что в моей любимой вышке у кого-то добуквенно совпадут фамилия и имя
{
    short originIndex; // индекс в исходнике
    string surname; // ключевое поле фамилии(сортировать начинаем по нему)
    string name; // надесюсь, поле имени не станет ключевым, но если вдруг совпадут фамилии продолжим сортировать по нему
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

void MakeArrayConsole(Student studs[], int len) // ручное создание массива
{    
    for (int p = 0; p < len; p++)
    {
        studs[p].number = p + 1;

        cout << "Введите имя студента\n";
        cin >> studs[p].name;

        cout << "Введите фамилию студента\n";
        cin >> studs[p].surname;

        cout << "Введите отчество студента\n";
        cin >> studs[p].patronymic;

        for (int q = 0; q < marksNumber; q++)
        {
            studs[p].marks[q] = InputNumber("Введите оценку за предмет\n", "Вы не ввели целое число в допустимом диапазоне от 1 до 10\n", 1, 10);
        }
    }
}

double CalculateRating(Student stud) // вычисление рейтинга для сортировки в будущем
{
    double rating = 0.0;
    for (int p = 0; p < marksNumber; p++)
    {
        rating += stud.marks[p];
    }
    return rating / marksNumber;
}

string ToString(Student stud)
{
    return to_string(stud.number) + " " + stud.name + " " + stud.patronymic + " " + "Рейтинг: " + to_string(CalculateRating(stud)) + '\n';
}

void PrintArrayConsole(Student studs[], int len)
{
    for (int p = 0; p < len; p++)
    {
        cout << ToString(studs[p]);
    }
}

unsigned short GetMenuAction(string option1, string option2, string message) // пришло с лаб по программированию. максимально упрощено
{
    unsigned short action;

    cout << message << '\n';
    cout << "1 " << option1 << '\n';
    cout << "2 " << option2 << '\n';
    cout << "Введите номер выбранного действия (1 или 2): ";

    cin >> action;

    return action;
}

void PrintArrayFile(Student studs[], int len) // магия чистой воды. и сам и железным другом
{ 
     unsigned short action = GetMenuAction("Новый файл", "Существующий файл", "Выберете в какой файл писать");
    
     string fileName = "output.txt";
     switch (action)
     {
        case 1:
        {
            cout << "Как нужно назвать новый файл?"; // сам придумал так сделать. больше удобства
            cin >> fileName;
            fileName = fileName + ".txt";
            
            ofstream outFile(fileName); // говорим какой файл надо создать и открыть

            if (outFile.is_open()) // проверяем, открылся ли файл
            {
                for (int p = 0; p < len; p++) // пишем в файл
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
                for (int p = 0; p < len; p++) // пишем в файл
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
}

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

void HoareSort(IndexStudentSurnameName array[], int left, int right)
{
    if (left < right)
    {
        int pivotIndex = Partition(array, left, right);
        HoareSort(array, left, pivotIndex - 1);
        HoareSort(array, pivotIndex + 1, right);
    }
}

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8"); // была какая-то жесть с выводом и товарищ интернет сказал сделать так
    system("chcp 65001"); // чтобы писались нормальные русские слова

    
    Student students[len];
    MakeArrayConsole(students, len);

    PrintArrayConsole(students, len);

}
