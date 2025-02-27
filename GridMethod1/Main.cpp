#include "Main.h"

int main()
{
	try
	{		
		Dielectric* diel = new Dielectric(4.5);
		Conductor* signalCond = new Conductor(true);
		Conductor* screenCond = new Conductor(false);

		Line2D* line1 = new Line2D();
		Line2D* line2 = new Line2D();
		Line2D* line3 = new Line2D();
		Rectangle2D* rect1 = new Rectangle2D();

		rect1->setPoint(Point2D<double>(0.0, 0.0));
		rect1->setSize(Size2D<double>(10.0, 1.5));
		rect1->makeAsScreen(false);

		line1->setP1(Point2D<double>(2.0, 1.5));
		line1->setP2(Point2D<double>(4.5, 1.5));

		line2->setP1(Point2D<double>(5.5, 1.5));
		line2->setP2(Point2D<double>(8, 1.5));

		line3->setP1(Point2D<double>(0.0, 0.0));
		line3->setP2(Point2D<double>(0.0, 1.5));

		line1->setMaterial(signalCond);
		line2->setMaterial(signalCond);
		line3->setMaterial(screenCond);
		rect1->setMaterial(diel);

		StripStructure strip;
		strip.addShape(rect1);
		strip.addShape(line1);
		strip.addShape(line2);
		strip.addShape(line3);

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