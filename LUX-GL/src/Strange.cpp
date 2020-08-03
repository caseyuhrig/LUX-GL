#include <math.h>

// STRANGE
   /*
   const int iterations = 10000;
   float strange_points[iterations * 5];
   float abcd[4] = { -0.966918f, 2.879879f, 0.765145f, 0.744728f };
   float efgh[4];

   make_strange(abcd, iterations, strange_points);


   VertexArray vaStrange;

   VertexBuffer vbStrange = { strange_points, num_cnt * iterations * sizeof(float) };
   vaStrange.AddBuffer(vbStrange, layout);


   unsigned int indiciesStrange[iterations];
   for (int n = 0;n < iterations;n++)
   {
       indiciesStrange[n] = n;
   }
   IndexBuffer ibStrange(indiciesStrange, iterations);
   */
void make_strange(float abcd[4], int iterations, float results[])
{


    float	x = 0.1, y = 0.1,		// starting point
        & a = abcd[0], //-0.966918,			// coefficients for "The King's Dream"
        & b = abcd[1], //2.879879,
        & c = abcd[2], //0.765145,
        & d = abcd[3]; //0.744728;
    int	initialIterations = 100;//,	// initial number of iterations
                        // to allow the attractor to settle
        //iterations = 100000;		// number of times to iterate through
                        // the functions and draw a point

    // compute some initial iterations to settle into the orbit of the attractor
    for (int i = 0; i < initialIterations; i++) {

        // compute a new point using the strange attractor equations
        float xnew = sin(y * b) + c * sin(x * b);
        float ynew = sin(x * a) + d * sin(y * a);

        // save the new point
        x = xnew;
        y = ynew;
    }

    // go through the equations many times, drawing a point for each iteration
    for (int i = 0; i < iterations * 5; i += 5) {

        // compute a new point using the strange attractor equations
        float xnew = sin(y * b) + c * sin(x * b);
        float ynew = sin(x * a) + d * sin(y * a);

        // save the new point
        x = xnew;
        y = ynew;

        // draw the new point
        //glVertex2f(x, y);
        results[i] = x;
        results[i + 1] = y;
        results[i + 2] = 0.0;
        // unused tex mapping coords
        results[i + 3] = 0.0;
        results[i + 4] = 0.0;
    }

}