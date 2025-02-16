#include "Main.h"

int main()
{
	try
	{
		Point2D<double> p(2.2, 2.8);
		std::cout << p << "\n";

		Shape2D shape(22.2, 1.2);
		std::cout << shape.getOrigin() << "\n";

		Rectangle2D rect;

		Polygon polygon(
			Point2D<double>(0.0, 0.0),
			Point2D<double>(1.1, 1.1),
			Point2D<double>(2.2, 2.2),
			Point2D<double>(3.3, 3.3),
			Point2D<double>(4.4, 4.4));
		std::cout << polygon.getPoints() << "\n";

		Matrix2D<int> m(2, 2);
		Vector<Point2D<double>> v(2);

        for (int i = 0; i < v.getLength(); i++)
        {
			v[i] = Point2D<double>(double(i) / 10.0, double(i) / 10.0);
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