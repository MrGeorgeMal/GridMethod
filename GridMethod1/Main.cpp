#include "Main.h"

int main()
{
	try
	{
		StructureCreator::createStripStructureFromJson("C:/Users/mrmag/Desktop/strip_structure_1.json");

		Dielectric* diel1 = new Dielectric(9.8);
		Dielectric* diel2 = new Dielectric(6.5);
		Conductor* signalCond = new Conductor(true);
		Conductor* screenCond = new Conductor(false);

		Line2D* line1 = new Line2D();
		Line2D* line2 = new Line2D();
		Line2D* line3 = new Line2D();
		Line2D* line4 = new Line2D();
		Rectangle2D* rect1 = new Rectangle2D();
		Rectangle2D* rect2 = new Rectangle2D();

		double width = 9.0;
		double w1 = 0.25;
		double w2 = 0.25;
		double s = 0.2;
		double w3 = w1 + w2 + s;
		double h1 = 1;
		double h2 = 0.2;

		rect1->setPoint(Point2D<double>(0.0, 0.0));
		rect1->setSize(Size2D<double>(width, h1));
		rect1->makeAsScreen(false);

		rect2->setPoint(Point2D<double>(width / 2 - s / 2 - w1, h1));
		rect2->setSize(Size2D<double>(w3, h2));
		rect2->makeAsScreen(false);

		line1->setP1(Point2D<double>(width / 2 - s / 2 - w1, h1));
		line1->setP2(Point2D<double>(width / 2 - s / 2, h1));

		line2->setP1(Point2D<double>(width / 2 + s / 2, h1));
		line2->setP2(Point2D<double>(width / 2 + s / 2 + w2, h1));

		line3->setP1(Point2D<double>(width / 2 - s / 2 - w1, h1 + h2));
		line3->setP2(Point2D<double>(width / 2 + s / 2 + w2, h1 + h2));

		line4->setP1(Point2D<double>(0.0, 0.0));
		line4->setP2(Point2D<double>(width, 0.0));
		
		line1->setMaterial(signalCond);
		line2->setMaterial(signalCond);
		line3->setMaterial(signalCond);
		line4->setMaterial(screenCond);
		rect1->setMaterial(diel1);
		rect2->setMaterial(diel2);

		StripStructure strip;
		strip.addShape(rect1);
		strip.addShape(rect2);
		strip.addShape(line1);
		strip.addShape(line2);
		strip.addShape(line3);
		strip.addShape(line4);

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