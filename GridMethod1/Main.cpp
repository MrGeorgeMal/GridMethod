#include "Main.h"

int main()
{
	Point2D<double> p1(1.0, 1.0), p2(2.2, 2.0);

	Point2D<double> p3 = p1 * p2;

	std::cout << p3;

	std::cout << "\n\n";
	system("pause");
	return 0;
}