#include "Main.h"

int main()
{
	try
	{		
		Dielectric* diel1 = new Dielectric(4.3);
		Dielectric* diel2 = new Dielectric(3.2);
		Dielectric* air = new Dielectric(1.0);
		Conductor* cond1 = new Conductor(false);
		Conductor* cond2 = new Conductor(true);

		Line2D line1;
		Line2D line2;
		Polygon2D poly1;
		Polygon2D poly2;
		Rectangle2D rect1;

		line1.setP1(Point2D<double>(5.0, 15.0));
		line1.setP2(Point2D<double>(20.0, 5.0));

		line2.setP1(Point2D<double>(30.0, 20.0));
		line2.setP2(Point2D<double>(45.0, 20.0));

		poly1.addPoints(Point2D<double>(5.0, 10.0));
		poly1.addPoints(Point2D<double>(10.0, 5.0));
		poly1.addPoints(Point2D<double>(5.0, 5.0));

		poly2.addPoints(Point2D<double>(20.0, 10.0));
		poly2.addPoints(Point2D<double>(30.0, 15.0));
		poly2.addPoints(Point2D<double>(40.0, 5.0));
		poly2.addPoints(Point2D<double>(20.0, 5.0));

		rect1.setPoint(Point2D<double>(15.0, 20.0));
		rect1.setSize(Size2D<double>(10.0, 5.0));
		rect1.makeAsScreen(false);

		line1.setMaterial(cond1);
		line2.setMaterial(cond2);
		poly1.setMaterial(diel1);
		poly2.setMaterial(diel1);
		rect1.setMaterial(diel1);

		StripStructure strip;
		strip.addShape(&poly1);
		strip.addShape(&poly2);
		strip.addShape(&rect1);
		strip.addShape(&line1);
		strip.addShape(&line2);

		strip.computeElectroStaticAnalysis();
	}
	catch (const char* errorMsg)
	{
		std::cout << errorMsg;
	}

	std::cout << "\n\n";
	system("pause");
	return 0;
}