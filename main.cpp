#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <fstream>
#include <string>
#include <mutex>

using namespace std;

int* A; // исходная последовательность
bool isDivided[11]; // массив результатов
int n; // длина числа
int nThread; //количество потоков

std::mutex mut;


struct element
{
	std::thread::id ID;
	int number;
	bool isDiv;

	element()
	{
		number = 0;
		isDiv = false;
	}

	element(std::thread::id _ID, int _number, bool _isDiv)
	{
		ID = _ID;
		number = _number;
		isDiv = _isDiv;
	}
};

struct queue
{
	element* elements;
	int curr_n;
	int size;
	int step = 100;

	queue()
	{
		curr_n = 0;
		size = step;
		elements = new element[size];
	}

	void add(element elem)
	{
		if (curr_n < size)
		{
			elements[curr_n] = elem;
			curr_n++;
		}
		else
		{
			element* new_elements = new element[size];
			for (int i = 0; i < size; i++)
				new_elements[i] = elements[i];
			elements = new element[size + step];
			for (int i = 0; i < size; i++)
				elements[i] = new_elements[i];
			size += step;
		}
	}
	void print()
	{
		cout << "ID потока	|	Число	|	Делится?" << endl;
		for (int i = 0; i < curr_n; i++)
		{
			cout << elements[i].ID << "\t\t\t" << elements[i].number << "\t\t";
			if (elements[i].isDiv)
				cout << "Да" << endl;
			else
				cout << "Нет" << endl;
		}
	}
};




void isDividedBySeven()
{
	int i;
	int number;
	if (A[0] >= 7) {
		i = 1;
		number = A[0];
	}
	else {
		i = 2;
		number = A[0] * 10 + A[1];
	}
	int remains = number % 7;;
	while (i < n) {
		number = remains * 10 + A[i];
		remains = number % 7;
		i++;
	}
	if (remains == 0)
		isDivided[7] = true;
	else
		isDivided[7] = false;
}

queue myQueue1;

void* thread_body(void* arr)
{
	int i = 0;
	while (((int*)arr)[i] != NULL)
	{
		int sum = 0;
		int number = 0;		
		switch (((int*)arr)[i])
		{
		case 2:
			isDivided[2] = (A[n - 1] % 2 == 0);
			break;
		case 3:
			sum = 0;
			for (int j = 0; j < n; j++)
				sum += A[j];
			isDivided[3] = (sum % 3 == 0);
			break;
		case 4:
			number = A[n - 2] * 10 + A[n - 1];
			isDivided[4] = (number % 4 == 0);
			break;
		case 5:
			isDivided[5] = ((A[n - 1] == 0) || (A[n - 1] == 5));
			break;
		case 6:
			sum = 0;
			for (int j = 0; j < n; j++)
				sum += A[j];
			isDivided[6] = ((A[n - 1] % 2 == 0) && (sum % 3 == 0));
			break;
		case 7:
			isDividedBySeven();
			break;
		case 8:
			number = A[n - 2] * 10 * +A[n - 2] * 10 + A[n - 1];
			isDivided[8] = (number % 8 == 0);
			break;
		case 9:
			sum = 0;
			for (int j = 0; j < n; j++)
				sum += A[j];
			isDivided[9] = (sum % 9 == 0);
			break;
		case 10:
			isDivided[10] = (A[n - 1] == 0);
			break;
		}
		mut.lock();
		element elem = element(this_thread::get_id(), ((int*)arr)[i], isDivided[((int*)arr)[i]]);
		myQueue1.add(elem);
		mut.unlock();		

		i++;
	}


	return NULL;
}

int main()
{
	queue myQueue = queue();

	string name = "input.txt";
	string text;
	ifstream input(name);
	if (input.is_open())
		getline(input, text);
	input.close();

	n = text.length();
	A = new int[n];
  cout<<"Число:"<<endl;
	for (int i = 0; i < n; i++){
    cout<<text[i];
		A[i] = text[i] - '0';
  }
	cout << endl;
	//Вводим количество потоков
	nThread = n / 100 + 1;

	//Распределяем количество числе на проверку для каждого потока
	int* c = new int[nThread];
	for (int i = 0; i < nThread; i++)
		c[i] = 9 / nThread; // 9 чисел нужно проверить
	for (int i = 0; i < 9 % nThread; i++)
		c[i]++;
	//Распределяем числа на проверку для каждого потока
	int** arrNumber = new int* [nThread];
	for (int i = 0; i < nThread; i++)
		arrNumber[i] = new int[c[i] + 1]; //в конце массива будет NULL
	int k = 2;
	for (int i = 0; i < nThread; i++) {
		cout << i + 1 << " thread: ";
		for (int j = 0; j < c[i]; j++) {
			arrNumber[i][j] = k++;
			cout << arrNumber[i][j] << " ";
		}
		arrNumber[i][c[i]] = NULL;
		cout << endl;
	}

	pthread_t* thread = new pthread_t[nThread];
	for (int i = 0; i < nThread; i++)
		pthread_create(&thread[i], NULL, thread_body, (void*)arrNumber[i]);

	//Цикл получения и группировки резульатов
	for (int i = 0; i < nThread; i++)
		pthread_join(thread[i], NULL);

	//Вывод результатов
	for (int i = 2; i < 11; i++) {
		printf("%d: ", i);
		if (isDivided[i])
			printf("Да");
		else
			printf("Нет");
		printf("\n");
	}
	myQueue1.print();

	printf("\n\nPress any key...");
	return 0;
}
