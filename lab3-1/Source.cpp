#include <mpi.h>
#include <iostream>
#include <time.h>
#include <math.h>

using namespace std;

enum Operation { ADD, SUBTRACT, MULTIPLY, DIVIDE };

int msgForZero[3] = {5, 45, ADD};
int msgForN[3] = { 0, 73, SUBTRACT };

void makeOperation(int input[3], int output[3]) {
	//�������� �����
	float leftOperand = *input;
	float rightOperand = rand() % 9 + rand() / pow(10, 4);
	float result = 0;

	int decimals = 0;
	int temp = *(input + 1);
	while (temp) {
		decimals++;
		temp /= 10;
	};

	leftOperand += *(input + 1) / pow(10, decimals);

	switch (*(input + 2)) {
	case ADD:
		result = leftOperand + rightOperand;
		break;
	case SUBTRACT:
		result = leftOperand - rightOperand;
		break;
	case MULTIPLY:
		result = leftOperand * rightOperand;
		break;
	case DIVIDE:
		result = leftOperand / rightOperand;
		break;
	};

	//��������� �����
	*(output + 2) = *(input + 2);
	*(output) = (int)floor(result);
	*(output + 1) = (int)((result - floor(result)) * pow(10, decimals));
};

int main(int argc, char* argv[]) {
	srand(time(0));
	setlocale(LC_ALL, "Russian");
	int* message = nullptr;
	int buffer1[3] = { 0, 0, 0 };

	MPI_Init(&argc, &argv);
	int proc_count, received_rank;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &received_rank);

	if (!(proc_count % 2)) {
		cout << "������! ����� ��������� ������ ���� ��������." << endl;
	}
	else {
		MPI_Datatype userType;
		MPI_Type_contiguous(3, MPI_INT, &userType);
		MPI_Type_commit(&userType);

		if (!received_rank || (received_rank == proc_count - 1)) {
			if (!received_rank) {
				message = msgForZero;
			}
			else {
				message = msgForN;
			};

			cout << "������� " << received_rank << " ���������� ����� " << *message << "." << *(message + 1) << " � ��������� ";
			switch (*(message + 2)) {
			case ADD:
				cout << "��������.";
				break;
			case SUBTRACT:
				cout << "���������.";
				break;
			case MULTIPLY:
				cout << "���������.";
				break;
			case DIVIDE:
				cout << "�������.";
				break;
			default:
				cout << "������������ ��������.";
				MPI_Finalize();
				return 0;
			};
			cout << endl;

			if (!received_rank) {
				MPI_Send(message, 1, userType, 1, 0, MPI_COMM_WORLD);
				cout << "������� " << received_rank << " �������� ������ �������� 1." << endl;
			}
			else {
				MPI_Send(message, 1, userType, received_rank - 1, 0, MPI_COMM_WORLD);
				cout << "������� " << received_rank << " �������� ������ �������� " << (received_rank - 1) << "." << endl;
			};
		}
		else {
			int answer[3] = { 0, 0, 0 };
			if(received_rank < (proc_count / 2 - 1)) {
				MPI_Recv(buffer1, 1, userType, received_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				cout << "������� " << received_rank << " ������� �� �������� " << received_rank - 1 << " ����� " << *buffer1 << "." << *(buffer1 + 1) << " � ��������� ";
				switch (*(buffer1 + 2)) {
				case ADD:
					cout << "��������.";
					break;
				case SUBTRACT:
					cout << "���������.";
					break;
				case MULTIPLY:
					cout << "���������.";
					break;
				case DIVIDE:
					cout << "�������.";
					break;
				default:
					cout << "������������ ��������.";
				};
				cout << endl;

				makeOperation(buffer1, answer);
				MPI_Send(answer, 1, userType, received_rank + 1, 0, MPI_COMM_WORLD);
				cout << "������� " << received_rank << " �������� ������ �������� " << (received_rank + 1) << "." << endl;
			}
			else {
				if (received_rank > (proc_count / 2 - 1)) {
					MPI_Recv(buffer1, 1, userType, received_rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
					cout << "������� " << received_rank << " ������� �� �������� " << received_rank + 1 << " ����� " << *buffer1 << "." << *(buffer1 + 1) << " � ��������� ";
					switch (*(buffer1 + 2)) {
					case ADD:
						cout << "��������.";
						break;
					case SUBTRACT:
						cout << "���������.";
						break;
					case MULTIPLY:
						cout << "���������.";
						break;
					case DIVIDE:
						cout << "�������.";
						break;
					default:
						cout << "������������ ��������.";
					};
					cout << endl;

					makeOperation(buffer1, answer);
					MPI_Send(answer, 1, userType, received_rank - 1, 0, MPI_COMM_WORLD);
					cout << "������� " << received_rank << " �������� ������ �������� " << (received_rank - 1) << "." << endl;
				}
				else {
					MPI_Recv(buffer1, 1, userType, received_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
					cout << "������� " << received_rank << " ������� �� �������� " << received_rank - 1 << " ����� " << *buffer1 << "." << *(buffer1 + 1) << endl;
					makeOperation(buffer1, answer);
					cout << "���������� ����� �� �������� " << received_rank - 1 << ": " << *buffer1 << "." << *(buffer1 + 1) << endl;

					MPI_Recv(buffer1, 1, userType, received_rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
					cout << "������� " << received_rank << " ������� �� �������� " << received_rank + 1 << " ����� " << *buffer1 << "." << *(buffer1 + 1) << endl;
					makeOperation(buffer1, answer);
					cout << "���������� ����� �� �������� " << received_rank + 1 << ": " << *buffer1 << "." << *(buffer1 + 1) << endl;
					cout << "������� " << received_rank << " ��������� ��������." << endl;
				};
			};
		};
		MPI_Type_free(&userType);
	};

	MPI_Finalize();
	return 0;
};