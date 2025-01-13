#include "Main.h"

int main()
{
	try
	{
		Point2D<double> p(2.2, 2.8);
		std::cout << p << "\n";

		Shape2D shape(22.2, 1.2);
		std::cout << shape.origin << "\n";

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

		Material mat;
		Dielectric diel;
		Conductor cond(false);

		std::cout << mat << "\n";
		std::cout << diel << "\n";
		std::cout << cond << "\n";
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}