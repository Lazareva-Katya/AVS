//Лабораторная работа №1. Вариант 10.

#include <iostream>
using namespace std;

void print_arr(__int8* A, __int8* B, __int8* C, __int16* D, __int16* X, bool n)
{
	int i = 0;

	while (i < 8) 
	{
		if (n == true)
			X[i] = (A[i] + B[i]) * (C[i] + D[i]);
		cout << X[i] << "\t";
		i++;
	}
	cout << endl;
}

void check(__int8* A, __int8* B, __int8* C, __int16* D) 
{
	__int16 arr[8] = { 0 };
	print_arr(A, B, C, D, arr, true);
}

void task(__int8* A, __int8* B, __int8* C, __int16* D)
{
	__int16 F[8] = { 0 };
	__asm 
	{
		mov ESI, A
		movq mm2, [ESI]

		mov ESI, B
		movq mm3, [ESI]

		paddb mm2, mm3

		movq mm4, mm2
		pcmpgtb mm5, mm4
		punpcklbw mm4, mm5
		movq mm3, mm4

		movq mm4, mm2
		pcmpgtb mm5, mm4
		punpckhbw mm4, mm5
		movq mm6, mm4

		mov ESI, D
		movq mm0, [ESI]
		mov ESI, D
		movq mm1, [ESI + 2 * 4]

		mov ESI, C
		movq mm2, [ESI]

		movq mm4, mm2
		pcmpgtb mm5, mm4
		punpcklbw mm4, mm5
		movq mm7, mm4

		movq mm4, mm2
		pcmpgtb mm5, mm4
		punpckhbw mm4, mm5
		movq mm5, mm4

		paddw mm0, mm7
		paddw mm1, mm5

		pmullw mm3, mm0
		pmullw mm6, mm1

		movq[F], mm3
		movq[F + 2 * 4], mm6
	}

	print_arr(A, B, C, D, F, false);
}

void print_res(__int8* A, __int8* B, __int8* C, __int16* D)
{
	cout << "Task:\t";
	task(A, B, C, D);
	cout << "Check:\t";
	check(A, B, C, D);
}

void example() 
{
	__int8 A[8]{ -219, 11, -364, 110, -55, 580, 12, 83 };
	__int8 B[8]{ 199, 540, -15, -26, 16, 1, 14, -5 };
	__int8 C[8]{ 1, 2, -3, -4, 5, 6, -7, 8 };
	__int16 D[8]{ -1999, 0, -3999, 4999, 5999, -6999, 7999, 8999 };
	print_res(A, B, C, D);
}

int main()
{
	example();
	return 0;
}

