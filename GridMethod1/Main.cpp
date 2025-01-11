#include "Main.h"

int main()
{
	Matrix2D<int> m(2, 2);

	int k = 0;
	for (int i = 0; i < m.getRows(); i++)
	{
		for (int j = 0; j < m.getCols(); j++)
		{
			m[i][j] = k;
			k++;
		}
	}

	for (int i = 0; i < m.getRows(); i++)
	{
		for (int j = 0; j < m.getCols(); j++)
		{
			std::cout << m[i][j] << " ";
		}
		std::cout << "\n";
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}