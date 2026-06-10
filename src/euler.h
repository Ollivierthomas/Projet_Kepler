#ifndef EULER_H
#define EULER_H

struct Vecteurs positions(struct Vecteurs r, double h, struct Vecteurs v);

struct Vecteurs vitesses(struct Vecteurs v, double h, struct Vecteurs a);

struct Vecteurs accelerations(double g, double m_soleil, struct Vecteurs r);

#endif 