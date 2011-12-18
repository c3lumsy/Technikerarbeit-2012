// void axis_move_circle(int16_t x0, int16_t y0, uint16_t r,uint8_t direction, uint16_t AxisSpeed)
// {
// 	//Beispiel eines Algorithmus zum Fräsen eines Kreissegments
// 	//Mit den eingestellten Werten wird ein Dreiviertelkreis im Uhrzeigersinn ausgegeben
// 
// 	//Die Startposition
// 	//damit der Algorithmus funktioniert muss x oder y zu Beginn 0, bzw r sein
// 	//der Fräser muss also genau zwischen zwei Quadranten stehen, da sonst das anfängliche h nicht stimmt
//   
//   int16_t x=0, y=r;
// 
//   //Der punkt bis zu dem der Fräser fahren soll
//   int16_t xsoll=x0-r;
//   int16_t ysoll=y0;
//   
//   //Hilfsvariablen
//   int16_t h=r, a=0, b=0, c=0, d=0;
//     
//   do
// 	{
//     
//     //Hier wird ermittelt, in welchem Quadranten sich der Fräser gerade befindet
// 		uint8_t sector = (2 * (y >= 0) + (x >= 0));
// 	//Wenn sich der Fräster genau zwischen zwei Quadranten befindet wird h zurückgesetzt
// 		if (x == 0 || y == 0)
// 				h = r;
// 	//Je nachdem in welchem Quadranten sich der Fräser befindet werden a und b unterschiedlich belegt
// 	switch (sector)
// 	{
// 		case 3: a = x; b = y; break;	//1.Quadrant
// 		case 2: a = y; b = -x; break;	//2.Quadrant
// 		case 0: a = -x; b = -y; break;  //3.Quadrant
// 		case 1: a = -y; b = x; break;	//4.Quadrant
// 	}
// 
//     //Wenn gegen den Uhrzeigersinn gefahren werden soll werden a und b vertauscht
// 	if (direction)
// 		{
// 			int temp=a;
// 			a=b;
// 			b=temp;
// 		}
// 
//     //Eigentlicher Bresenham Algorithmus für die erste 45° des Viertelkreises
//                 if (b > a)
//                 {
//                     if (h < 0)
//                         h += (2 * --b +2);
//                     h -= (2 * ++a +1);
//                 }
//     //Algorithmus für die anderen 45° des Viertelkreises
//                 else
//                 {
//                     if (h < 0)
//                         h += (2 * ++a +2);
//                     h -= (2 * --b -1);
//                 }
// 
//     //Wenn gegen den Uhrzeigersinn gefahren werden soll werden a und b vertauscht
//                 if (direction)
//                 {  
//         int temp=a;
//         a=b;
//         b=temp;
//                 }
//               
//     //Je nachdem in welchem Quadranten sich der Fräser befindet werden x und y unterschiedlich belegt
//               switch (sector)
//                 {
//                     case 3: x = a; y = b; break;
//                     case 2: x = -b; y = a; break;
//                     case 0: x = -a; y = -b; break;
//                     case 1: x = b; y = -a; break;
//                 }
// 
//     //Pseudofunktion, die die Schrittmotoren auf die gewünschte Position setzt
//                 axis_move_parallel(x,y,AxisSpeed);
// 
//                }
//             while ((x)!=xsoll  && (y)!=ysoll);
// }