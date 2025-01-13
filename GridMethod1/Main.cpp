#include "Main.h"

int main()
{
	try
	{
		Matrix2D<int> m(2, 2);
		Vector<int> v(5);

		for (int i = 0; i < v.getLength(); i++)
		{
			v[i] = i;
		}
		std::cout << v << "\n";

		int k = 0;
		for (int i = 0; i < m.getRows(); i++)
		{
			for (int j = 0; j < m.getCols(); j++)
			{
				m[i][j] = k;
				k++;
			}
		}
		std::cout << m << "\n";

		Dielectric d(2.1);

		std::cout << d << "\n";
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}